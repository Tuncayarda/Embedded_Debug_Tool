#include "../../cspiwindow.h"

void CSPIWindow::on_stopButton_clicked()
{
    emit stopRequested();
}
