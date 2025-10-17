#include <stdint.h>
#include "uart.h"
#include "peripherals.h"

__attribute__((aligned(UART_RX_RING_SZ)))
volatile uint8_t rxRing[UART_RX_RING_SZ];

static volatile uint16_t s_rxTail = 0;

/* Mask helper for power-of-two ring sizes (fast modulo). */
static inline uint16_t rx_mask(uint16_t v)
{
    return (uint16_t)(v & (UART_RX_RING_SZ - 1u));
}


static inline uint16_t rx_head_hw(void)
{
    __DSB();
    __ISB();
    // DMA kanalının Destination Address Register (DADDR)'ını oku.
    // Yani DMA şu anda RAM'de hangi adrese veri yazıyor?
    uint32_t daddr = DMA_DMA_BASEADDR->TCD[DMA_CH0_DMA_CHANNEL].DADDR;

    // RX ring buffer'ın başlangıç adresini al.
    uint32_t base  = (uint32_t)&rxRing[0];

    // DMA'nın yazdığı adres ile ring buffer'ın başlangıcı arasındaki fark,
    // şu anki head (yazma konumu)'dur.
    return (uint16_t)(daddr - base);
}

//ring bufferda veri var ise okuyup döndürür
int uart0_try_read_byte(uint8_t *out)
{
    uint16_t head = rx_head_hw();
    uint16_t tail = s_rxTail;

    if (head == tail)
        return 0;  /* ring empty */

    *out = rxRing[tail];
    s_rxTail = rx_mask((uint16_t)(tail + 1u));
    return 1;
}
