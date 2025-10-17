#include <string.h>
#include "uart_proto.h"
#include "uart.h"

typedef enum {
    RX_WAIT_SOF0,  // 0xAA beklenir; bulmadan ilerlenmez (resync için sağlam nokta)
    RX_WAIT_SOF1,  // 0x55 beklenir; değilse reset ve yeniden ara
    RX_WAIT_MSG,   // MSG_ID’yi al; CRC’ye dahil etmeye bu adımda başlıyoruz
    RX_WAIT_LEN_H, // LEN high (BE)
    RX_WAIT_LEN_L, // LEN low  (BE)
    RX_PAYLOAD,    // LEN kadar payload topla (ara CRC güncellenir)
    RX_CRC_H,      // CRC high (son doğrulama öncesi latched)
    RX_CRC_L       // CRC low  → karşılaştır, başarılıysa yayına al
} rx_state_t;


static struct {
    rx_state_t st;          // FSM state
    uint8_t    msg;         // MSG_ID (frame kodlayıcıyla hizalı)
    uint16_t   len;         // Beklenen payload uzunluğu (sınır kontrolü yapılır)
    uint16_t   got;         // O ana kadar toplanan payload byte sayısı
    uint16_t   crc;         // CCITT-FALSE akümülatörü (0xFFFF başlangıç)
    uint8_t    crc_hi;      // Alınan CRC’nin üst byte’ı (bir sonraki adımda birleştirilecek)
    uint8_t    pl[MAX_PAYLOAD]; // Payload scratch (tamamlanınca üst katmana kopyalanır)
} rx;

void proto_rx_reset(void)
{
    rx.st  = RX_WAIT_SOF0;
    rx.len = 0;
    rx.got = 0;
    rx.crc = 0xFFFF;
    rx.crc_hi = 0;
}

static inline void crc16_feed(uint8_t b){ rx.crc = crc16_step(rx.crc, b); }


//Uart rx ring bufferından paket toplar.
int proto_rx_poll(uint8_t *out_msg, uint8_t *out_pl, uint16_t *out_len)
{
    uint8_t b;

    while (uart0_try_read_byte(&b) == 1)
    {
        switch (rx.st)
        {
        case RX_WAIT_SOF0:
            /* AA gelmeden devam etmiyoruz → bit kaymaları, çöp karakterlerde doğal filtre. */
            if (b == SOF0) rx.st = RX_WAIT_SOF1;
            break;

        case RX_WAIT_SOF1:
            /* AA’dan hemen sonra 55 beklenir; değilse tüm bağlamı temizle ve yeniden ara. */
            if (b == SOF1) { rx.st = RX_WAIT_MSG; rx.crc = 0xFFFF; }
            else           { proto_rx_reset(); }
            break;

        case RX_WAIT_MSG:
            /* MSG_ID’yi yayınlamadan önce CRC’ye kat; üst katman mesaj ayrımı bu ID’ye bakar. */
            rx.msg = b; crc16_feed(b);
            rx.st  = RX_WAIT_LEN_H;
            break;

        case RX_WAIT_LEN_H:
            /* Big-endian uzunluk; sınır kontrolü LEN_L’den sonra yapılır. */
            rx.len = ((uint16_t)b) << 8; crc16_feed(b);
            rx.st  = RX_WAIT_LEN_L;
            break;

        case RX_WAIT_LEN_L:
            /* LEN tamamlandı; önce CRC’ye kat, sonra bütçe kontrolü → oversize ise reset. */
            rx.len |= b; crc16_feed(b);
            if (rx.len > MAX_PAYLOAD) { proto_rx_reset(); break; }
            rx.got = 0;
            /* 0-length payload desteklenir (sadece başlık+CRC); bir sonraki state CRC_H. */
            rx.st  = (rx.len == 0) ? RX_CRC_H : RX_PAYLOAD;
            break;

        case RX_PAYLOAD:
            /* Payload’ı doğrudan scratch buffera al; aynı anda CRC’yi yürüterek tek geçiş. */
            rx.pl[rx.got++] = b; crc16_feed(b);
            if (rx.got == rx.len) rx.st = RX_CRC_H;  // tamamlanınca CRC’ye geç
            break;

        case RX_CRC_H:
            /* CRC üst byte’ını park et; bir sonraki byte ile birleştirip kontrol edeceğiz. */
            rx.crc_hi = b;
            rx.st     = RX_CRC_L;
            break;

        case RX_CRC_L: {
            /* Alınan CRC (rx_crc) ile hesaplanan (rx.crc) birebir aynı olmalı. */
            uint16_t rx_crc = ((uint16_t)rx.crc_hi << 8) | b;
            if (rx_crc == rx.crc) {
                /* Başarılı çerçeve: isteğe bağlı OUT param’lara yayınla ve temiz başlangıç. */
                if (out_msg) *out_msg = rx.msg;
                if (out_len) *out_len = rx.len;
                if (out_pl && rx.len) memcpy(out_pl, rx.pl, rx.len);
                proto_rx_reset();
                return 1;               // üst katmana “tamam” sinyali
            } else {
                /* CRC uyuşmazlığı: agresif resync; en baştan yeni SOF ara. */
                proto_rx_reset();
            }
            break;
        }
        }
    }

    /* Bu turda tam frame üretemedik; bir dahaki poll’da devam. */
    return 0;
}
