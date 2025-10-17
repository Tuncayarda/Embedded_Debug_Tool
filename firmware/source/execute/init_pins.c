/*
 * init_pins.c
 *  execute() başlamadan önce senaryoda geçen tüm pinleri uygun moda kurar.
 */

#include "execute.h"
#include "gpio/gpio_utils.h"
#include "fsl_clock.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "uart/uart.h"

/* Tüm action’larda geçen pinleri tek tek kurar (mux, yön, ilk seviye). */
int init_pins(t_action_set *set)
{
    if (!set) return -1;

    for (int i = 0; i < set->count; i++) {
        t_action_rec *a = &set->actions[i];

        switch (a->type) {

        case TYPE_PIN_READ: {
            /* Okuma için: saat aç, mux=GPIO, yön=giriş */
            uint8_t port = a->u.pin_read.port;
            uint8_t pin  = a->u.pin_read.pin;
            if (port > GPIO_PORT_E || pin >= 32) break;

            gpio_enable_clock((gpio_port_t)port);
            gpio_set_mux((gpio_port_t)port, pin);
            gpio_set_input((gpio_port_t)port, pin);
        } break;

        case TYPE_PIN_TRIGGER: {
            /* Tetik için: saat aç, mux=GPIO, yön=giriş */
            uint8_t port = a->u.pin_trigger.port;
            uint8_t pin  = a->u.pin_trigger.pin;
            if (port > GPIO_PORT_E || pin >= 32) break;

            gpio_enable_clock((gpio_port_t)port);
            gpio_set_mux((gpio_port_t)port, pin);
            gpio_set_input((gpio_port_t)port, pin);
        } break;

        case TYPE_PIN_WRITE: {
            /* Yazma için: saat aç, mux=GPIO, ilk seviyeyi sür, sonra yön=çıkış */
            uint8_t port = a->u.pin_write.port;
            uint8_t pin  = a->u.pin_write.pin;
            if (port > GPIO_PORT_E || pin >= 32) break;

            gpio_enable_clock((gpio_port_t)port);
            gpio_set_mux((gpio_port_t)port, pin);

            if (a->u.pin_write.initial == LVL_HIGH)
                gpio_write_high((gpio_port_t)port, pin);
            else
                gpio_write_low((gpio_port_t)port, pin);

            gpio_set_output((gpio_port_t)port, pin);
        } break;

        default:
            /* Bu tip için pin hazırlığı yok */
            break;
        }
    }
    return 0;
}
