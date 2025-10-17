#include "../../actionwindow.h"
#include "../../ui_actionwindow.h"
#include <QString>

/*
 * Convert (ms, us) into a compact "ms[.uuu]" string.
 * - If microseconds exceed 999, carry full milliseconds into ms.
 * - If us == 0, return just the millisecond part (e.g., "250").
 * - Otherwise, return "ms.us" with zero-padded 3-digit microseconds (e.g., "12.034").
 */
static inline QString formatTime(uint32_t ms, uint32_t us)
{
    if (us >= 1000) { ms += us / 1000; us %= 1000; }
    if (us == 0) return QString::number(ms);
    return QString("%1.%2").arg(ms).arg(us, 3, 10, QChar('0'));
}

/*
 * Populate the AddActionWindow UI from an existing Action instance.
 *
 * Behavior:
 *  - Switches the stacked UI to the page corresponding to a->kind.
 *  - Fills the dependency field from the provided parent IDs.
 *  - For each supported action kind, copies fields into the respective widgets:
 *      * DELAY:       duration -> lineEdit_duration_d  (formatted via formatTime)
 *      * PIN_READ:    port/pin/levels/duration -> read page widgets
 *      * PIN_WRITE:   port/pin/levels/duration -> write page widgets
 *      * PIN_TRIGGER: port/pin/levels/timeout  -> trigger page widgets
 */
void AddActionWindow::fillFromAction(const Action* a, const std::vector<int>& parents)
{
    if (!a) return;

    // Select the correct UI page and preload dependency list
    setKindPage(a->kind);
    depsSetCsv(idsToCsv(parents));

    // --- DELAY --------------------------------------------------------------
    if (a->kind == Kind::DELAY) {
        auto d = static_cast<const DelayAction*>(a);
        ui->lineEdit_duration_d->setText(formatTime(d->durationMs, d->durationUs));
        return;
    }

    // --- PIN_READ -----------------------------------------------------------
    if (a->kind == Kind::PIN_READ) {
        auto r = static_cast<const PinReadAction*>(a);
        ui->comboBox_port_r->setCurrentIndex(std::max(0, r->port));
        ui->spinBox_r->setValue(r->pin);
        levelSet(r->initial, ui->initialRHButton, ui->initialRLButton);
        levelSet(r->target,  ui->targetRHButton,  ui->targetRLButton);
        levelSet(r->final,   ui->finalRHButton,   ui->finalRLButton);
        ui->lineEdit_duration_r->setText(formatTime(r->durationMs, r->durationUs));
        return;
    }

    // --- PIN_WRITE ----------------------------------------------------------
    if (a->kind == Kind::PIN_WRITE) {
        auto w = static_cast<const PinWriteAction*>(a);
        ui->comboBox_port_w->setCurrentIndex(std::max(0, w->port));
        ui->spinBox_w->setValue(w->pin);
        levelSet(w->initial, ui->initialWHButton, ui->initialWLButton);
        levelSet(w->target,  ui->targetWHButton,  ui->targetWLButton);
        levelSet(w->final,   ui->finalWHButton,   ui->finalWLButton);
        ui->lineEdit_duration_w->setText(formatTime(w->durationMs, w->durationUs));
        return;
    }

    // --- PIN_TRIGGER --------------------------------------------------------
    if (a->kind == Kind::PIN_TRIGGER) {
        auto t = static_cast<const PinTriggerAction*>(a);
        ui->comboBox_port_t->setCurrentIndex(std::max(0, t->port));
        ui->spinBox_t->setValue(t->pin);
        levelSet(t->initial, ui->initialTHButton, ui->initialTLButton);
        levelSet(t->target,  ui->targetTHButton,  ui->targetTLButton);
        ui->lineEdit_timeout_t->setText(formatTime(t->timeoutMs, t->timeoutUs));
        return;
    }
}
