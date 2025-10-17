#include "serialmonitor.h"
#include "ui_serialmonitor.h"
#include "actionEncoder.h"
#include <QDateTime>

/*
 * SerialMonitor
 * -------------
 * Lightweight serial terminal widget with optional protocol awareness.
 *
 * Responsibilities:
 *  - Attach/detach a QSerialPort and stream incoming bytes into the UI.
 *  - Render either hex-dumped frames or printable ASCII lines with timestamps.
 *  - Incrementally parse custom UART protocol frames (AA 55 ... CRC16).
 *  - Emit cspiReqReceived() when a valid MSG_ID_CSPI_REQ (len=0) is observed.
 *
 * Rendering:
 *  - Hex view: single line per incoming chunk (uppercase hex, spaced).
 *  - ASCII view: line-buffered; appends only complete lines terminated by '\n'.
 *
 * Protocol parsing:
 *  - A simple re-sync strategy: search for 0xAA 0x55, verify minimum size,
 *    extract msg/len, wait for full frame, validate CRC16-CCITT, consume
 *    the frame or drop noise and continue.
 */

SerialMonitor::SerialMonitor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SerialMonitor)
{
    ui->setupUi(this);

    // Toggle between Hex and ASCII render modes.
    connect(ui->checkBoxHex, &QCheckBox::toggled,
            this, &SerialMonitor::onHexToggled);

    // Default to ASCII view (unchecked).
    ui->checkBoxHex->setChecked(false);
    m_hexView = false;
}

SerialMonitor::~SerialMonitor()
{
    detachPort();
    delete ui;
}

/**
 * Attach a serial port to the monitor.
 * - Clears any pending bytes in the port input buffer.
 * - Connects readyRead() to our handler for incremental consumption.
 */
void SerialMonitor::attachPort(QSerialPort *port) {
    if (m_port == port) return;
    detachPort();
    m_port = port;
    if (m_port) {
        m_port->readAll(); // drop stale data to avoid mixing previous session

        m_conn = connect(m_port, &QSerialPort::readyRead,
                         this,   &SerialMonitor::handleReadyRead);
    }
}

/**
 * Detach the currently attached serial port (if any).
 * - Disconnects the readyRead() handler and clears the handle.
 */
void SerialMonitor::detachPort() {
    if (m_port) {
        if (m_conn) QObject::disconnect(m_conn);
        m_conn = QMetaObject::Connection();
        m_port = nullptr;
    }
}

/**
 * Qt show event override.
 * - Currently unused; kept for future UI preparations on show.
 */
void SerialMonitor::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
}

/* ------------------------- Protocol helpers ------------------------- */

/* One-step CRC16-CCITT update (poly 0x1021, init 0xFFFF). */
static inline quint16 crc16_step(quint16 crc, quint8 b) {
    crc ^= (quint16)b << 8;
    for (int i=0; i<8; ++i)
        crc = (crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1);
    return crc;
}

/* Compute CRC16-CCITT over [msgId, len_hi, len_lo, payload...] */
static quint16 crc16_calc(quint8 msgId, quint16 len, const quint8* pl) {
    quint16 crc = 0xFFFF;
    crc = crc16_step(crc, msgId);
    crc = crc16_step(crc, (quint8)((len >> 8) & 0xFF));
    crc = crc16_step(crc, (quint8)(len & 0xFF));
    for (quint16 i=0; i<len; ++i) crc = crc16_step(crc, pl[i]);
    return crc;
}

/**
 * Incremental protocol frame parser.
 *
 * Buffering strategy:
 *  - Append new chunk to m_protoBuf.
 *  - Loop:
 *      * Find SOF0 (0xAA). If not found -> clear buffer (noise) and return.
 *      * Verify SOF1 (0x55) next; otherwise skip this 0xAA and continue.
 *      * Ensure minimum frame size (7 bytes): SOF(2)+MSG(1)+LEN(2)+CRC(2).
 *      * If full frame not yet available (by LEN), return (wait for more).
 *      * Validate CRC; if OK, handle message (emit signal for CSPI_REQ),
 *        then remove the frame; else drop the first SOF and resync.
 */
