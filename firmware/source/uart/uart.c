#include <stdint.h>
#include "uart.h"
#include "fsl_uart_edma.h"
#include "fsl_dmamux.h"

uart_edma_handle_t g_uartEdmaHandle;

edma_handle_t      g_uartTxEdmaHandle;

void uart_tx_on_done(void);

static void uart_tx_cb(UART_Type *base,
                       uart_edma_handle_t *h,
                       status_t st,
                       void *ud)
{
    (void)base;
    (void)h;
    (void)ud;

    if (st == kStatus_UART_TxIdle) {
        uart_tx_on_done();
    }
}

void uart0_init(void)
{
    /* ---- 1) UART configuration ---- */
    uart_config_t uart_cfg;
    UART_GetDefaultConfig(&uart_cfg);
    uart_cfg.baudRate_Bps = UARTx_BAUDRATE;  /* From uart.h */
    uart_cfg.enableTx     = true;
    uart_cfg.enableRx     = true;

    UART_Init(UARTx, &uart_cfg, UARTx_CLK_FREQ);


    DMAMUX_SetSource(UART_DMAMUX_BASEADDR,
                     UART_TX_DMA_CHANNEL,
                     UART_TX_DMA_REQUEST);
    DMAMUX_EnableChannel(UART_DMAMUX_BASEADDR, UART_TX_DMA_CHANNEL);

    EDMA_CreateHandle(&g_uartTxEdmaHandle,
                      UART_DMA_BASEADDR,
                      UART_TX_DMA_CHANNEL);

    UART_TransferCreateHandleEDMA(
        UARTx,
        &g_uartEdmaHandle,
        uart_tx_cb,
        NULL,
        &g_uartTxEdmaHandle,
        NULL
    );

    UART_EnableRxDMA(UARTx, true);
}
