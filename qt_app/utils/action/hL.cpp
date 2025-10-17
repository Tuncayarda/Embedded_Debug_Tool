#include "../../actionwindow.h"

Level AddActionWindow::hL(bool highChecked) {
    return highChecked ? Level::HIGH : Level::LOW;
}
