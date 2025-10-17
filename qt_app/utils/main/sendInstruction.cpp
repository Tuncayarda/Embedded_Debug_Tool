#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"
#include "../../actionEncoder.h"
#include <QtSerialPort/QSerialPort>
#include <algorithm>

/*
 * Write the full `data` buffer to the serial port, waiting for the device
 * to drain its TX FIFO between partial writes.
 *
 * - QSerialPort::write() may return before all bytes are queued.
 * - We loop until every byte is accepted by the driver.
 * - After each write (including zero-byte progress), we call
 *   waitForBytesWritten(perChunkTimeoutMs) to block until the OS/hardware
 *   confirms the bytes were handed off, or a timeout occurs.
 *
 * Returns:
 *   true  on success (all bytes written and flushed)
 *   false on error or timeout
 */
bool MainWindow::writeAllWithTimeout(const QByteArray& data, int perChunkTimeoutMs)
{
    qint64 total = 0;
    const qint64 len = data.size();
    const char*  p   = data.constData();

    while (total < len) {
        // Try to write the remaining bytes
        const qint64 n = serial.write(p + total, len - total);
        if (n < 0) return false;                           // QSerialPort error
        if (n == 0) {
            // No progress yet; wait for the driver to push pending data out
            if (!serial.waitForBytesWritten(perChunkTimeoutMs)) return false;
            continue;
        }

        total += n;

        // Ensure the just-written bytes are actually pushed out by the driver
        if (!serial.waitForBytesWritten(perChunkTimeoutMs)) return false;
    }
    return true;
}

/*
 * Frame and send one protocol packet over serial.
 *
 * - Validates that the serial port is open.
 * - Builds the packet using the UART protocol (SOF + msg + len + payload + CRC).
 * - Uses writeAllWithTimeout() to ensure complete transmission.
 * - Flushes the port to minimize latency for small packets.
 * - If a SerialMonitor is present, logs the TX packet (passing the ActionSet
 *   only for MSG_ID_EXECUTE_ACTIONS to enable rich decoding in the monitor UI).
 *
 * Returns:
 *   true  if the packet was sent successfully
 *   false if the port is closed or a write/timeout error occurred
 */
bool MainWindow::sendPacket(quint8 msgId, const QByteArray& payload)
{
    if (!serial.isOpen()) {
        ui->statusbar->showMessage("Serial not connected", 2500);
        return false;
    }

    QByteArray pkt = buildPacket(msgId, payload);

    if (!writeAllWithTimeout(pkt, /*perChunk*/200)) {
        ui->statusbar->showMessage("Serial write failed or timed out", 3000);
        return false;
    }
    serial.flush();  // Nudge the driver to push immediately

    // Optional: mirror TX in the monitor (with action metadata when relevant)
    if (monitor) {
        if (msgId == MSG_ID_EXECUTE_ACTIONS)
            monitor->logTx(pkt, &actionSet);
        else
            monitor->logTx(pkt, nullptr);
    }

    return true;
}

/*
 * Build the actions payload in the wire format expected by the device.
 *
 * - Gathers all Action nodes from the ActionSet.
 * - Sorts them by their numeric id to produce a stable, deterministic order.
 * - Converts the ordered list to the compact binary payload using
 *   encodeActionPayload().
 *
 * Returns:
 *   QByteArray with the payload (could be empty if there are no actions
 *   or the encoder returned nothing).
 */
QByteArray MainWindow::buildActionsPayload()
{
    if (actionSet.nodes().empty())
        return {};

    // Collect and sort action IDs to enforce a deterministic packing order
    std::vector<int> ids; ids.reserve(actionSet.nodes().size());
    for (const auto& up : actionSet.nodes()) ids.push_back(up->id);
    std::sort(ids.begin(), ids.end());

    // Map IDs back to pointers (skip missing/invalid ones)
    std::vector<Action*> ptrs; ptrs.reserve(ids.size());
    for (int id : ids)
        if (auto* a = actionSet.find(id)) ptrs.push_back(a);

    if (ptrs.empty())
        return {};

    // Encode to the device wire format
    std::vector<std::uint8_t> raw = encodeActionPayload(ptrs);
    if (raw.empty())
        return {};

    return QByteArray(reinterpret_cast<const char*>(raw.data()),
                      static_cast<int>(raw.size()));
}

