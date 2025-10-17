#include "../../actionwindow.h"
#include "../../ui_actionwindow.h"

void AddActionWindow::setKindPage(Kind k) {
    switch (k) {
    case Kind::DELAY:       ui->actionBox->setCurrentText("DELAY"); break;
    case Kind::PIN_READ:    ui->actionBox->setCurrentText("PIN_READ"); break;
    case Kind::PIN_WRITE:   ui->actionBox->setCurrentText("PIN_WRITE"); break;
    case Kind::PIN_TRIGGER: ui->actionBox->setCurrentText("PIN_TRIGGER"); break;
    default: ui->actionBox->setCurrentText("DELAY"); break;
    }
    ui->stackedWidget->setCurrentIndex(ui->actionBox->currentIndex());
}
