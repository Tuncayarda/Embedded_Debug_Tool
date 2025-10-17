/*
 * action.h
 *  Action tipleri/verileri, CSPI alanları ve parser prototipleri.
 */

#ifndef ACTION_H_
#define ACTION_H_

#include <stdint.h>

#define MAX_ACTIONS   64   /* Tek sette desteklenen maksimum action sayısı */
#define MAX_TARGETS   16   /* Bir action’ın referans verebileceği maksimum target sayısı */

/* Action türleri (opkod benzeri) */
enum {
    TYPE_START       = 0x01, /* Senaryonun başlangıcı/ilk adımı işaretle */
    TYPE_DELAY       = 0x02, /* Zaman gecikmesi (ms/us/ticks) */
    TYPE_PIN_READ    = 0x03, /* GPIO pini oku ve (opsiyonel) seviyeyi doğrula */
    TYPE_PIN_WRITE   = 0x04, /* GPIO pinini hedef seviyeye sür */
    TYPE_PIN_TRIGGER = 0x05  /* Pin hedef seviyeye gelene kadar bekle (timeout’lu) */
};

/* GPIO için mantık seviyeleri */
enum {
    LVL_UNDEF = 0xFF, /* Başlangıçta bilinmiyor/kontrol etme */
    LVL_LOW   = 0x00, /* Düşük seviye */
    LVL_HIGH  = 0x01  /* Yüksek seviye */
};

/* Action yaşam döngüsü durumları */
enum {
    STATUS_IDLE,     /* Henüz planlanmadı/beklemede */
    STATUS_PENDING,  /* Planlandı, başlamayı bekliyor */
    STATUS_RUNNING,  /* Çalışıyor */
    STATUS_DONE,     /* Hatasız tamamlandı */
    STATUS_ERROR     /* Hata ile tamamlandı */
};

/* Action hata kodları */
enum {
    ERROR_NONE,           /* Hata yok */
    ERROR_INITIAL_LEVEL,  /* Başlangıç seviyesi bekleneni tutmadı */
    ERROR_NON_SPECIFIED   /* Sınıflandırılmamış/genel hata */
};

/* Gecikme alanları (ms/us/ticks) */
typedef struct
{
    uint32_t duration_ms;    /* Milisaniye cinsinden gecikme */
    uint16_t duration_us;    /* Ek mikrosaniye */
    uint32_t duration_ticks; /* Alternatif/önceden hesaplanmış tick sayısı */
} t_delay_fields;

/* Pin okuma alanları (gözlem ve opsiyonel seviye kontrolü) */
typedef struct
{
    uint8_t  port;           /* GPIO port indeksi (platforma özgü) */
    uint8_t  pin;            /* Port içindeki pin numarası */
    uint8_t  initial;        /* Beklenen başlangıç seviyesi (UNDEF: yok say) */
    uint8_t  target;         /* Ulaşılması/doğrulanması beklenen seviye */
    uint8_t  final;          /* İşlem sonrası beklenen seviye (UNDEF: yok say) */
    uint32_t duration_ms;    /* Gözlem penceresi ms */
    uint16_t duration_us;    /* Gözlem penceresi us */
    uint32_t duration_ticks; /* Gözlem penceresi ticks */
} t_pin_read_fields;

/* Pin yazma alanları (pini belli seviyede tutma) */
typedef struct
{
    uint8_t  port;           /* GPIO port indeksi */
    uint8_t  pin;            /* Pin numarası */
    uint8_t  initial;        /* Sürmeden önce beklenen seviye (UNDEF: atla) */
    uint8_t  target;         /* Sürülecek seviye (LOW/HIGH) */
    uint8_t  final;          /* Bitişte beklenen seviye (UNDEF: atla) */
    uint32_t duration_ms;    /* Tutma süresi ms (0: anlık ayar) */
    uint16_t duration_us;    /* Ek mikrosaniye */
    uint32_t duration_ticks; /* Ek ticks */
} t_pin_write_fields;

/* Pin tetikleme alanları (hedef seviyeyi bekle, timeout’lu) */
typedef struct
{
    uint8_t  port;           /* GPIO port indeksi */
    uint8_t  pin;            /* Pin numarası */
    uint8_t  initial;        /* Başlangıç seviyesi beklentisi (UNDEF: atla) */
    uint8_t  target;         /* Beklenen hedef seviye (LOW/HIGH) */
    uint32_t timeout_ms;     /* Maksimum bekleme süresi ms */
    uint16_t timeout_us;     /* Ek mikrosaniye */
    uint32_t duration_ticks; /* Timeout’un tick karşılığı */
} t_pin_trigger_fields;

