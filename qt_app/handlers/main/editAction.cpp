#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"
#include "../../actionwindow.h"

/*
 * MainWindow::on_editActionButton_clicked
 * ---------------------------------------
 * Edit the action corresponding to the currently selected row.
 *
 * Process:
 *  1) Validate selection and map the row to a model node.
 *  2) Fetch the target Action and its current parent dependencies.
 *  3) Launch the AddActionWindow in "edit" mode, pre-populated with
 *     the action's existing fields and dependencies.
 *  4) If the dialog is accepted, construct a replacement Action from
 *     the dialog, preserve the original ID, and apply edits to the model.
 *  5) Refresh the UI list and restore a reasonable selection; surface
 *     clear status messages for success/failure paths.
 */
void MainWindow::on_editActionButton_clicked()
{
    // 1) Selection validation
    const int row = ui->actionList->currentRow();
    if (row < 0 || row >= static_cast<int>(actionSet.nodes().size())) {
        ui->statusbar->showMessage("No action selected", 2000);
        return;
    }

    // 2) Resolve model object and existing dependencies
    Action* a = actionSet.nodes()[row].get();
    const int id = a->id;
    const auto parents = actionSet.parentsOf(id);

    // 3) Open editor dialog in edit mode with prefilled data
    AddActionWindow dlg(this);
    dlg.setEditMode(true);
    dlg.fillFromAction(a, parents);

    if (dlg.exec() != QDialog::Accepted)
        return; // User cancelled

    // 4) Build replacement node from dialog; preserve ID and apply edit
    auto repl = dlg.makeAction();
    if (!repl) {
        ui->statusbar->showMessage("Invalid data", 2000);
        return;
    }
    repl->id = id; // keep identity stable for graph references

    if (!actionSet.edit(id, std::move(repl), dlg.deps())) {
        ui->statusbar->showMessage("Edit failed", 2000);
        return;
    }

    // 5) Reflect changes in the UI and provide feedback
    rebuildList();
    ui->actionList->setCurrentRow(std::min(row, ui->actionList->count() - 1));
    ui->statusbar->showMessage(QString("Edited action ID:%1").arg(id), 2000);
}
