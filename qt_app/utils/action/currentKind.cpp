#include "../../actionwindow.h"
#include "../../ui_actionwindow.h"

Kind AddActionWindow::currentKind() const {
    const QString k = ui->actionBox->currentText();
    if (k=="DELAY")        return Kind::DELAY;
    if (k=="PIN_READ")     return Kind::PIN_READ;
    if (k=="PIN_WRITE")    return Kind::PIN_WRITE;
    if (k=="PIN_TRIGGER")  return Kind::PIN_TRIGGER;
    return Kind::DELAY;
}
