#include "../../mainwindow.h"
#include "../../ui_mainwindow.h"

/*
 * Rebuild the action list displayed in the UI.
 * --------------------------------------------
 * - Clears the current list widget (ui->actionList).
 * - Iterates through all actions stored in `actionSet`.
 * - For each action, generates a human-readable description line
 *   using `actionInfoLine()` and adds it as a new list item.
 * - This ensures the GUI stays in sync with the underlying model
 *   whenever actions are added, edited, or deleted.
 */
void MainWindow::rebuildList() {
    if (!ui->actionList) return;      // Guard: ensure the widget exists
    ui->actionList->clear();          // Remove old list entries

    // Repopulate with updated action descriptions
    for (auto& up : actionSet.nodes()) {
        const Action* a = up.get();   // Access raw pointer from unique_ptr
        ui->actionList->addItem(actionInfoLine(a));
    }
}
