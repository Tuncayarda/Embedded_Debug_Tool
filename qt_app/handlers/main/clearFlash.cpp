#include "../../mainwindow.h"
#include "../../actionEncoder.h"

void MainWindow::on_cFlashButton_clicked()
{
    sendPacket(MSG_ID_CLEAR_FLASH, nullptr);
}
