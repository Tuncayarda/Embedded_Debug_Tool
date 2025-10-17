/*
 * execute.c
 *  Action yürütme motoru (PIT tick’leri ile zamanlama, GPIO kontrolü, bağımlılık akışı).
 */

#include "execute.h"
#include "pit/pit.h"
#include "fsl_clock.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "gpio/gpio_utils.h"
#include "uart/uart.h"

extern volatile uint32_t g_tick;  /* Global PIT tick sayacı */

/* Bir action için “şu andan itibaren” bitiş tick’ini ayarla */
static inline void arm_deadline(uint32_t *deadline, uint32_t now, uint32_t dur_ticks)
{
    *deadline = now + dur_ticks;
}

/* Bitiş tick’i geçildi mi? (signed fark ile) */
static inline int tick_reached(uint32_t deadline)
{
    return (int)((int32_t)(g_tick - deadline) >= 0);
}

/* Tamamlanan action’ın hedeflerini (targets) PENDING yap */
void prepare_targets(t_action_set *S, const t_action_rec *a)
{
    for (uint8_t k = 0; k < a->target_count; k++)
    {
        uint8_t tid = a->targets[k];
        if (tid >= MAX_ACTIONS) continue;
        t_action_rec *ch = &S->actions[tid];
        if (ch->status == STATUS_IDLE) {
            ch->status = STATUS_PENDING;
        }
    }
}

/* Bir action’ı başlat (PENDING→RUNNING) ve zamanlayıcıları kur
 * Dönüş: >0 anında bitti, 0 çalışıyor, <0 hata
 */
int start_action(t_action_rec *a)
{
    if (a->status != STATUS_PENDING) return 0;
    uint32_t now = g_tick;
    a->status     = STATUS_RUNNING;
    a->start_tick = now;

    switch (a->type)
    {
    case TYPE_START:
        return 1; /* Anında biter */

    case TYPE_DELAY:
        arm_deadline(&a->deadline_tick, now, a->u.delay.duration_ticks);
        return 0;

    case TYPE_PIN_WRITE: {
        /* Hedef seviyeyi hemen sür, süre dolunca finalize et */
        t_pin_write_fields *f = &a->u.pin_write;
        if (f->target == LVL_HIGH) gpio_write_high(f->port, f->pin);
        else if (f->target == LVL_LOW) gpio_write_low(f->port, f->pin);
        arm_deadline(&a->deadline_tick, now, f->duration_ticks);
        return 0;
    }

    case TYPE_PIN_TRIGGER: {
        /* Zaman aşımına kadar hedef seviyeyi bekle */
        t_pin_trigger_fields *f = &a->u.pin_trigger;
        arm_deadline(&a->deadline_tick, now, f->duration_ticks);
        return 0;
    }

    default:
        a->status = STATUS_ERROR;
        a->error  = ERROR_NON_SPECIFIED;
        return -1;
    }
}

/* RUNNING durumundaki action’ı ilerlet
 * Dönüş: >0 bitti, 0 devam, <0 hata
 */
int run_action(t_action_rec *a)
{
    if (a->status != STATUS_RUNNING) return 0;

    switch (a->type)
    {
    case TYPE_START:
        return 1;

    case TYPE_DELAY:
        return tick_reached(a->deadline_tick);

    case TYPE_PIN_WRITE: {
        /* Süre dolunca final seviyeyi uygula ve bitir */
        t_pin_write_fields *f = &a->u.pin_write;
        if (tick_reached(a->deadline_tick)) {
            if (f->final == LVL_HIGH) gpio_write_high(f->port, f->pin);
            else if (f->final == LVL_LOW) gpio_write_low(f->port, f->pin);
            return 1;
        }
        return 0;
    }

    case TYPE_PIN_TRIGGER: {
        /* Hedef seviye gelirse başarı; yoksa timeout’ta hata */
        t_pin_trigger_fields *f = &a->u.pin_trigger;

        if (gpio_read(f->port, f->pin) == f->target) {
            return 1;
        }

        if (tick_reached(a->deadline_tick)) {
            a->status = STATUS_ERROR;
            a->error  = ERROR_NON_SPECIFIED;
            return -1;
        }
        return 0;
    }

    default:
        a->status = STATUS_ERROR;
        a->error  = ERROR_NON_SPECIFIED;
        return -1;
    }
}

/* Action’ı DONE yap ve hedeflerini PENDING’e geçir */
static inline void finish_action(t_action_set *S, t_action_rec *a)
{
    a->status = STATUS_DONE;
    for (uint8_t k = 0; k < a->target_count; ++k) {
        uint8_t tid = a->targets[k];
        if (tid >= MAX_ACTIONS) continue;
        t_action_rec *ch = &S->actions[tid];
        if (ch->status == STATUS_IDLE)
            ch->status = STATUS_PENDING;
    }
}

/* Tüm action set’i sırayla çalıştır:
 * - init_pins(set) ile pinleri hazırla
 * - START tiplerini giriş noktası olarak PENDING yap
 * - Döngüde PENDING→start, RUNNING→run, bitenleri finish et
 * - Hepsi DONE olunca PIT’i durdur
 */
int execute(t_action_set *set)
{
    init_pins(set);

    /* Başlangıç durumlarını sıfırla ve ID index tutarlılığını kontrol et */
    for (int i = 0; i < set->count; ++i) {
        if (set->actions[i].id != (uint8_t)i) return -1;
        set->actions[i].status         = STATUS_IDLE;
        set->actions[i].error          = ERROR_NONE;
        set->actions[i].start_tick     = 0;
        set->actions[i].deadline_tick  = 0;
    }

    /* Giriş noktaları: TYPE_START → PENDING */
    for (int i = 0; i < set->count; ++i) {
        if (set->actions[i].type == TYPE_START)
            set->actions[i].status = STATUS_PENDING;
    }

    uint16_t total_done = 0;
    pit_start();
    while (total_done < set->count) {

        for (uint16_t i = 0; i < set->count; ++i) {
            t_action_rec *a = &set->actions[i];

            if (a->status == STATUS_PENDING) {
                if (start_action(a) > 0) {
                    finish_action(set, a);
                    total_done++;
                }
            }
            if (a->status == STATUS_RUNNING) {
                if (run_action(a) > 0) {
                    finish_action(set, a);
                    total_done++;
                }
            }

            if (a->status == STATUS_ERROR)
            {
            	pit_stop();
            	return -55;
            }
        }
    }
    pit_stop();
    return 0;
}
