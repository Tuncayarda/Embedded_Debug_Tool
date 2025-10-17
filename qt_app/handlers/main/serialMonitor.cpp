#include "../../mainwindow.h"
#include "../../serialmonitor.h"
#include <QScreen>
#include <QGuiApplication>

/*
 * MainWindow::on_serialMonitorButton_clicked
 * ------------------------------------------
 * Create (if needed), wire up, and show the Serial Monitor window.
 *
 * Behavior:
 *  - Lazily constructs the SerialMonitor on first invocation.
 *  - If a serial connection is already open, attaches the port so the monitor
 *    can immediately display incoming/outgoing traffic.
 *  - Connects the monitor’s CSPI request signal to the corresponding slot in
 *    MainWindow (single connection policy).
 *  - Positions the monitor near the right edge of the primary screen and
 *    vertically centered; brings it to the front and focuses it.
 */
void MainWindow::on_serialMonitorButton_clicked()
{
    // Lazy creation: set up the monitor only once.
    if (!monitor) {
        monitor = new SerialMonitor(this);
        monitor->setWindowFlag(Qt::Window, true);
        monitor->setWindowTitle("Serial Monitor");
        monitor->setAttribute(Qt::WA_DeleteOnClose);

        // If we are already connected, let the monitor tap into the live port.
        if (connected)
            monitor->attachPort(&serial);

        // Bridge CSPI 'request for data' notifications from the monitor to MainWindow.
        connect(monitor, &SerialMonitor::cspiReqReceived,
                this,    &MainWindow::onCspiReqReceived,
                Qt::UniqueConnection);
    }

    // Determine available desktop geometry on the primary screen.
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    const int screenWidth  = screenGeometry.width();
    const int screenHeight = screenGeometry.height();

    // Use the monitor's current size for positioning calculations.
    const int winWidth  = monitor->width();
    const int winHeight = monitor->height();

    // Place the window 20 px from the right edge, vertically centered.
    const int posX = screenWidth  - winWidth  - 20;
    const int posY = (screenHeight - winHeight) / 2;

    monitor->move(posX, posY);

    // Show and raise to foreground, giving it input focus.
    monitor->show();
    monitor->raise();
    monitor->activateWindow();
}
