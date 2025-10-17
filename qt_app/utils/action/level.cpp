#include "../../actionwindow.h"

Level AddActionWindow::levelGet(QRadioButton* h, QRadioButton* l) {
    if (h->isChecked()) return Level::HIGH;
    if (l->isChecked()) return Level::LOW;
    return Level::UNDEFINED;
}
void AddActionWindow::levelSet(Level v, QRadioButton* h, QRadioButton* l) {
    h->setChecked(v==Level::HIGH);
    l->setChecked(v==Level::LOW);
}
