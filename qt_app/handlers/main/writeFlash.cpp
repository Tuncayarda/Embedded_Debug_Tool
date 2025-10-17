#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"
#include "../../actionEncoder.h"

/*
 * MainWindow::on_wFlashButton_clicked
 * -----------------------------------
 * Build the current action set as a UART protocol payload and send it to the
 * device to be stored in on-chip flash.
 *
 * Workflow:
 *  1) Build the actions payload (raw action blob) using buildActionsPayload().
 *     - If empty, report and abort.
 *  2) Choose message ID based on the "Boot" checkbox:
 *        - MSG_ID_WRITE_FLASH_BOOT : store and mark as bootable (executed on boot)
 *        - MSG_ID_WRITE_FLASH      : store only (not auto-executed on boot)
 *  3) Frame and transmit with sendPacket(msgId, payload).
 *     - On failure, show error; otherwise, show a short success notice.
 */
void MainWindow::on_wFlashButton_clicked()
{
    // 1) Encode current UI action graph into a binary payload for the target
    QByteArray payload = buildActionsPayload();
    if (payload.isEmpty()) {
        ui->statusbar->showMessage("No actions to write into flash", 2500);
        return;
    }

    // 2) Select message ID depending on whether the user wants a bootable frame
    const quint8 msgId = ui->flashBootcheckBox->isChecked()
                             ? MSG_ID_WRITE_FLASH_BOOT
                             : MSG_ID_WRITE_FLASH;

    // 3) Send framed packet over the serial link; report status to the user
    if (!sendPacket(msgId, payload)) {
        ui->statusbar->showMessage("Flash write failed", 3000);
        return;
    }
    ui->statusbar->showMessage("Flash frame sent", 2000);
}
