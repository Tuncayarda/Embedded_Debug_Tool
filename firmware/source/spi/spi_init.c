/*
 * spi_init.cc
 *
 *  Created on: Sep 18, 2025
 *      Author: tuncayardaaydin
 *
 *  Amaç:
 *  -----
 *  - CSPI transferleri için SPI slave başlatma ve IRQ işleyici (ISR) mantığı.
 *  - NXP DSPI sürücüsü kullanılır (Slave mod).
 *  - TX ring buffer'dan veri besleme, opsiyonel RX yakalama,
 *    ve gönderilen baytlardan basit bir "eşik karşılaştırma" ile GPIO sürme.
 */

#include "action/action.h"
#include "gpio/gpio_utils.h"
#include "spi.h"

/* --- Global Durum --- */

/* Eşik kontrolü için son gönderilen baytları toplayan kaydırmalı akümülatör. */
static volatile uint32_t g_tx_acc  = 0;
/* g_tx_acc içine kaç bayt yüklendi (0..4). */
static volatile uint8_t  g_tx_cnt  = 0;
/* Aktif CSPI konfigürasyonu (parse edilmiş alanlar + ring adresleri). */
static volatile t_cspi_fields *g_cspi = NULL;
/* Bir "round" bittiğinde (TX boyutu tamamlandı ya da RX hedefe ulaşıldı) set edilir. */
volatile bool g_spi_done = false;

/* --- Ring buffer yardımcıları --- */

/**
 * @brief Ring içinde kullanılan byte sayısı.
 */
static inline uint16_t rb_count(uint16_t h, uint16_t t, uint16_t sz){
    return (h>=t) ? (h-t) : (uint16_t)(sz - (t-h));
}

/**
 * @brief Ring içinde boş byte sayısı (her zaman bir boş slot bırak).
 */
static inline uint16_t rb_free(uint16_t h, uint16_t t, uint16_t sz){
    return (uint16_t)(sz - rb_count(h,t,sz) - 1);
}

/**
 * @brief TX ring'e veri it (hosttan gelen chunk'ı parçalayarak sığdırır).
 *
 * @return Yazılan byte sayısı.
 */
size_t cspi_tx_push(t_cspi_fields *C, const uint8_t *data, size_t len)
{
    size_t w = 0;
    while (w < len) {
        uint16_t h=C->tx_rb_head, t=C->tx_rb_tail;
        uint16_t free = rb_free(h,t,C->tx_rb_size);
        if (!free) break; /* Ring dolu → dur. */

        /* Ring sonuna kadar olan kesintisiz boşluk. */
        uint16_t to_end = (t>h) ? (uint16_t)(t - h - 1)
                                : (uint16_t)(C->tx_rb_size - h - (t==0?1:0));
        if (!to_end) {
            /* Sona geldik → başa sar. */
            C->tx_rb_head = h = 0;
            to_end = (t==0) ? (uint16_t)(C->tx_rb_size - 1)
                            : (uint16_t)(t - 1);
            if (!to_end) break;
        }

        uint16_t chunk = (uint16_t)((len - w) > to_end ? to_end : (len - w));
        memcpy(&C->tx_rb[h], data + w, chunk);
        C->tx_rb_head = (uint16_t)((h + chunk) % C->tx_rb_size);
        w += chunk;
    }
    return w;
}

/* --- SPI IRQ İşleyici --- */

/**
 * @brief SPI0 ISR.
 *
 * - RX FIFO boşaltma isteği geldikçe byte okur.
 * - TX için ring'de veri varsa onu, yoksa idle_fill gönderir.
 * - Gönderilen baytları eşik karşılaştırması için g_tx_acc içine biriktirir.
 * - "transfer_size" kadar byte gönderildiğinde round biter ve g_spi_done set edilir.
 */
