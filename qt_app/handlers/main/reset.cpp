#include "../../mainwindow.h"
#include "../../actionEncoder.h"

void MainWindow::on_resetButton_clicked()
{
    sendPacket(MSG_ID_RESET, nullptr);
}
