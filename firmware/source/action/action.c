/*
 * action.c
 *  Action buffer’ını iki geçişte parse eder, ID’ye göre dizi oluşturur.
 */

#include <string.h>
#include <stdlib.h>
#include "action.h"
#include "pit/pit.h"

/* 16-bit big-endian oku (wire’dan kısa okuma yardımcı) */
static inline uint16_t u16be(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] << 8) | (uint16_t)p[1];
}

/* 32-bit big-endian oku (wire’dan uzun okuma yardımcı) */
static inline uint32_t u32be(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |
           ((uint32_t)p[3]);
}

/* (ms,us) → PIT tick dönüştürme (en yakın tick’e yuvarlar, saturasyon var) */
static inline uint32_t msus_to_ticks(uint32_t ms, uint16_t us)
{
    uint32_t us_ticks = (uint32_t)((us + (PIT_TICK_US/2)) / PIT_TICK_US);
    uint64_t ticks = (uint64_t)ms * (uint64_t)PIT_TICKS_PER_MS + (uint64_t)us_ticks;
    if (ticks > 0xFFFFFFFFu) ticks = 0xFFFFFFFFu;
    return (uint32_t)ticks;
}

/* Action set içindeki tüm dinamik alanları serbest bırakır ve sıfırlar (idempotent) */
void free_actions(t_action_set *S)
{
    if (!S || !S->actions) { if (S) { S->actions=NULL; S->count=0; } return; }
    for (int i=0;i<S->count;i++) {
        if (S->actions[i].targets) {
            free(S->actions[i].targets);
            S->actions[i].targets = NULL;
        }
    }
    free(S->actions);
    S->actions = NULL;
    S->count = 0;
}

/*
 * Wire format
 * START:       [01][ID][TCOUNT][TIDS...]
 * DELAY:       [02][ID][MS:4BE][US:2BE][TCOUNT][TIDS...]
 * PIN_READ:    [03][ID][PORT][PIN][INIT][TARGET][FINAL][MS:4BE][US:2BE][TCOUNT][TIDS...]
 * PIN_WRITE:   [04][ID][PORT][PIN][INIT][TARGET][FINAL][MS:4BE][US:2BE][TCOUNT][TIDS...]
 * PIN_TRIGGER: [05][ID][PORT][PIN][INIT][TARGET][TO_MS:4BE][TO_US:2BE][TCOUNT][TIDS...]
 *
 * Dönüş kodları (<0): -1 arg, -10.. uzunluk, -21 tip, -30/31 alloc hatası.
 */

