/*
 * tx.c
 *
 *  Created on: Sep 24, 2025
 *      Author: tuncayardaaydin
 *
 */

#include <string.h>
#include "uart.h"
#include "fsl_uart_edma.h"

/* ---------------------------- Ring buffer state ---------------------------- */

/* TX ring storage. Align to ring size (good for DMA and future optimizations). */
__attribute__((aligned(UART_TX_RING_SZ)))
static uint8_t s_txRing[UART_TX_RING_SZ];

/* Head: next byte index the producer (app) will write. */
static volatile uint16_t s_head   = 0;
/* Tail: next byte index the consumer (DMA) will read/send. */
static volatile uint16_t s_tail   = 0;

/* True while an EDMA transfer is in flight. */
static volatile bool     s_busy   = false;
/* Length of the last chunk given to UART_SendEDMA() (used to advance tail). */
static volatile uint16_t s_lastLen = 0;

/* Reused transfer descriptor passed to UART_SendEDMA(). */
static uart_transfer_t   s_txXfer;

/* Handle created by UART_TransferCreateHandleEDMA() (provided by uart0_init.c). */
extern uart_edma_handle_t g_uartEdmaHandle;

/* ------------------------------- Utilities -------------------------------- */

static inline bool rb_empty(void)  { return s_head == s_tail; }

/* Ring full when advancing head by 1 would hit tail. */
static inline bool rb_full(void)
{
    return (uint16_t)((s_head + 1u) % UART_TX_RING_SZ) == s_tail;
}

/* Number of contiguous bytes available from current tail to either head or end. */
static inline uint16_t rb_contig_len_from_tail(void)
{
    return (s_head >= s_tail)
         ? (uint16_t)(s_head - s_tail)                  /* [tail .. head) */
         : (uint16_t)(UART_TX_RING_SZ - s_tail);        /* [tail .. end)  */
}

/* ------------------------------- DMA kicker ------------------------------- */
/**
 * @brief If no DMA is running and ring is non-empty, start one EDMA send
 *        for the largest contiguous run from tail.
 *
 * Implementation detail:
 *  - We do *not* span the wrap in a single DMA request; we send the headless
 *    tail segment first, then the callback will advance tail and kick again.
 */
static void uart0_tx_kick_if_idle(void)
{
    if (s_busy || rb_empty()) return;

    uint16_t len = rb_contig_len_from_tail();
    if (!len) return;

    s_txXfer.data     = &s_txRing[s_tail];
    s_txXfer.dataSize = len;
    s_lastLen         = len;
    s_busy            = true;

    UART_EnableTxDMA(UARTx, true);
    (void)UART_SendEDMA(UARTx, &g_uartEdmaHandle, &s_txXfer);
}

/* --------------------------- DMA completion hook -------------------------- */
/**
 * @brief Called from the UART EDMA callback when st == kStatus_UART_TxIdle.
 *
 * Advances the tail by the last submitted chunk and immediately kicks the
 * next segment if available; otherwise disables Tx DMA.
 */
void uart_tx_on_done(void)
{
    /* Consume the chunk we just finished transmitting. */
    s_tail    = (uint16_t)((s_tail + s_lastLen) % UART_TX_RING_SZ);
    s_lastLen = 0;
    s_busy    = false;

    /* Try to send the next contiguous block (if any). */
    uart0_tx_kick_if_idle();

    /* No more data? Save power/noise by disabling Tx DMA. */
    if (!s_busy && rb_empty()) {
        UART_EnableTxDMA(UARTx, false);
    }
}

/* ----------------------------- Public API --------------------------------- */
/**
 * @brief Enqueue up to @len bytes into the TX ring.
 *
 * @param data Pointer to source bytes
 * @param len  Requested length to enqueue
 * @return     Number of bytes actually enqueued (can be < len if ring is full)
 *
 * Side effect: If the DMA is idle and we enqueued at least one byte,
 *              this will kick the DMA to start transmitting.
 */
size_t uart0_write(const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t*)data;
    size_t wr = 0;

    while (wr < len) {
        if (rb_full()) break;

        uint16_t next = (uint16_t)((s_head + 1u) % UART_TX_RING_SZ);
        s_txRing[s_head] = p[wr++];
        s_head = next;
    }

    if (wr) uart0_tx_kick_if_idle();
    return wr;
}

/* Convenience helpers */
void uart0_putc(char c)           { (void)uart0_write(&c, 1); }
void uart0_print(const char *s)   { (void)uart0_write(s, strlen(s)); }

static inline char nib_to_hex(uint8_t v){ return (v < 10) ? (char)('0'+v) : (char)('A'+(v-10)); }
void uart0_puthex(uint8_t b)      { char hx[3]={nib_to_hex(b>>4),nib_to_hex(b&0xF),' '}; (void)uart0_write(hx,3); }
void uart0_puthex_pref(uint8_t b) { const char p[]="0x"; (void)uart0_write(p,2); uart0_puthex(b); }

/**
 * @brief Minimal int32 print (no malloc/printf).
 */
void uart0_print_i32(int32_t v)
{
    char buf[16]; int i=0;
    if (v==0){ uart0_putc('0'); return; }
    if (v<0){ uart0_putc('-'); v=-v; }
    while (v && i<(int)sizeof(buf)) { buf[i++] = (char)('0' + (v%10)); v/=10; }
    while (i--) uart0_putc(buf[i]);
}
