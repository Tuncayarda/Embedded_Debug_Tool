#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"

/*
 * MainWindow::on_connButton_clicked
 * ---------------------------------
 * Toggle serial connection:
 *  - When disconnected: open port using UI-selected name and standard UART
 *    parameters, update UI state, and attach the SerialMonitor.
 *  - When connected: detach monitor, close port, and reset UI state.
 *
 * Behavior details:
 *  - Port parameters:
 *      Baud      : 115200
 *      Data bits : 8
 *      Parity    : None
 *      Stop bits : 1
 *      Flow ctrl : None
 *  - Status bar shows short feedback messages for success/failure.
 *  - Guarded against empty port name and failed open() with error message.
 */
void MainWindow::on_connButton_clicked()
{
    if (!connected)
    {
        // Read target port from UI; abort if none selected.
        const QString portName = ui->comboBox_port->currentText();
        if (portName.isEmpty()) return;

        // Configure low-level serial parameters before opening.
        serial.setPortName(portName);
        serial.setBaudRate(QSerialPort::Baud115200);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        // Attempt to open the port for read/write; show reason if it fails.
        if (!serial.open(QIODevice::ReadWrite)) {
            statusBar()->showMessage(
                QString("Open failed: %1").arg(serial.errorString()), 3000);
            return;
        }

        // Mark as connected and refresh UI/monitor state.
        connected = true;
        ui->connButton->setText("Disconnect");
        statusBar()->showMessage(QString("Connected: %1").arg(portName), 2000);

        // If the serial monitor window is alive, hook it to the active port.
        if (monitor) monitor->attachPort(&serial);
    }
    else
    {
        // Detach monitor first to stop it from reading a closing device.
        if (monitor) monitor->detachPort();

        // Close port if still open; ignore errors here (best-effort shutdown).
        if (serial.isOpen()) serial.close();

        // Mark as disconnected and refresh UI/monitor state.
        connected = false;
        ui->connButton->setText("Connect");
        statusBar()->showMessage("Disconnected", 2000);
    }
}
