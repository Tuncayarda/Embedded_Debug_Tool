/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    Debug_Tool.c
 * @brief   Debug Tool ana döngü (UART/Flash/PIT/SPI yönetimi ve host protokolü)
 */

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "core_cm4.h"

#include "uart/uart.h"
#include "uart/uart_proto.h"
#include "action/action.h"
#include "execute/execute.h"
#include "flash/flash.h"
#include "pit/pit.h"
#include "spi/spi.h"
#include "debug.h"

/* --------------------------- CSPI oturum durumu --------------------------- */
extern volatile bool g_spi_done;            // ISR round bittiğinde set edilir
static volatile bool g_cspi_active = false; // Aktif CSPI oturumu bayrağı
static t_cspi_fields gC;                    // Host’tan parse edilen CSPI ayarları/durum
static volatile uint8_t s_req_inflight = 0; // Host’a refill REQ atıldı, cevabı bekleniyor

/* ------------------------ CSPI: host’a REQ paketi gönder ------------------ */
// TX ring’inde low-watermark koşulu oluştuğunda host’tan veri istemek için
static void cspi_send_req(void)
{
    if (s_req_inflight) return; // aynı anda birden fazla REQ atma

    uint8_t buf[8];
    size_t n = build_packet(MSG_ID_CSPI_REQ, NULL, 0, buf); // projeye özel framing
    uart0_write(buf, n);
    s_req_inflight = 1;

    uart0_print("CSPI REQ\r\n"); // iz takibi için kısa log
}

/* ------------------------- CSPI: low-watermark kontrolü ------------------- */
// TX ring doluluk azaldığında ve yeterli boş yer olduğunda REQ tetikler
static void cspi_check_ring(t_cspi_fields *C)
{
    if (!g_cspi_active || !C || !C->bulk_active || C->bulk_finished) return;

    uint16_t head = C->tx_rb_head, tail = C->tx_rb_tail;
    uint16_t used = (head >= tail) ? (head - tail)
                                   : (uint16_t)(C->tx_rb_size - (tail - head));
    uint16_t free = (uint16_t)(C->tx_rb_size - used - 1);

    if ((used <= C->tx_low_wm) && (free >= 512) && !s_req_inflight) {
        cspi_send_req();
    }
}

/* ---------------------------- CSPI: oturumu kapat ------------------------- */
// SPI’yi durdur, bayrakları temizle, CSPI kaynaklarını bırak
static void cspi_shutdown(void)
{
    DSPI_DisableInterrupts(SPIx, kDSPI_RxFifoDrainRequestInterruptEnable);
    DSPI_StopTransfer(SPIx);
    DSPI_FlushFifo(SPIx, true, true);
    DSPI_ClearStatusFlags(SPIx, kDSPI_AllStatusFlag);

    g_cspi_active   = false;
    s_req_inflight  = 0;

    free_cspi(&gC); // projeye özel ayrılan bellekleri serbest bırak
    uart0_print("CSPI SHUTDOWN\r\n");
}

/* --------------------------- CSPI: yeni round başlat ---------------------- */
// Bir “round” başlatır; per-round sayaçları sıfırlar, IRQ’yu açar
static inline void cspi_start_one_round(t_cspi_fields *C)
{
    g_spi_done            = false;
    C->rx_offset          = 0;
    C->tx_sent_in_round   = 0;

    DSPI_EnableInterrupts(SPIx, kDSPI_RxFifoDrainRequestInterruptEnable);
    DSPI_StartTransfer(SPIx);
}

