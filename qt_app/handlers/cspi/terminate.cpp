#include "../../cspiwindow.h"

void CSPIWindow::on_terminateButton_clicked()
{
    emit terminateRequested();
}
