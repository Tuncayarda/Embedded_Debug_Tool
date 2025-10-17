#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"

/*
 * MainWindow::on_delActionButton_clicked
 * --------------------------------------
 * Delete the currently selected action from the list and underlying model.
 *
 * Flow:
 *  1) Validate there is a selection and that it maps to a valid node index.
 *  2) Resolve the action ID from the selected row (row -> nodes()[row]->id).
 *  3) Disallow deletion of the START node (ID 0) to keep graph well-formed.
 *  4) Ask ActionSet to remove the node; on success, rebuild the UI list and
 *     show a transient status message.
 *  5) Provide clear user feedback on all failure paths.
 */
void MainWindow::on_delActionButton_clicked()
{
    // 1) Must have a selected row in the list widget.
    int row = ui->actionList->currentRow();
    if (row < 0) {
        ui->statusbar->showMessage("No action selected", 2000);
        return;
    }

    // 2) Row must be within bounds of the backing model.
    if (row >= static_cast<int>(actionSet.nodes().size())) {
        ui->statusbar->showMessage("Invalid row selected", 2000);
        return;
    }

    // Map the UI row to the model's action ID.
    const int id = actionSet.nodes()[row]->id;

    // 3) Guard: do not allow deleting the START node (root).
    if (id == 0) {
        ui->statusbar->showMessage("Start action cannot be deleted", 2000);
        return;
    }

    // 4) Attempt removal in the model; ActionSet handles dependency relinking/cleanup.
    if (!actionSet.remove(id)) {
        ui->statusbar->showMessage("Failed to remove action", 2000);
        return;
    }

    // Refresh the list view to reflect the updated model state.
    rebuildList();

    // 5) Positive feedback with context (ID and original row).
    ui->statusbar->showMessage(
        QString("Deleted action ID:%1 (row %2)").arg(id).arg(row),
        2000
        );
}