/*
 * Build and send the actions payload immediately.
 *
 * - Validates that there are actions and they can be encoded.
 * - Sends a MSG_ID_EXECUTE_ACTIONS packet to the device.
 * - Shows status messages for UX feedback.
 */
void MainWindow::sendActions()
{
    if (actionSet.nodes().empty()) {
        ui->statusbar->showMessage("No actions to send", 2000);
        return;
    }

    // Same ordering pipeline as buildActionsPayload()
    std::vector<int> ids; ids.reserve(actionSet.nodes().size());
    for (const auto& up : actionSet.nodes()) ids.push_back(up->id);
    std::sort(ids.begin(), ids.end());

    std::vector<Action*> ptrs; ptrs.reserve(ids.size());
    for (int id : ids)
        if (auto* a = actionSet.find(id)) ptrs.push_back(a);

    if (ptrs.empty()) {
        ui->statusbar->showMessage("No valid actions found", 2000);
        return;
    }

    std::vector<std::uint8_t> raw = encodeActionPayload(ptrs);
    if (raw.empty()) {
        ui->statusbar->showMessage("Encoder produced empty payload", 2500);
        return;
    }

    QByteArray payload(reinterpret_cast<const char*>(raw.data()), int(raw.size()));
    sendPacket(MSG_ID_EXECUTE_ACTIONS, payload);
}

/*
 * Make a 512-byte chunk by cycling over `src` starting at index `pos`.
 *
 * - If `src` is shorter than 512 bytes, bytes repeat in a wrap-around fashion.
 * - If `src` is empty, returns a zero-filled buffer.
 * - Updates `pos` to the next start index for the following call.
 *
 * This is used to stream continuous SPI data chunks upon device requests.
 */
static inline QByteArray makeCycledChunk512(const QByteArray& src, int& pos)
{
    QByteArray out(512, char(0));
    if (src.isEmpty()) return out;

    const int n = src.size();
    char* dst = out.data();
    for (int i = 0; i < 512; ++i) {
        dst[i] = src.at(pos);
        pos = (pos + 1) % n;  // wrap around
    }
    return out;
}

/*
 * Handle a "CSPI_REQ" event coming from the SerialMonitor.
 *
 * - The device requested more SPI TX bytes.
 * - If CSPI streaming is active, generate a 512-byte chunk (cycled)
 *   and send it as MSG_ID_CSPI_DATA.
 */
void MainWindow::onCspiReqReceived()
{
    if (!m_cspiActive) return;
    const QByteArray chunk = makeCycledChunk512(m_cspiTxData, m_cspiPos);
    sendPacket(MSG_ID_CSPI_DATA, chunk);
}

/*
 * Begin a new Continuous SPI (CSPI) session.
 *
 * - `payload` is the CSPI header + TX data (as built by encodeCSPIPayload()).
 * - Device expects a separate BEGIN (header only) followed by DATA bursts.
 * - We split the QByteArray:
 *     * header: first 13 bytes (mode/wordSize/readSize/transfer_size/txLen/threshold/port/pin)
 *     * txData: remainder (the generator source used for makeCycledChunk512)
 * - Send MSG_ID_CSPI_BEGIN with just the header.
 * - Mark the CSPI session as active; data will then be fed in response to
 *   onCspiReqReceived() triggers.
 */
void MainWindow::onCspiPayloadReady(const QByteArray& payload)
{
    constexpr int kHeaderLen = 13;
    const QByteArray header = payload.left(kHeaderLen);
    const QByteArray txData = payload.mid(kHeaderLen);

    m_cspiTxData = txData;  // source buffer we will cycle over
    m_cspiPos    = 0;

    sendPacket(MSG_ID_CSPI_BEGIN, header);
    m_cspiActive = true;
}
