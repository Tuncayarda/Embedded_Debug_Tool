#include "../../mainwindow.h"
#include "../../actionwindow.h"
#include "../../actionset.h"

/*
 * MainWindow::on_addActionButton_clicked
 * --------------------------------------
 * Opens the "Add Action" dialog, collects the newly created action along with
 * its dependencies, translates dependency *indices in the current UI list*
 * into canonical *action IDs*, and inserts the new node into the ActionSet.
 *
 * Flow:
 *   1) Show AddActionWindow and bail if the user cancels.
 *   2) Take ownership of the freshly created Action (raw pointer -> unique_ptr).
 *   3) Map dialog-reported dependency indices (rows in the current list view)
 *      to the corresponding persistent action IDs held by ActionSet.
 *   4) Add the new node with its dependency edges into ActionSet.
 *   5) Refresh the list UI.
 */
void MainWindow::on_addActionButton_clicked()
{
    // 1) Launch modal dialog to gather action data from user
    AddActionWindow dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    // 2) Take ownership of the created action (nullptr if dialog produced none)
    std::unique_ptr<Action> node(dlg.createdAction());
    if (!node) return;

    // 3) Convert dependency *indices* (rows in the current list widget)
    //    into stable *IDs* from the ActionSet's backing store.
    const auto& nodes   = actionSet.nodes();
    const auto  depsIdx = dlg.dependencies();
    std::vector<int> depsId;
    depsId.reserve(depsIdx.size());

    for (int idx : depsIdx) {
        // Validate index against current nodes vector; ignore out-of-range
        if (idx >= 0 && idx < static_cast<int>(nodes.size())) {
            depsId.push_back(nodes[idx]->id); // map list row -> action ID
        }
    }

    // 4) Insert the new node into the graph with its parent links
    actionSet.add(std::move(node), depsId);

    // 5) Rebuild the UI list to reflect the updated model
    rebuildList();
}
