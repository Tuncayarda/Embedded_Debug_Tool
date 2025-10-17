#include "../../actionwindow.h"
#include "../../ui_actionwindow.h"

/*
 * Construct a concrete Action from the current UI state.
 *
 * Behavior:
 *  - Inspects the currently selected "Kind" (action type) in the dialog.
 *  - Reads relevant widgets for that kind and populates a derived Action.
 *  - Parses time fields using parseTime(...) to split into ms/us.
 *  - Returns a std::unique_ptr<Action> to the newly created object,
 *    or nullptr if no supported kind is selected.
 *
 * Notes:
 *  - Levels (HIGH/LOW/UNDEFINED) are read via levelGet(...) for the active radio buttons.
 *  - Port and pin are taken from combo/spin boxes as integers.
 *  - For DELAY/PIN_* actions, duration/timeout is parsed from the corresponding line edit.
 *  - The caller is responsible for setting the Action's id and wiring dependencies.
 */
std::unique_ptr<Action> AddActionWindow::makeAction() const
{
    const Kind k = currentKind();

    // --- DELAY --------------------------------------------------------------
    if (k == Kind::DELAY) {
        auto d = std::make_unique<DelayAction>();
        uint32_t ms = 0, us = 0;
        // Parse "ms[.us]" style text into (ms, us)
        parseTime(ui->lineEdit_duration_d->text(), ms, us);
        d->durationMs = ms;
        d->durationUs = us;
        return d;
    }

    // --- PIN_READ -----------------------------------------------------------
    if (k == Kind::PIN_READ) {
        auto r = std::make_unique<PinReadAction>();
        r->port    = ui->comboBox_port_r->currentIndex();                       // 0..N (GPIO port index)
        r->pin     = ui->spinBox_r->value();                                    // 0..31
        r->initial = levelGet(ui->initialRHButton, ui->initialRLButton);        // initial pin level (triaged from radio buttons)
        r->target  = levelGet(ui->targetRHButton,  ui->targetRLButton);         // target level to observe
        r->final   = levelGet(ui->finalRHButton,   ui->finalRLButton);          // expected final level after duration

        uint32_t ms = 0, us = 0;
        parseTime(ui->lineEdit_duration_r->text(), ms, us);                     // read duration (ms/us)
        r->durationMs = ms;
        r->durationUs = us;
        return r;
    }

    // --- PIN_WRITE ----------------------------------------------------------
    if (k == Kind::PIN_WRITE) {
        auto w = std::make_unique<PinWriteAction>();
        w->port    = ui->comboBox_port_w->currentIndex();
        w->pin     = ui->spinBox_w->value();
        w->initial = levelGet(ui->initialWHButton, ui->initialWLButton);        // initial output level before write window
        w->target  = levelGet(ui->targetWHButton,  ui->targetWLButton);         // level to drive during the write window
        w->final   = levelGet(ui->finalWHButton,   ui->finalWLButton);          // level to leave the pin at after window

        uint32_t ms = 0, us = 0;
        parseTime(ui->lineEdit_duration_w->text(), ms, us);                     // write window duration
        w->durationMs = ms;
        w->durationUs = us;
        return w;
    }

    // --- PIN_TRIGGER --------------------------------------------------------
    if (k == Kind::PIN_TRIGGER) {
        auto t = std::make_unique<PinTriggerAction>();
        t->port    = ui->comboBox_port_t->currentIndex();
        t->pin     = ui->spinBox_t->value();
        t->initial = levelGet(ui->initialTHButton, ui->initialTLButton);        // initial expected level
        t->target  = levelGet(ui->targetTHButton,  ui->targetTLButton);         // level that must be reached before timeout

        uint32_t ms = 0, us = 0;
        parseTime(ui->lineEdit_timeout_t->text(), ms, us);                      // timeout for trigger condition
        t->timeoutMs = ms;
        t->timeoutUs = us;
        return t;
    }

    // Unsupported or "START/CSPI" kinds are not created here
    return nullptr;
}
