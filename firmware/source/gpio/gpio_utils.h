/*
 * gpio.h
 *
 *  Created on: Sep 12, 2025
 *      Author: tuncayardaaydin
 *
 *  Purpose:
 *  --------
 *  - Provides utility functions to configure and control GPIO pins.
 *  - Wraps NXP SDK functions (fsl_gpio, fsl_port, fsl_clock) into simpler APIs.
 *  - Used throughout the project for pin initialization, reading, and writing.
 */

#ifndef GPIO_GPIO_UTILS_H_
#define GPIO_GPIO_UTILS_H_

#include <stdint.h>
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"


typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B = 1,
    GPIO_PORT_C = 2,
    GPIO_PORT_D = 3,
    GPIO_PORT_E = 4
} gpio_port_t;


void gpio_enable_clock(gpio_port_t port);


void gpio_set_mux(gpio_port_t port, uint8_t pin);

void gpio_set_output(gpio_port_t port, uint8_t pin);

void gpio_set_input(gpio_port_t port, uint8_t pin);

void gpio_write_high(gpio_port_t port, uint8_t pin);

void gpio_write_low(gpio_port_t port, uint8_t pin);

uint8_t gpio_read(gpio_port_t port, uint8_t pin);

#endif /* GPIO_GPIO_UTILS_H_ */