void SPI0_IRQHandler(void)
{
    if (!g_cspi) {
        /* Oturum yoksa bayrakları temizle ve çık. */
        DSPI_ClearStatusFlags(SPIx, kDSPI_RxFifoDrainRequestFlag | kDSPI_TxFifoFillRequestFlag);
        SDK_ISR_EXIT_BARRIER;
        return;
    }

    /* --- RX/TV besleme döngüsü --- */
    while (DSPI_GetStatusFlags(SPIx) & kDSPI_RxFifoDrainRequestFlag)
    {
        /* 1) RX: Master'dan gelen bir kelimeyi çek. */
        uint32_t rxw = DSPI_ReadData(SPIx);
        uint8_t  rxb = (uint8_t)rxw;

        /* RX yakalama açıksa RAM'e koy. */
        if (g_cspi->rx_data && (g_cspi->rx_offset < g_cspi->rx_size)) {
            g_cspi->rx_data[g_cspi->rx_offset++] = rxb;
        }

        /* 2) TX: Ring'de byte varsa onu, yoksa idle_fill kullan. */
        uint32_t txw;
        bool have_tx = false;

        if (g_cspi->bulk_active) {
            uint16_t head = g_cspi->tx_rb_head;
            uint16_t tail = g_cspi->tx_rb_tail;
            uint16_t cnt  = (head >= tail)
                          ? (uint16_t)(head - tail)
                          : (uint16_t)(g_cspi->tx_rb_size - (tail - head));

            if (cnt > 0u) {
                txw = g_cspi->tx_rb[tail];
                g_cspi->tx_rb_tail = (uint16_t)((tail + 1u) % g_cspi->tx_rb_size);
                have_tx = true;
            }
        }

        if (!have_tx) {
            txw = g_cspi->idle_fill;  /* Underflow'ta gönderilecek dolgu byte'ı. */
        }

        /* 3) Eşik karşılaştırma için gönderilen baytı akümülatöre kaydırarak ekle. */
        uint8_t n = g_cspi->transfer_size;   /* round uzunluğu (byte) */
        if (n == 0) n = 1;
        if (n > 4)  n = 4;                   /* 32-bit'e kadar destek. */

        /* n baytlık pencere maskesi (1→0xFF, 2→0xFFFF, 3→0xFFFFFF, 4→0xFFFFFFFF). */
        uint32_t mask = (n == 4) ? 0xFFFFFFFFu : ((1u << (8u * n)) - 1u);
        uint8_t  txb  = (uint8_t)txw;

        g_tx_acc = ((g_tx_acc << 8) | txb) & mask;
        if (g_tx_cnt < n) g_tx_cnt++;

        /* 4) Byte'ı DSPI TX'e yaz ve bayrakları temizle. */
        DSPI_SlaveWriteData(SPIx, txw);
        g_cspi->tx_sent_in_round++;
        DSPI_ClearStatusFlags(SPIx, kDSPI_RxFifoDrainRequestFlag | kDSPI_TxFifoFillRequestFlag);
    }

    /* --- Round bitiş kontrolü (TX boyutu ve/veya RX hedefi) --- */
    bool tx_round_done = (g_cspi->transfer_size > 0u) &&
                         (g_cspi->tx_sent_in_round >= g_cspi->transfer_size);
    bool rx_round_done = (g_cspi->rx_size > 0u) &&
                         (g_cspi->rx_offset >= g_cspi->rx_size);

    if (tx_round_done) {
        /* Gönderilen n byte'lık pencereyi eşik ile kıyasla ve GPIO sür. */
        uint8_t  n   = g_cspi->transfer_size;
        if (n == 0) n = 1;
        if (n > 4)  n = 4;
        uint32_t mask = (n == 4) ? 0xFFFFFFFFu : ((1u << (8u * n)) - 1u);

        uint32_t val = g_tx_acc & mask;              /* son n byte */
        uint32_t ref = g_cspi->threshold_val & mask; /* eşik */

        if (val > ref) {
            gpio_write_high((gpio_port_t)g_cspi->port, g_cspi->pin);
        } else {
            gpio_write_low((gpio_port_t)g_cspi->port, g_cspi->pin);
        }

        /* Sonraki round için sayaçları temizle. */
        g_tx_acc = 0;
        g_tx_cnt = 0;
        g_cspi->tx_sent_in_round = 0;
    }

    if (tx_round_done || rx_round_done) {
        g_spi_done = true;
        DSPI_DisableInterrupts(SPIx, kDSPI_RxFifoDrainRequestInterruptEnable);
    }

    SDK_ISR_EXIT_BARRIER;
}

/* --- Başlatma --- */

/**
 * @brief DSPI'yi slave modda başlat, GPIO'yu hazırla, IRQ'yu aç.
 *
 * - C->mode (CPOL/CPHA) ve C->word_size (bit/çerçeve) ayarlanır.
 * - Eşik sonucu için kullanılacak GPIO pin çıkışa alınır.
 * - RX FIFO drain IRQ etkinleştirilir ve transfer başlatılır.
 */
void spi_init(t_cspi_fields *C)
{
    dspi_slave_config_t slaveConfig;
    g_cspi = C;

    /* CTAR ve mod/kenar ayarları. */
    slaveConfig.whichCtar                  = kDSPI_Ctar0;
    slaveConfig.ctarConfig.bitsPerFrame    = C->word_size;
    slaveConfig.ctarConfig.cpol            = (C->mode == 2 || C->mode == 3) ? kDSPI_ClockPolarityActiveLow : kDSPI_ClockPolarityActiveHigh;
    slaveConfig.ctarConfig.cpha            = (C->mode == 1 || C->mode == 3) ? kDSPI_ClockPhaseSecondEdge : kDSPI_ClockPhaseFirstEdge;
    slaveConfig.enableContinuousSCK        = false;
    slaveConfig.enableRxFifoOverWrite      = false;
    slaveConfig.enableModifiedTimingFormat = false;
    slaveConfig.samplePoint                = kDSPI_SckToSin0Clock;

    /* Eşik çıktısı için GPIO yapılandır */
    gpio_enable_clock((gpio_port_t)C->port);
    gpio_set_mux((gpio_port_t)C->port, C->pin);
    gpio_set_output((gpio_port_t)C->port, C->pin);

    DSPI_SlaveInit(SPIx, &slaveConfig);

    DSPI_StopTransfer(SPIx);
    DSPI_FlushFifo(SPIx, true, true);
    DSPI_ClearStatusFlags(SPIx, kDSPI_AllStatusFlag);
    DSPI_EnableInterrupts(SPIx, kDSPI_RxFifoDrainRequestInterruptEnable);
    DSPI_StartTransfer(SPIx);
    EnableIRQ(SPI0_IRQn);
}