/*
 * CSPI (SPI slave) işlem demeti ayar/durumları (tek “round”u yönetir):
 * - TX ring: slave verisini besler, RX opsiyoneldir (log/validasyon için).
 * - transfer_size penceresi (1..N): round sonunda son N byte’ı 32-bit değere toplayıp threshold ile kıyaslar.
 * - Eşik sonucuna göre belirtilen GPIO çıkışı sürülür (ISR tarafında yorumlanır).
 */
typedef struct
{
    /* Sabit SPI konfigürasyonu */
    uint8_t   mode;             /* SPI modu (0..3), CPOL/CPHA */
    uint8_t   word_size;        /* Çerçeve bit sayısı (bu sürüm 8 bit kullanır) */

    /* Round bazlı karşılaştırma parametreleri */
    uint8_t   transfer_size;    /* Round sonunda dikkate alınacak byte sayısı (maks 4’te kıstırılır) */
    uint32_t  threshold_val;    /* 32-bit karşılaştırma eşiği */

    /* Opsiyonel RX yakalama */
    uint16_t  rx_size;          /* RX buffer boyu (0: kapalı) */
    uint8_t  *rx_data;          /* RX buffer pointer’ı (sahiplik bu yapıda) */
    volatile uint16_t rx_offset;/* ISR tarafından ilerletilen yazma ofseti */

    /* GPIO bildirim çıkışı (eşik sonucunda sürülür) */
    uint8_t   port, pin;        /* Bildirim için GPIO port/pin */
    uint8_t   idle_fill;        /* TX ring boşsa çıkacak dolgu byte’ı */

    /* TX ring (slave besleme) */
    uint8_t  *tx_rb;            /* TX ring buffer */
    uint16_t  tx_rb_size;       /* TX ring kapasitesi */
    volatile uint16_t tx_rb_head;/* Üretici indeksi (yazıcı) */
    volatile uint16_t tx_rb_tail;/* Tüketici indeksi (ISR) */

    /* Akış kontrolü / ilerleme metrikleri */
    uint16_t  tx_low_wm;            /* Düşük su seviyesi (refill tetikleme eşiği) */
    volatile uint8_t  bulk_active;   /* Host sürekli veri akıtıyor mu? */
    volatile uint8_t  bulk_finished; /* Host veri bitti sinyali verdi mi? */
    volatile uint32_t tx_total_recv; /* Toplam kabul edilen TX byte (tanı) */

    volatile uint16_t tx_sent_in_round; /* Mevcut round’da gönderilen byte sayısı */
} t_cspi_fields;

/* Tek action kaydı (tipine göre union payload) */
typedef struct
{
    uint8_t  id;             /* Action ID (dizi indekslemesi için) */
    uint8_t  type;           /* TYPE_* */
    uint8_t  status;         /* STATUS_* */
    uint8_t  error;          /* ERROR_* */

    uint32_t start_tick;     /* Başlama tick’i */
    uint32_t deadline_tick;  /* Bitiş/timeout tick’i */

    uint8_t  target_count;   /* Hedef sayısı */
    uint8_t *targets;        /* Hedef ID dizisi, dinamik */

    union {
        t_delay_fields       delay;        /* TYPE_DELAY */
        t_pin_read_fields    pin_read;     /* TYPE_PIN_READ */
        t_pin_write_fields   pin_write;    /* TYPE_PIN_WRITE */
        t_pin_trigger_fields pin_trigger;  /* TYPE_PIN_TRIGGER */
        t_cspi_fields        cspi;         /* CSPI işlem demeti */
    } u; /* Tip özel yük */
} t_action_rec;

/* Action set (senaryo) */
typedef struct
{
    t_action_rec *actions;   /* Action dizisi (boy = count) */
    int           count;     /* Geçerli kayıt sayısı */
} t_action_set;

/* Ham payload’ı action set’e parse eder (actions alanını tahsis eder) */
int  parse_actions(const uint8_t *pl, uint16_t len, t_action_set *S);
/* Action set içindeki tahsisleri serbest bırakır (actions dahil) */
void free_actions(t_action_set *S);

/* Tek bir CSPI “begin” bloğunu t_cspi_fields’e parse eder */
int  parse_cspi_begin(const uint8_t *pl, uint16_t len, t_cspi_fields *C);
/* CSPI alanlarına ait tahsisleri temizler (ring/RX buffer vb.) */
void free_cspi(t_cspi_fields *C);

#endif /* ACTION_H_ */
