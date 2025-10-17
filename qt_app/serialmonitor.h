#ifndef SERIALMONITOR_H
#define SERIALMONITOR_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QSerialPort>
#include <QTextEdit>
#include "actionset.h"

namespace Ui {
class SerialMonitor;
}

/*------------------------------------------------------------------------------
 * SerialMonitor
 *------------------------------------------------------------------------------
 * Purpose
 *   - Diagnostic window for observing framed protocol traffic over the serial
 *     link (both TX and RX).
 *   - Provides colored/structured rendering of protocol frames, payloads,
 *     CRCs, and (optionally) decoded action graphs and CSPI payloads.
 *
 * Responsibilities
 *   - Attach to a QSerialPort (non-owning) and consume its readyRead() stream.
 *   - Buffer partial reads, parse protocol frames, and append formatted lines.
 *   - Emit cspiReqReceived() when a device-side CSPI REQ is detected.
 *   - Allow toggling between Hex view and ASCII line view.
 *
 * Lifetime / Ownership
 *   - Owns Ui::SerialMonitor* (generated form).
 *   - Does NOT own the QSerialPort; caller must keep it alive while attached.
 *   - Maintains a scoped connection to the port’s readyRead().
 *
 * Threading
 *   - GUI thread only. QSerialPort notifications come on the same thread.
 *----------------------------------------------------------------------------*/
class SerialMonitor : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Construct the monitor widget and initialize UI.
     */
    explicit SerialMonitor(QWidget *parent = nullptr);

    /**
     * @brief Destroy the monitor and release UI resources.
     */
    ~SerialMonitor();

    /**
     * @brief Wire the monitor to an existing serial port.
     *        Creates a readyRead() connection and begins parsing incoming data.
     *
     * @param port Non-owning pointer to an open QSerialPort.
     */
    void attachPort(QSerialPort *port);

    /**
     * @brief Detach from the current serial port and drop signal connections.
     */
    void detachPort();

    /**
     * @brief Log a transmitted packet (host→device).
     *        Optionally provides an ActionSet to enable decoded coloring
     *        of action payloads.
     *
     * @param packet  Full framed packet (SOF..CRC).
     * @param set     Optional action set used for pretty-printing (may be null).
     */
    void logTx(const QByteArray &packet, const ActionSet* set);

protected:
    /**
     * @brief Ensure scrolling and layout behavior when the widget becomes visible.
     */
    void showEvent(QShowEvent *e) override;

signals:
    /**
     * @brief Emitted when an incoming CSPI REQ frame is observed from the device.
     *        The main window typically responds by sending the next data chunk.
     */
    void cspiReqReceived();

private slots:
    /**
     * @brief Slot connected to QSerialPort::readyRead(). Reads bytes,
     *        buffers partial frames, and appends parsed/colored output.
     */
    void handleReadyRead();

    /**
     * @brief Toggle between hex-dump view and ASCII line view.
     * @param on True → hex; False → ASCII.
     */
    void onHexToggled(bool on);

    /**
     * @brief Clear the monitor contents (UI button handler).
     */
    void on_clearButton_clicked();

private:
    /*--------------------------- Formatting helpers ------------------------*/
    /**
     * @brief Convert raw bytes to spaced uppercase hex string.
     */
    QString bytesToHex(const QByteArray &data) const;

    /**
     * @brief Render protocol header (SOF/ID/LEN) with color spans.
     */
    QString coloredHeader(const QByteArray& packet) const;

    /**
     * @brief Render trailing CRC with pass/fail coloring when known.
     */
    QString coloredCrc(const QByteArray& packet) const;

    /**
     * @brief Pretty-print an actions payload using IDs/nodes from @actions.
     */
    QString coloredActions(const QByteArray& payload,
                           const std::vector<Action*>& actions) const;

    /**
     * @brief Pretty-print a CSPI header/payload (human-friendly breakdown).
     */
    QString coloredCSPI(const QByteArray& payload) const;

    /**
     * @brief Pretty-print CSPI bulk-begin header fields.
     */
    QString coloredBulkBeginCSPI(const QByteArray& hdr) const;

    /**
     * @brief Pretty-print a CSPI bulk-data packet (length, preview, etc.).
     */
    QString coloredBulkData(const QByteArray& payload) const;

    /**
     * @brief Pretty-print CSPI bulk-end marker/line.
     */
    QString coloredBulkEnd() const;

    /**
     * @brief Return action pointers sorted by id ascending (stable view).
     */
    std::vector<Action*> idAscending(const ActionSet* set) const;

    /**
     * @brief Append a fully formatted line to the monitor (hex or rich HTML).
     */
    void appendLine(const QByteArray &data);

    /**
     * @brief Append ASCII lines by splitting incoming chunk on '\n' boundaries.
     */
    void appendAsciiLines(const QByteArray &chunk);

    /*--------------------------- UI / Port state ---------------------------*/
    Ui::SerialMonitor *ui = nullptr;          ///< Generated form (owned).
    QSerialPort *m_port = nullptr;            ///< Attached serial (non-owning).
    QMetaObject::Connection m_conn;           ///< Scoped connection to readyRead().

    /*--------------------------- View / Parse state ------------------------*/
    bool m_hexView = true;                    ///< True → hex; False → ASCII view.
    bool m_timestamp = true;                  ///< Prepend timestamps to lines.
    bool m_dropInitialPending = false;        ///< Drop stale port buffer on attach.
    QByteArray m_lineBuf;                     ///< For ASCII line accumulation.

    /**
     * @brief Convert a byte array to a single line of hex with spacing/grouping.
     */
    QString bytesToHexLine(const QByteArray &data) const;

    QByteArray m_protoBuf;                    ///< RX buffer for protocol frame reassembly.

    /**
     * @brief Parse framed protocol messages from @chunk (append to m_protoBuf).
     *        Emits cspiReqReceived() when a CSPI REQ is decoded.
     */
    void parseProtoFrames(const QByteArray& chunk);
};

#endif // SERIALMONITOR_H
