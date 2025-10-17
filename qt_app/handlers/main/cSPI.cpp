#include "../../mainwindow.h"
#include "../../cspiwindow.h"

/*
 * MainWindow::on_cSPIButton_clicked
 * ---------------------------------
 * Open (or bring to front) the Continuous SPI control window.
 *
 * Behavior:
 *  - Lazily creates a CSPIWindow on first use, sets it up as a top-level tool
 *    window with automatic deletion on close.
 *  - Wires CSPIWindow signals to MainWindow slots exactly once
 *    (Qt::UniqueConnection) to avoid duplicate handlers if the window is
 *    reopened.
 *  - On subsequent invocations, reuses the existing instance and simply
 *    focuses it.
 *
 * Signal wiring:
 *  - CSPIWindow::payloadReady(QByteArray)  -> MainWindow::onCspiPayloadReady
 *      Emitted when user prepares a valid CSPI header/payload; the main window
 *      forwards it over the serial link.
 *  - CSPIWindow::stopRequested()           -> MainWindow::onSPIStopRequested
 *      Requests a graceful stop of the ongoing bulk SPI transfer.
 *  - CSPIWindow::terminateRequested()      -> MainWindow::onSPITerminateRequested
 *      Requests an immediate terminate of SPI activity.
 */
void MainWindow::on_cSPIButton_clicked()
{
    if (!cspiWin) {
        // Create the tool window once and configure its windowing behavior.
        cspiWin = new CSPIWindow(this);
        cspiWin->setWindowFlag(Qt::Window, true);          // make it a top-level window
        cspiWin->setWindowTitle("Continuous SPI");
        cspiWin->setAttribute(Qt::WA_DeleteOnClose);       // auto-delete on close

        // Keep our pointer in sync with the widget’s lifetime.
        connect(cspiWin, &QWidget::destroyed, this, [this]() { cspiWin = nullptr; });

        // Forward CSPI control/data signals to the main window logic.
        connect(cspiWin, &CSPIWindow::payloadReady,
                this,    &MainWindow::onCspiPayloadReady,
                Qt::UniqueConnection);

        connect(cspiWin, &CSPIWindow::stopRequested,
                this,    &MainWindow::onSPIStopRequested,
                Qt::UniqueConnection);

        connect(cspiWin, &CSPIWindow::terminateRequested,
                this,    &MainWindow::onSPITerminateRequested,
                Qt::UniqueConnection);
    }

    // Show and focus the tool window (works for both new and existing instances).
    cspiWin->show();
    cspiWin->raise();
    cspiWin->activateWindow();
}
