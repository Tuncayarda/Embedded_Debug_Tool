#include "../../cspiwindow.h"
#include "../../ui_cspiwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

/*
 * on_importButton_clicked
 * -----------------------
 * Slot for handling the "Import" button click in the CSPIWindow UI.
 *
 * Behavior:
 *   1. Opens a file dialog allowing the user to select a *.txt file (or any file).
 *   2. If the user cancels, the function simply returns without action.
 *   3. Attempts to open the selected file in read-only text mode.
 *      - If opening fails, shows a QMessageBox warning with the error string.
 *   4. If successful, reads the entire file into a QString.
 *   5. Populates the UI's tx_data text editor with the imported content.
 */
void CSPIWindow::on_importButton_clicked()
{
    // Step 1: Prompt user to select a file
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Import Data from TXT"),
        "",
        tr("Text Files (*.txt);;All Files (*)")
        );

    // Step 2: If no file was chosen, abort
    if (fileName.isEmpty())
        return;

    // Step 3: Open the chosen file in read-only text mode
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Error handling: show message box with cause
        QMessageBox::warning(
            this,
            tr("Error"),
            tr("Cannot open file: %1").arg(file.errorString())
            );
        return;
    }

    // Step 4: Read entire content of file
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Step 5: Set text content into the designated UI text editor
    ui->textEdit_tx_data->setPlainText(content);
}