/* İki geçişli parser: 1) doğrula&max ID bul, 2) ID indeksli diziye doldur */
int parse_actions(const uint8_t *pl, uint16_t len, t_action_set *S)
{
    if (!pl || !S) return -1;
    memset(S, 0, sizeof(*S));

    uint16_t i = 0;
    int max_id = -1;

    /* Geçiş-1: uzunluk kontrolü ve max ID tespiti (hafızayı doğru boyutlamak için) */
    while (i < len) {
        if (i + 2 > len) return -10;
        uint8_t type = pl[i++];
        uint8_t id   = pl[i++];
        if (id > max_id) max_id = id;

        switch (type) {
        case TYPE_START: {
            if (i + 1 > len) return -12;
            uint8_t ct = pl[i++];
            if (i + ct > len) return -14;
            i += ct;  /* target ID’leri atla */
        } break;

        case TYPE_DELAY: {
            /* 4B ms + 2B us + 1B ct + ctB ids */
            if (i + 4 + 2 + 1 > len) return -15;
            i += 4;  /* ms */
            i += 2;  /* us */
            uint8_t ct = pl[i++];
            if (i + ct > len) return -17;
            i += ct;
        } break;

        case TYPE_PIN_READ:
        case TYPE_PIN_WRITE: {
            /* 1+1+1+1+1 + 4 + 2 + 1 + ct */
            if (i + 5 + 4 + 2 + 1 > len) return -18;
            i += 5;  /* port,pin,init,target,final */
            i += 4;  /* ms */
            i += 2;  /* us */
            uint8_t ct = pl[i++];
            if (i + ct > len) return -20;
            i += ct;
        } break;

        case TYPE_PIN_TRIGGER: {
            /* 1+1+1+1 + 4 + 2 + 1 + ct */
            if (i + 4 + 4 + 2 + 1 > len) return -22;
            i += 4;  /* port,pin,init,target */
            i += 4;  /* timeout_ms */
            i += 2;  /* timeout_us */
            uint8_t ct = pl[i++];
            if (i + ct > len) return -23;
            i += ct;
        } break;

        default:
            return -21;  /* bilinmeyen tip */
        }
    }

    int N = (max_id >= 0) ? (max_id + 1) : 0;
    if (N == 0) { S->actions = NULL; S->count = 0; return 0; }

    S->actions = (t_action_rec*)calloc((size_t)N, sizeof(t_action_rec)); /* ID doğrudan indeks */
    if (!S->actions) return -30;
    S->count = N;

    /* Geçiş-2: kayıtları ID alanına göre doldur (hedef listesi varsa malloc) */
    i = 0;
    while (i < len) {
        t_action_rec a; memset(&a, 0, sizeof(a));
        a.type   = pl[i++];
        a.id     = pl[i++];
        a.status = STATUS_IDLE;
        a.error  = ERROR_NONE;

        switch (a.type) {
        case TYPE_START: {
            uint8_t ct = pl[i++];
            a.target_count = ct;
            if (ct) {
                a.targets = (uint8_t*)malloc((size_t)ct);
                if (!a.targets) { free_actions(S); return -31; }
                for (uint8_t k=0; k<ct; k++) a.targets[k] = pl[i++];
            }
        } break;

        case TYPE_DELAY: {
            a.u.delay.duration_ms = u32be(&pl[i]); i += 4;
            a.u.delay.duration_us = u16be(&pl[i]); i += 2;
            a.u.delay.duration_ticks = msus_to_ticks(a.u.delay.duration_ms, a.u.delay.duration_us);

            uint8_t ct = pl[i++];
            a.target_count = ct;
            if (ct) {
                a.targets = (uint8_t*)malloc((size_t)ct);
                if (!a.targets) { free_actions(S); return -31; }
                for (uint8_t k=0; k<ct; k++) a.targets[k] = pl[i++];
            }
        } break;

        case TYPE_PIN_READ:
        case TYPE_PIN_WRITE: {
            /* Ortak alanları topla, tipe göre union’a yerleştir (ticks önceden hesap) */
            uint8_t port  = pl[i++], pin = pl[i++];
            uint8_t init  = pl[i++], tgt = pl[i++], fin = pl[i++];
            uint32_t dur_ms = u32be(&pl[i]); i += 4;
            uint16_t dur_us = u16be(&pl[i]); i += 2;
            uint8_t ct      = pl[i++];

            if (a.type == TYPE_PIN_READ) {
                a.u.pin_read.port    = port;
                a.u.pin_read.pin     = pin;
                a.u.pin_read.initial = init;
                a.u.pin_read.target  = tgt;
                a.u.pin_read.final   = fin;
                a.u.pin_read.duration_ms    = dur_ms;
                a.u.pin_read.duration_us    = dur_us;
                a.u.pin_read.duration_ticks = msus_to_ticks(dur_ms, dur_us);
            } else {
                a.u.pin_write.port    = port;
                a.u.pin_write.pin     = pin;
                a.u.pin_write.initial = init;
                a.u.pin_write.target  = tgt;
                a.u.pin_write.final   = fin;
                a.u.pin_write.duration_ms    = dur_ms;
                a.u.pin_write.duration_us    = dur_us;
                a.u.pin_write.duration_ticks = msus_to_ticks(dur_ms, dur_us);
            }

            a.target_count = ct;
            if (ct) {
                a.targets = (uint8_t*)malloc((size_t)ct);
                if (!a.targets) { free_actions(S); return -31; }
                for (uint8_t k=0; k<ct; k++) a.targets[k] = pl[i++];
            }
        } break;

        case TYPE_PIN_TRIGGER: {
            uint8_t port = pl[i++], pin = pl[i++];
            uint8_t init = pl[i++], tgt = pl[i++];
            uint32_t timeout_ms = u32be(&pl[i]); i += 4;
            uint16_t timeout_us = u16be(&pl[i]); i += 2;
            uint8_t ct          = pl[i++];

            a.u.pin_trigger.port         = port;
            a.u.pin_trigger.pin          = pin;
            a.u.pin_trigger.initial      = init;
            a.u.pin_trigger.target       = tgt;
            a.u.pin_trigger.timeout_ms   = timeout_ms;
            a.u.pin_trigger.timeout_us   = timeout_us;
            a.u.pin_trigger.duration_ticks = msus_to_ticks(timeout_ms, timeout_us);

            a.target_count = ct;
            if (ct) {
                a.targets = (uint8_t*)malloc((size_t)ct);
                if (!a.targets) { free_actions(S); return -31; }
                for (uint8_t k=0; k<ct; k++) a.targets[k] = pl[i++];
            }
        } break;

        default:
            free_actions(S);
            return -21;  /* ilk geçişte yakalanmalıydı */
        }

        /* ID’ye göre yerleştir (ilk geçişte N = max_id+1 boyutlandı) */
        S->actions[a.id] = a;
    }

    return S->count;
}

