#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QPointer>
#include "serialmonitor.h"
#include "cspiwindow.h"
#include "actionset.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*------------------------------------------------------------------------------
 * MainWindow
 *------------------------------------------------------------------------------
 * Purpose
 *   - Top-level application window that orchestrates:
 *       * Serial connection lifecycle (open/close, write/read).
 *       * Action graph management (add/edit/remove, listing).
 *       * Packet encoding/dispatch for runtime execution and flash ops.
 *       * CSPI workflow (hosting CSPIWindow and streaming chunks on request).
 *       * Optional live SerialMonitor tapping TX/RX frames for debugging.
 *
 * Responsibilities
 *   - Manage UI controls and reflect device/port state.
 *   - Periodically refresh available serial ports.
 *   - Frame and send protocol packets; handle simple responses.
 *   - Bridge CSPI “REQ/DATA/END/TERMINATE” with the device.
 *----------------------------------------------------------------------------*/
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief Construct the main application window and initialize UI/timers.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroy the window and release owned resources.
     */
    ~MainWindow();

private slots:
    /*--------------------------- Actions UI ---------------------------------*/
    /**
     * @brief Open “Add Action” dialog and append a new node to the action set.
     */
    void on_addActionButton_clicked();
    /**
     * @brief Remove the currently selected action (optionally re-linking).
     */
    void on_delActionButton_clicked();
    /**
     * @brief Edit the currently selected action via the dialog.
     */
    void on_editActionButton_clicked();
    /**
     * @brief Update right-hand inspector/controls when selection changes.
     */
    void on_actionList_currentRowChanged(int currentRow);

    /*--------------------------- Serial / Ports -----------------------------*/
    /**
     * @brief Periodically rescan available serial ports and update the combo.
     */
    void refreshPorts();
    /**
     * @brief Connect/disconnect toggle handler; opens/closes QSerialPort.
     */
    void on_connButton_clicked();
    /**
     * @brief Open or focus the SerialMonitor window (optional live trace).
     */
    void on_serialMonitorButton_clicked();

    /*--------------------------- Execute / Flash ----------------------------*/
    /**
     * @brief Build and send current action set for immediate execution.
     */
    void on_executeButton_clicked();
    /**
     * @brief Write a framed payload into device user flash (boot/non-boot).
     */
    void on_wFlashButton_clicked();
    /**
     * @brief Clear (erase) device user flash region.
     */
    void on_cFlashButton_clicked();
    /**
     * @brief Send a device reset command.
     */
    void on_resetButton_clicked();

    /*--------------------------- CSPI Workflow ------------------------------*/
    /**
     * @brief Show CSPIWindow; connect its signals to start a CSPI session.
     */
    void on_cSPIButton_clicked();
    /**
     * @brief Receive the ready-to-send CSPI payload from CSPIWindow (header+TX)
     *        and forward it to the device (e.g., as MSG_ID_CSPI_BEGIN).
     */
    void onCspiPayloadReady(const QByteArray& payload);
    /**
     * @brief Handle device “REQ” (data request) by pushing the next 512-byte
     *        chunk from m_cspiTxData (cyclic if shorter than 512).
     */
    void onCspiReqReceived();
    /**
     * @brief Graceful CSPI stop requested by the UI; forward to device.
     */
    void onSPIStopRequested();
    /**
     * @brief Forced CSPI terminate requested by the UI; forward to device.
     */
    void onSPITerminateRequested();

private:
    /*--------------------------- UI / Model --------------------------------*/
    Ui::MainWindow *ui{nullptr};    ///< Generated form (owned).

    ActionSet   actionSet;          ///< In-memory action graph model.
    void        rebuildList();      ///< Rebuild list widget from actionSet.
    QString     actionInfoLine(const Action* a) const; ///< One-line summary.

    /*--------------------------- Port / Serial ------------------------------*/
    QTimer      *portTimer{nullptr};///< Periodic port refresh timer (owned).
    QSerialPort serial;             ///< Synchronous serial transport.
    bool        connected{false};   ///< Cached connection state for UI.

    /*--------------------------- Aux Windows --------------------------------*/
    QPointer<SerialMonitor> monitor;///< Optional live monitor (not owned).
    QPointer<CSPIWindow>    cspiWin;///< Optional CSPI config window (not owned).

    /*--------------------------- I/O helpers --------------------------------*/
    /**
     * @brief Write entire buffer with per-chunk timeout; returns success/fail.
     *        Uses QSerialPort::write + waitForBytesWritten to drain OS buffers.
     */
    bool writeAllWithTimeout(const QByteArray& data, int perChunkTimeoutMs = 200);

    /**
     * @brief Read one response frame with initial and tail timeouts.
     *        Useful for small ACK/status replies (optional).
     */
    QByteArray readOneResponse(int firstByteTimeoutMs = 300, int tailTimeoutMs = 50);

    /**
     * @brief Format a byte array as spaced uppercase hex (e.g., "AA 55 10 ...").
     */
    static QString hexSpaced(const QByteArray& b);

    /**
     * @brief Frame and send a protocol packet (SOF + ID + LEN + PAYLOAD + CRC16).
     */
    bool sendPacket(quint8 msgId, const QByteArray& payload);

    /**
     * @brief Encode current ActionSet into a binary payload suitable for device.
     */
    QByteArray buildActionsPayload();

    /**
     * @brief Convenience: build, frame and send the execute-actions packet.
     */
    void sendActions();

    /*--------------------------- CSPI streaming state -----------------------*/
    QByteArray m_cspiTxData;  ///< Full TX pattern/buffer to stream to device.
    int        m_cspiPos{0};  ///< Current read pointer within m_cspiTxData.
    bool       m_cspiActive{false}; ///< Whether a CSPI session is active.
};

#endif // MAINWINDOW_H
