#include "../../actionwindow.h"
#include "../../ui_actionwindow.h"
#include <QString>
#include <QStringList>

/*
 * on_addButton_clicked
 * --------------------
 * Collects user input from the dialog, constructs the corresponding Action
 * (Delay, PinRead, PinWrite, PinTrigger), and records its dependencies.
 *
 * Behavior
 *   - Clears any previous staged action/dependency state.
 *   - Branches by current action type (combo index).
 *   - Parses time fields into (ms/us) using parseTime().
 *   - Converts UI radio-button states into Level via hL().
 *   - Parses dependency CSV string into vector<int> with parseIds().
 *   - If a valid Action is created, calls accept() to close the dialog with OK,
 *     otherwise reject().
 *
 * UI index mapping (must match .ui):
 *   0 → DELAY
 *   1 → PIN_READ
 *   2 → PIN_WRITE
 *   3 → PIN_TRIGGER
 */
void AddActionWindow::on_addButton_clicked()
{
    // Reset any previous result from a prior click
    m_action = nullptr;
    m_deps.clear();

    switch (ui->actionBox->currentIndex()) {
    case 0: { // DELAY
        auto *d = new DelayAction();

        // Parse duration text (e.g. "123(ms),45(us)") into numeric fields.
        uint32_t ms = 0, us = 0;
        parseTime(ui->lineEdit_duration_d->text(), ms, us);
        d->durationMs = ms;
        d->durationUs = us;

        // Stash result and dependencies (comma/space separated IDs).
        m_action = d;
        m_deps   = parseIds(ui->lineEdit_dependency_d->text());
        break;
    }

    case 1: { // PIN_READ
        auto *r = new PinReadAction();

        // Port/pin selection (indexes are assumed to match firmware mapping).
        r->port    = ui->comboBox_port_r->currentIndex();
        r->pin     = ui->spinBox_r->value();

        // Initial/target/final logic levels from radio buttons.
        r->initial = hL(ui->initialRHButton->isChecked());
        r->target  = hL(ui->targetRHButton->isChecked());
        r->final   = hL(ui->finalRHButton->isChecked());

        // Duration to keep measuring/holding the read condition.
        uint32_t ms = 0, us = 0;
        parseTime(ui->lineEdit_duration_r->text(), ms, us);
        r->durationMs = ms;
        r->durationUs = us;

        m_action = r;
        m_deps   = parseIds(ui->lineEdit_dependency_r->text());
        break;
    }

    case 2: { // PIN_WRITE
        auto *w = new PinWriteAction();

        // Port/pin selection
        w->port = ui->comboBox_port_w->currentIndex();
        w->pin  = ui->spinBox_w->value();

        // Levels: initial (set before timing), target (set at start),
        // final (apply after duration elapses).
        w->initial = hL(ui->initialWHButton->isChecked());
        w->target  = hL(ui->targetWHButton->isChecked());
        w->final   = hL(ui->finalWHButton->isChecked());

        // Pulse/hold duration
        uint32_t ms = 0, us = 0;
        parseTime(ui->lineEdit_duration_w->text(), ms, us);
        w->durationMs = ms;
        w->durationUs = us;

        m_action = w;
        m_deps   = parseIds(ui->lineEdit_dependency_w->text());
        break;
    }

    case 3: { // PIN_TRIGGER
        auto *t = new PinTriggerAction();

        // Port/pin selection
        t->port = ui->comboBox_port_t->currentIndex();
        t->pin  = ui->spinBox_t->value();

        // Trigger condition: wait until pin reaches 'target', starting from 'initial'
        t->initial = hL(ui->initialTHButton->isChecked());
        t->target  = hL(ui->targetTHButton->isChecked());

        // Timeout for the trigger window
        uint32_t ms = 0, us = 0;
        parseTime(ui->lineEdit_timeout_t->text(), ms, us);
        t->timeoutMs = ms;
        t->timeoutUs = us;

        m_action = t;
        m_deps   = parseIds(ui->lineEdit_dependency_t->text());
        break;
    }

    default:
        // Unknown action type; bail out
        m_action = nullptr;
        break;
    }

    // Close the dialog with the appropriate result.
    if (m_action) accept();
    else          reject();
}
