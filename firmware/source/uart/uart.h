#ifndef UART_H_
#define UART_H_

#include "fsl_uart.h"
#include "fsl_device_registers.h"

#define UART_RX_RING_SZ  1024U
extern volatile uint8_t rxRing[UART_RX_RING_SZ];

#define UART_TX_RING_SZ  1024u


#define UARTx              UART0
#define UARTx_CLK_SRC      kCLOCK_CoreSysClk
#define UARTx_CLK_FREQ     CLOCK_GetFreq(UARTx_CLK_SRC)
#define UARTx_BAUDRATE     115200U  /* Default baud rate */



#define UART_DMA_BASEADDR    	DMA0
#define UART_DMAMUX_BASEADDR 	DMAMUX

#define UART_TX_DMA_CHANNEL     1U
#define UART_TX_DMA_REQUEST     kDmaRequestMux0UART0Tx


void uart0_init(void);

size_t uart0_write(const void *data, size_t len);

void uart0_putc(char c);

void uart0_print(const char *s);

void uart0_print_i32(int32_t v);

void uart0_puthex(uint8_t b);

void uart0_puthex_pref(uint8_t b);

int uart0_try_read_byte(uint8_t *out);

uint16_t rx_rb_wrap_add(uint16_t base, uint16_t add);

void uart0_tx_poll(void);

#endif /* UART_H_ */
