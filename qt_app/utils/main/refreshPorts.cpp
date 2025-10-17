#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"

/*
 * Refresh the list of available serial ports shown in the UI.
 * -----------------------------------------------------------
 * - Reads currently listed items from the combo box.
 * - Enumerates system serial ports via QSerialPortInfo.
 * - If the set of names has changed, repopulates the combo while preserving
 *   the previously selected item when possible.
 * - Enables/disables the Connect button based on port availability.
 */
void MainWindow::refreshPorts()
{
    // Snapshot the current set of items in the combo box to detect changes.
    QSet<QString> current;
    for (int i = 0; i < ui->comboBox_port->count(); ++i)
        current.insert(ui->comboBox_port->itemText(i));

    // Enumerate available ports and build a fresh name list + set.
    const auto ports = QSerialPortInfo::availablePorts();
    QSet<QString>   found;
    QStringList     names;
    names.reserve(ports.size());
    for (const auto& p : ports) {
        const QString name = p.portName();
        names << name;
        found.insert(name);
    }

    // If nothing changed (same set of names), skip UI updates.
    if (found == current) return;

    // Try to preserve the previously selected port (if still present).
    const QString prev = ui->comboBox_port->currentText();

    // Replace the combo box contents without emitting spurious signals.
    ui->comboBox_port->blockSignals(true);
    ui->comboBox_port->clear();
    ui->comboBox_port->addItems(names);
    ui->comboBox_port->blockSignals(false);

    // Restore prior selection when possible.
    int idx = ui->comboBox_port->findText(prev);
    if (idx >= 0) ui->comboBox_port->setCurrentIndex(idx);

    // Enable the Connect button only if at least one port is available.
    ui->connButton->setEnabled(ui->comboBox_port->count() > 0);
}
