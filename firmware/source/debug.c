/*
 * debug.c
 *
 *  Created on: Sep 11, 2025
 *      Author: tuncayardaaydin
 */

#include "debug.h"
#include "uart/uart.h"

static inline char lvl_ch(uint8_t l){
    if (l == LVL_LOW) return 'L';
    if (l == LVL_HIGH) return 'H';
    return '?';
}

static void print_targets(const t_action_rec *a){
    uart0_print("  targets=[");
    for (uint8_t i=0;i<a->target_count;i++){
        uart0_print_i32(a->targets[i]);
        if (i+1 < a->target_count) uart0_print(",");
    }
    uart0_print("]");
}

static void print_ms_us(const char* label, uint32_t ms, uint16_t us)
{
    uart0_print("  ");
    uart0_print(label);
    uart0_print("=");

    uart0_print_i32((int32_t)ms);
    uart0_putc('.');
    if (us < 10)       { uart0_putc('0'); uart0_putc('0'); }
    else if (us < 100) { uart0_putc('0'); }
    uart0_print_i32((int32_t)us);

    uart0_print("ms");
}

static void print_ticks_if_any(const char* label, uint32_t ticks)
{
    if (ticks != 0u) {
        uart0_print("  ");
        uart0_print(label);
        uart0_print("=");
        uart0_print_i32((int32_t)ticks);
    }
}

void dump_action_set(const t_action_set *S)
{
    if (!S) return;
    uart0_print("----- ACTION SET -----\r\n");
    uart0_print("count="); uart0_print_i32(S->count); uart0_print("\r\n");

    for (int id=0; id<S->count; ++id){
        const t_action_rec *a = &S->actions[id];
        uart0_print("ID="); uart0_print_i32(id); uart0_print("  ");

        switch (a->type) {
        case TYPE_START:
            uart0_print("START");
            print_targets(a);
            uart0_print("\r\n");
            break;

        case TYPE_DELAY:
            uart0_print("DELAY");
            print_ms_us("duration", a->u.delay.duration_ms, a->u.delay.duration_us);
            print_ticks_if_any("ticks", a->u.delay.duration_ticks);
            print_targets(a);
            uart0_print("\r\n");
            break;

        case TYPE_PIN_READ:
            uart0_print("PIN_READ  P=");
            uart0_print_i32(a->u.pin_read.port);
            uart0_print("  pin=");
            uart0_print_i32(a->u.pin_read.pin);
            uart0_print("  init=");   uart0_putc(lvl_ch(a->u.pin_read.initial));
            uart0_print("  target="); uart0_putc(lvl_ch(a->u.pin_read.target));
            uart0_print("  final=");  uart0_putc(lvl_ch(a->u.pin_read.final));
            print_ms_us("duration", a->u.pin_read.duration_ms, a->u.pin_read.duration_us);
            print_ticks_if_any("ticks", a->u.pin_read.duration_ticks);
            print_targets(a);
            uart0_print("\r\n");
            break;

        case TYPE_PIN_WRITE:
            uart0_print("PIN_WRITE P=");
            uart0_print_i32(a->u.pin_write.port);
            uart0_print("  pin=");
            uart0_print_i32(a->u.pin_write.pin);
            uart0_print("  init=");   uart0_putc(lvl_ch(a->u.pin_write.initial));
            uart0_print("  target="); uart0_putc(lvl_ch(a->u.pin_write.target));
            uart0_print("  final=");  uart0_putc(lvl_ch(a->u.pin_write.final));
            print_ms_us("duration", a->u.pin_write.duration_ms, a->u.pin_write.duration_us);
            print_ticks_if_any("ticks", a->u.pin_write.duration_ticks);
            print_targets(a);
            uart0_print("\r\n");
            break;

        case TYPE_PIN_TRIGGER:
            uart0_print("PIN_TRIGGER P=");
            uart0_print_i32(a->u.pin_trigger.port);
            uart0_print("  pin=");
            uart0_print_i32(a->u.pin_trigger.pin);
            uart0_print("  init=");   uart0_putc(lvl_ch(a->u.pin_trigger.initial));
            uart0_print("  target="); uart0_putc(lvl_ch(a->u.pin_trigger.target));
            print_ms_us("timeout", a->u.pin_trigger.timeout_ms, a->u.pin_trigger.timeout_us);
            print_ticks_if_any("ticks", a->u.pin_trigger.duration_ticks);
            print_targets(a);
            uart0_print("\r\n");
            break;

        default:
            uart0_print("UNKNOWN\r\n");
            break;
        }
    }
    uart0_print("----------------------\r\n");
}

void dump_cspi(const t_cspi_fields *C)
{
    if (!C) {
        uart0_print("CSPI: (null)\r\n");
        return;
    }

    uart0_print("----- CSPI CONFIG -----\r\n");
    uart0_print("  mode=");           uart0_print_i32(C->mode);         uart0_print("\r\n");
    uart0_print("  word_size=");      uart0_print_i32(C->word_size);    uart0_print(" bits\r\n");
    uart0_print("  transfer_size=");  uart0_print_i32(C->transfer_size);uart0_print("\r\n");
    uart0_print("  threshold=");      uart0_print_i32(C->threshold_val);uart0_print("\r\n");

    uart0_print("  port=");           uart0_print_i32(C->port);
    uart0_print("  pin=");            uart0_print_i32(C->pin);          uart0_print("\r\n");

    uart0_print("  idle_fill=0x");    uart0_puthex(C->idle_fill);       uart0_print("\r\n");

    uart0_print("  rx_size=");        uart0_print_i32(C->rx_size);      uart0_print("\r\n");
    uart0_print("  tx_rb_size=");     uart0_print_i32(C->tx_rb_size);   uart0_print("\r\n");
    uart0_print("  tx_low_wm=");      uart0_print_i32(C->tx_low_wm);    uart0_print("\r\n");

    uart0_print("  bulk_active=");    uart0_print_i32(C->bulk_active);  uart0_print("\r\n");
    uart0_print("  bulk_finished=");  uart0_print_i32(C->bulk_finished);uart0_print("\r\n");
    uart0_print("  tx_total_recv=");  uart0_print_i32(C->tx_total_recv);uart0_print("\r\n");
    uart0_print("  tx_sent_in_round="); uart0_print_i32(C->tx_sent_in_round); uart0_print("\r\n");

    uart0_print("  tx_rb_head=");     uart0_print_i32(C->tx_rb_head);
    uart0_print("  tx_rb_tail=");     uart0_print_i32(C->tx_rb_tail);
    uart0_print("\r\n");

    uart0_print("-----------------------\r\n");
}