/* CSPI “begin” bloğunu t_cspi_fields’e parse eder ve ring/bellekleri kurar */
int parse_cspi_begin(const uint8_t *pl, uint16_t len, t_cspi_fields *C)
{
    if (!pl || !C) return -1;
    if (len < 13)  return -2;

    memset(C, 0, sizeof(*C));

    C->mode          = pl[0];                /* SPI mode (0..3) */
    C->word_size     = pl[1];                /* şu an 8 bit destekli */
    C->rx_size       = u16be(&pl[2]);        /* Masterdan okunacak veri boyutu */
    C->transfer_size = pl[4];                /* Mastera her roundda gönderilecek veri */
    C->threshold_val = u32be(&pl[7]);        /* projeye özgü eşik/değer */
    C->port          = pl[11];				 /* Uyarı için pin ve port */
    C->pin           = pl[12];

    /* Varsayılan çalışma ayarları (ring boyutu/low-watermark) */
    C->idle_fill   = 0x00;       /* TX ring boşsa gönderilecek dolgu byte’ı */
    C->tx_rb_size  = 2048;       /* TX ring kapasitesi (byte) */
    C->tx_low_wm   = 256;        /* refill için düşük eşik (ipucu) */

    if (C->tx_low_wm == 0 || C->tx_low_wm >= C->tx_rb_size)
        C->tx_low_wm = (uint16_t)(C->tx_rb_size / 4);

    /* Basit doğrulamalar (bu uygulama kısıtları) */
    if (C->mode > 3)       return -3;  /* geçersiz SPI modu */
    if (C->word_size != 8) return -4;  /* sadece 8-bit kelime desteklenir */

    /* TX ring tahsisi ve indekslerin sıfırlanması */
    C->tx_rb = (uint8_t*)malloc(C->tx_rb_size);
    if (!C->tx_rb) return -5;
    C->tx_rb_head = 0;
    C->tx_rb_tail = 0;

    /* İsteğe bağlı RX yakalama tamponu */
    if (C->rx_size > 0) {
        C->rx_data = (uint8_t*)malloc(C->rx_size);
        if (!C->rx_data) {
            return -6;
        }
        C->rx_offset = 0;
    }

    /* Çalışma bayrak/sayaçları (bulk akış) */
    C->bulk_active      = 1;  /* host akış gönderecek */
    C->bulk_finished    = 0;  /* henüz bitiş sinyali yok */
    C->tx_total_recv    = 0;
    C->tx_sent_in_round = 0;

    return 0;
}

/* CSPI konfigine ait tahsisleri bırakır ve alanları temizler (idempotent) */
void free_cspi(t_cspi_fields *C)
{
    if (!C) return;
    if (C->tx_rb)   { free(C->tx_rb);   C->tx_rb = NULL; }
    if (C->rx_data) { free(C->rx_data); C->rx_data = NULL; }
    memset(C, 0, sizeof(*C));
}
