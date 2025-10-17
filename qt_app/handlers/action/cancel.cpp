#include "../../actionwindow.h"

/*
 * on_cancelButton_clicked
 * -----------------------
 * Slot connected to the Cancel button of the AddActionWindow dialog.
 *
 * Behavior:
 *   - Clears any staged Action object (m_action = nullptr).
 *   - Empties the dependency list (m_deps.clear()).
 *   - Calls reject() to close the dialog with a "Cancel" result.
 *
 * Usage:
 *   Invoked automatically when the Cancel button is clicked.
 *   Ensures that no partial or invalid Action data remains after cancel.
 */
void AddActionWindow::on_cancelButton_clicked()
{
    // Reset the action pointer so caller knows nothing was created
    m_action = nullptr;

    // Clear any dependency information collected so far
    m_deps.clear();

    // Close dialog with "Cancel" state
    reject();
}