/* --------------------------------- main() --------------------------------- */
int main(void)
{
    // NXP board bring-up (pin/clock/peripheral)
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    // BOARD_InitDebugConsole(); // Kendi UART sürücümüzü kullanıyoruz
#endif

    // Sürücüler (projeye özel sıralama)
    uart0_init();     // UART + EDMA (protokol & log)
    flash_init();     // Flash API
    pit_init();       // PIT zaman tabanı
    pit_stop();       // execute() gerektirdikçe başlatılacak

    // Boot sırasında flash’taki çerçeveyi test etmek için geçici tampon
    uint8_t flash_cache[MAX_PAYLOAD + PROTO_CORE_SIZE];

    uart0_print("Debug Tool initialized\r\n");

    // Boot’ta flash üzerinde geçerli frame varsa isteğe bağlı çalıştır
    check_flash(flash_cache, MAX_PAYLOAD + PROTO_CORE_SIZE);

    // Ana servis döngüsü
    while (1)
    {
        uint8_t  msg   = 0;
        uint8_t  payload[MAX_PAYLOAD];
        uint16_t plen  = 0;

        cspi_check_ring(&gC); // TX ring refill durumu (arka plan)

        // UART protokolünden bir frame çek
        int got = proto_rx_poll(&msg, payload, &plen);
        if (got == 1)
        {
            // ------------------------ Host komutları ------------------------

            if (msg == MSG_ID_CLEAR_FLASH) {
                flash_erase();                         // kullanıcı flash’ını sil
                uart0_print("Flash erased\r\n");
            }
            else if (msg == MSG_ID_WRITE_FLASH || msg == MSG_ID_WRITE_FLASH_BOOT) {
                // Host payload’ını on-flash formatına çevirip yaz
                size_t packet_size = build_packet(msg, payload, plen, flash_cache); // projeye özel framing
                flash_erase();
                flash_program(0, flash_cache, packet_size);
                uart0_print("Flash programmed\r\n");
            }
            else if (msg == MSG_ID_EXECUTE_ACTIONS) {
                // Action graph’ı RAM’de parse et ve çalıştır
                t_action_set set;
                int n = parse_actions(payload, plen, &set); // projeye özel graph parser
                if (n > 0) {
                    uart0_print("Executing...\r\n");
                    int st = execute(&set);        // projeye özel executor
                    if (st == -55)
                    	uart0_print("Execution Error!!\r\n");
                    free_actions(&set);
                    uart0_print("Execution completed\r\n");
                } else {
                    uart0_print("Parse error ");
                    uart0_print_i32(n);
                    uart0_print("\r\n");
                }
            }
            else if (msg == MSG_ID_CSPI_BEGIN) {
                // CSPI bulk transfer oturumu başlat (SPI slave)
                int n = parse_cspi_begin(payload, plen, &gC); // projeye özel CSPI konfig parse
                dump_cspi(&gC);                               // konfig diyagnostiği
                if (n == 0) {
                    spi_init(&gC);            // projeye özel SPI init (ring/IRQ vb.)
                    g_cspi_active  = true;
                    s_req_inflight = 0;
                    cspi_send_req();          // ilk refill isteği
                    cspi_start_one_round(&gC);// ilk round’u başlat
                    uart0_print("CSPI BEGIN OK\r\n");
                } else {
                    uart0_print("CSPI parse error ");
                    uart0_print_i32(n);
                    uart0_print("\r\n");
                }
            }
            else if (msg == MSG_ID_CSPI_DATA) {
                // Host TX ring’e ham veri itiyor (bulk path)
                if (!g_cspi_active || !gC.bulk_active) {
                    uart0_print("CSPI DATA ignored (not active)\r\n");
                } else {
                    size_t written = cspi_tx_push(&gC, payload, plen); // projeye özel ring push
                    gC.tx_total_recv += (uint32_t)written;
                    if (written > 0) s_req_inflight = 0; // REQ karşılandı

                    // Kısa ring diyagnostiği (iz takibi)
                    uint16_t head = gC.tx_rb_head;
                    uint16_t tail = gC.tx_rb_tail;
                    uint16_t cnt  = (head >= tail) ? (head - tail)
                                                   : (uint16_t)(gC.tx_rb_size - (tail - head));
                    uint16_t free = (uint16_t)(gC.tx_rb_size - cnt - 1);

                    uart0_print("CSPI DATA push: written=");
                    uart0_print_i32((int32_t)written);
                    uart0_print(" total=");
                    uart0_print_i32((int32_t)gC.tx_total_recv);
                    uart0_print(" head=");
                    uart0_print_i32(head);
                    uart0_print(" tail=");
                    uart0_print_i32(tail);
                    uart0_print(" used=");
                    uart0_print_i32(cnt);
                    uart0_print("/");
                    uart0_print_i32(gC.tx_rb_size);
                    uart0_print(" free=");
                    uart0_print_i32(free);
                    uart0_print("\r\n");

                    // İsteğe bağlı küçük hexdump (en fazla 32 byte)
                    uint16_t dump_len = (cnt > 32) ? 32 : cnt;
                    if (dump_len > 0) {
                        uart0_print("RB data: ");
                        uint16_t idx = tail;
                        for (uint16_t i = 0; i < dump_len; i++) {
                            uart0_puthex(gC.tx_rb[idx]);
                            idx = (uint16_t)((idx + 1) % gC.tx_rb_size);
                        }
                        uart0_print("\r\n");
                    }
                }
            }
            else if (msg == MSG_ID_CSPI_END) {
                // Host artık veri göndermeyecek (drain ve kapanış beklenir)
                if (g_cspi_active && gC.bulk_active) {
                    gC.bulk_finished = 1;
                    s_req_inflight   = 0;
                    uart0_print("CSPI END\r\n");

                    // Ring boş ve RX hedefi tamam ise oturumu kapat
                    uint16_t head = gC.tx_rb_head, tail = gC.tx_rb_tail;
                    uint16_t used = (head >= tail) ? (head - tail)
                                                   : (uint16_t)(gC.tx_rb_size - (tail - head));
                    bool rx_done = (gC.rx_size == 0) || (gC.rx_offset >= gC.rx_size);

                    if (used == 0 && rx_done) {
                        cspi_shutdown();
                    }
                }
            }
            else if (msg == MSG_ID_CSPI_TERMINATE) {
                // Host’ta acil durdurma talebi: anında kapat
                uart0_print("CSPI TERMINATE\r\n");
                s_req_inflight   = 0;
                gC.bulk_finished = 1;
                gC.bulk_active   = 0;
                cspi_shutdown();
            }
            else if (msg == MSG_ID_RESET) {
                NVIC_SystemReset(); // Yazılımdan MCU reset
            }
            else {
                uart0_print("Unknown msg\r\n"); // Tanımsız komut
            }
        }
        else
        {
            // ---------------------- CSPI arka plan servisi ----------------------
            if (g_cspi_active) {
                // SPI ISR round bitti bilgisini g_spi_done ile verir
                if (g_spi_done) {
                    // Bu round’da RX yakalaması varsa bir defa dump et
                    if (gC.rx_size && gC.rx_data && gC.rx_offset > 0) {
                        for (uint16_t i = 0; i < gC.rx_offset; i++) {
                            uart0_puthex_pref(gC.rx_data[i]); // "0xHH " formatı
                        }
                        uart0_print("\r\n");
                    }

                    // Bitirme veya devam etme kararı
                    uint16_t head = gC.tx_rb_head, tail = gC.tx_rb_tail;
                    uint16_t used = (head >= tail) ? (head - tail)
                                                   : (uint16_t)(gC.tx_rb_size - (tail - head));
                    bool rx_done = (gC.rx_size == 0) || (gC.rx_offset >= gC.rx_size);

                    if (gC.bulk_finished && (used == 0) && rx_done) {
                        uart0_print("CSPI DONE total=");
                        uart0_print_i32((int32_t)gC.tx_total_recv);
                        uart0_print("\r\n");
                        cspi_shutdown();
                    } else {
                        // Devam: bir sonraki SPI round’u başlat
                        cspi_start_one_round(&gC);
                    }
                }
            }

            __NOP();
        }
    }
    return 0;
}
