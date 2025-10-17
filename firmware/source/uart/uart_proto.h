/*
 * uart_proto.h
 *
 * UART çerçeveleme ve protokol tanımları.
 *
 * Çerçeve formatı (big-endian uzunluk, CRC-16/CCITT-FALSE):
 *   [0]  SOF0 = 0xAA
 *   [1]  SOF1 = 0x55
 *   [2]  MSG_ID
 *   [3]  LEN_H
 *   [4]  LEN_L
 *   [5..5+LEN-1]  PAYLOAD (LEN byte)
 *   [5+LEN]   CRC_H   (CRC16 → MSG_ID + LEN + PAYLOAD)
 *   [6+LEN]   CRC_L
 *
 * Yardımcılar:
 *  - crc16_step()  : CRC güncelleme
 *  - build_packet(): Çerçeve paket inşa etme
 *  - proto_rx_*()  : UART alıcı state machine yardımcıları
 */

#ifndef UART_PROTO_H_
#define UART_PROTO_H_

#include <stdint.h>

/* Frame başlangıcı için sabit değerler (senkronizasyon için peş peşe gelir) */
#define SOF0 0xAA
#define SOF1 0x55

/* Maksimum payload boyutu (byte cinsinden) */
#define MAX_PAYLOAD 512

/* Sabit çerçeve boyutu (SOF + MSG + LEN + CRC) */
#define PROTO_CORE_SIZE (2u /*SOF*/ + 1u /*MSG*/ + 2u /*LEN*/ + 2u /*CRC*/)

/* Mesaj ID’leri (uygulama seviyesinde anlam yüklenir) */
#define MSG_ID_EXECUTE_ACTIONS  0x10  /* Action blob hemen çalıştır */

#define MSG_ID_CSPI_BEGIN       0x50  /* SPI slave başlatma/config */
#define MSG_ID_CSPI_DATA        0x52  /* SPI slave veri chunk */
#define MSG_ID_CSPI_END         0x54  /* SPI oturumunu bitir */
#define MSG_ID_CSPI_REQ         0x59  /* Cihaz host’tan daha fazla veri ister */
#define MSG_ID_CSPI_TERMINATE   0x5B  /* SPI oturumunu iptal/abort */

#define MSG_ID_WRITE_FLASH      0x90  /* Flash’a yaz (boot değil) */
#define MSG_ID_WRITE_FLASH_BOOT 0x91  /* Flash’a yaz (bootable) */

#define MSG_ID_RESET            0xFF  /* Yazılımsal reset isteği */

#define MSG_ID_CLEAR_FLASH      0xCC  /* User flash bölgesini sil */

/* CRC hesaplama adımı */
uint16_t crc16_step(uint16_t crc, uint8_t byte);

/* RX state machine reset fonksiyonu */
void proto_rx_reset(void);

/* RX polling fonksiyonu */
int proto_rx_poll(uint8_t *out_msg, uint8_t *out_pl, uint16_t *out_len);

/* Paket oluşturma fonksiyonu */
size_t build_packet(uint8_t msgId,
                    const uint8_t *payload, uint16_t payload_len,
                    uint8_t *out);

#endif /* UART_PROTO_H_ */
