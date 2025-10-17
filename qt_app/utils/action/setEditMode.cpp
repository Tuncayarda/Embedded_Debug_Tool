#include "../../actionwindow.h"
#include "../../ui_actionwindow.h"

void AddActionWindow::setEditMode(bool on) {
    setWindowTitle(on ? tr("Edit Action") : tr("Add Action"));
    ui->addButton->setText(on ? tr("Save") : tr("Add"));
}