void SerialMonitor::parseProtoFrames(const QByteArray& chunk)
{
    m_protoBuf += chunk;

    for (;;) {
        // Search for SOF
        int sof = m_protoBuf.indexOf(char(SOF0));
        if (sof < 0) { m_protoBuf.clear(); return; }
        if (sof + 1 >= m_protoBuf.size()) return;

        if ((quint8)m_protoBuf[sof] != SOF0 || (quint8)m_protoBuf[sof+1] != SOF1) {
            // 0xAA was found but not followed by 0x55 -> advance past this 0xAA
            m_protoBuf.remove(0, sof + 1);
            continue;
        }

        // Minimum frame: SOF(2) + msg(1) + len(2) + crc(2) = 7 bytes
        if (m_protoBuf.size() < sof + 7) return;

        const int base = sof + 2;
        quint8  msg = (quint8)m_protoBuf[base + 0];
        quint16 len = ((quint8)m_protoBuf[base + 1] << 8) | (quint8)m_protoBuf[base + 2];

        const int frameLen = 2 + 1 + 2 + len + 2; // SOF + header + payload + CRC
        if (m_protoBuf.size() < sof + frameLen) return; // incomplete frame -> wait more

        const char* plPtr = m_protoBuf.constData() + base + 3;
        quint16 rxCrc = ((quint8)m_protoBuf[sof + frameLen - 2] << 8)
                        | (quint8)m_protoBuf[sof + frameLen - 1];

        quint16 calc = crc16_calc(msg, len, reinterpret_cast<const quint8*>(plPtr));

        if (rxCrc == calc) {
            // Valid frame: react to known messages
            if (msg == MSG_ID_CSPI_REQ && len == 0) {
                emit cspiReqReceived();     // notify UI/app logic about CSPI refill request
            }
            // (Extend here to handle/log other message types if needed.)

            // Consume the frame and continue scanning (there might be more)
            m_protoBuf.remove(sof, frameLen);
        } else {
            // CRC mismatch: drop this SOF and attempt resynchronization
            m_protoBuf.remove(0, sof + 1);
        }
    }
}

/* ----------------------- Serial data ingestion ---------------------- */

/**
 * readyRead() handler.
 *  - Drain all currently available bytes.
 *  - Render as hex or ASCII depending on UI toggle.
 *  - Feed the protocol parser for out-of-band control messages.
 */
void SerialMonitor::handleReadyRead() {
    if (!m_port) return;

    QByteArray data = m_port->readAll();
    while (m_port->bytesAvailable() > 0)
        data += m_port->readAll();

    if (data.isEmpty()) return;

    if (m_hexView) {
        appendLine(data);
    } else {
        appendAsciiLines(data);
    }

    parseProtoFrames(data);
}

/**
 * Hex/ASCII toggle slot.
 *  - When entering Hex mode, clear the line buffer used by ASCII assembly.
 */
void SerialMonitor::onHexToggled(bool on)
{
    m_hexView = on;
    if (m_hexView) m_lineBuf.clear();
}

/**
 * Append one line in hex mode.
 *  - Uppercase hex, single space between bytes, optional timestamp prefix.
 */
void SerialMonitor::appendLine(const QByteArray &data) {
    QString line;
    if (m_timestamp) {
        line += QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
    }
    line += QString::fromLatin1(data.toHex(' ').toUpper());
    ui->textEdit->append(line);
}

/**
 * Append zero or more lines in ASCII mode.
 *  - Accumulate bytes until '\n'; strip optional trailing '\r'.
 *  - Prepend timestamp if enabled.
 *  - Only append to UI when RX checkbox is checked.
 */
void SerialMonitor::appendAsciiLines(const QByteArray &chunk)
{
    m_lineBuf.append(chunk);

    for (;;) {
        int nl = m_lineBuf.indexOf('\n');
        if (nl < 0) break;

        QByteArray one = m_lineBuf.left(nl);
        m_lineBuf.remove(0, nl + 1);

        if (!one.isEmpty() && one.back() == '\r')
            one.chop(1);

        QString line;
        if (m_timestamp) {
            line += QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
        }
        line += QString::fromUtf8(one);

        if (ui->rxCheckBox->isChecked())
            ui->textEdit->append(line);
    }
}

/**
 * Clear button handler: wipe the monitor contents.
 */
void SerialMonitor::on_clearButton_clicked()
{
    ui->textEdit->clear();
}
