#include "gpio_utils.h"

/* Port enum → PORT_Type adresini döndürür. */
static inline PORT_Type* port_base(gpio_port_t idx) {
    switch (idx) {
        case GPIO_PORT_A: return PORTA;
        case GPIO_PORT_B: return PORTB;
        case GPIO_PORT_C: return PORTC;
        case GPIO_PORT_D: return PORTD;
        case GPIO_PORT_E: return PORTE;
        default: return NULL;
    }
}

/* Port enum → GPIO_Type adresini döndürür. */
static inline GPIO_Type* gpio_base(gpio_port_t idx) {
    switch (idx) {
        case GPIO_PORT_A: return GPIOA;
        case GPIO_PORT_B: return GPIOB;
        case GPIO_PORT_C: return GPIOC;
        case GPIO_PORT_D: return GPIOD;
        case GPIO_PORT_E: return GPIOE;
        default: return NULL;
    }
}

/* İlgili GPIO portunun clock’unu açar. */
void gpio_enable_clock(gpio_port_t port) {
    switch (port) {
        case GPIO_PORT_A: CLOCK_EnableClock(kCLOCK_PortA); break;
        case GPIO_PORT_B: CLOCK_EnableClock(kCLOCK_PortB); break;
        case GPIO_PORT_C: CLOCK_EnableClock(kCLOCK_PortC); break;
        case GPIO_PORT_D: CLOCK_EnableClock(kCLOCK_PortD); break;
        case GPIO_PORT_E: CLOCK_EnableClock(kCLOCK_PortE); break;
        default: break;
    }
}

/* Pin mux’unu GPIO olarak ayarlar. */
void gpio_set_mux(gpio_port_t port, uint8_t pin) {
    PORT_Type *P = port_base(port);
    if (!P) return;
    PORT_SetPinMux(P, pin, kPORT_MuxAsGpio);
}

/* Bir pini dijital çıkış yapar. */
void gpio_set_output(gpio_port_t port, uint8_t pin) {
    GPIO_Type *G = gpio_base(port);
    if (!G) return;
    G->PDDR |= (1u << pin);
}

/* Bir pini dijital giriş yapar (pull-up/down direnç de açılır). */
void gpio_set_input(gpio_port_t port, uint8_t pin) {
    GPIO_Type *G = gpio_base(port);
    if (!G) return;

    gpio_pin_config_t pin_cfg = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic  = 0U
    };

    GPIO_PinInit(G, pin, &pin_cfg);


    //Pull down işlevi için gerekli olan kısım
    PORTD->PCR[pin] = ((PORTD->PCR[pin] &
                       (~(PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))
                       | (uint32_t)(PORT_PCR_PE_MASK));
}

/* Pini HIGH (1) yapar. */
void gpio_write_high(gpio_port_t port, uint8_t pin) {
    GPIO_Type *G = gpio_base(port);
    if (!G) return;
    G->PSOR = (1u << pin);
}

/* Pini LOW (0) yapar. */
void gpio_write_low(gpio_port_t port, uint8_t pin) {
    GPIO_Type *G = gpio_base(port);
    if (!G) return;
    G->PCOR = (1u << pin);
}

/* Pin seviyesini okur (0=LOW, 1=HIGH, 0xFF=hatalı port). */
uint8_t gpio_read(gpio_port_t port, uint8_t pin) {
    GPIO_Type *G = gpio_base(port);
    if (!G) return 0xFF;
    return (uint8_t)((G->PDIR >> pin) & 1u);
}
