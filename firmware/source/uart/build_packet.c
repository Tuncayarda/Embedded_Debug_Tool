/*
 * build_packet.c
 *
 *  Created on: Sep 15, 2025
 *      Author: tuncayardaaydin
 *
 *  Amaç:
 *  -----
 *  - Özel UART protokolüne göre çerçeveli paket üretmek.
 *  - SOF (başlangıç), mesaj ID, payload uzunluğu, payload ve CRC16 ekler.
 *
 *  Protokol çerçevesi:
 *      [0]   SOF0 (0xAA)
 *      [1]   SOF1 (0x55)
 *      [2]   MSG_ID
 *      [3]   LEN_H  (payload uzunluğu, üst byte)
 *      [4]   LEN_L  (payload uzunluğu, alt byte)
 *      [5..] PAYLOAD (LEN bayt, opsiyonel)
 *      [N]   CRC_H  (CRC16, MSG_ID + LEN_H + LEN_L + PAYLOAD üzerinden)
 *      [N+1] CRC_L
 */

#include "uart_proto.h"

/**
 * @brief Verilen buffer içine protokol paketini kurar (çerçeve + CRC dahil).
 *
 * @param msgId        Mesaj kimliği (uygulama tanımlı).
 * @param payload      Payload verisi (payload yoksa NULL olabilir).
 * @param payload_len  Payload uzunluğu (bayt).
 * @param out          Çıkış buffer’ı (paketin tamamı buraya yazılır; yeterli büyük olmalı).
 *
 * @return out içine yazılan toplam bayt sayısı (tam paket uzunluğu).
 */
size_t build_packet(uint8_t msgId,
                    const uint8_t *payload, uint16_t payload_len,
                    uint8_t *out)
{
    size_t i = 0;

    /* --- SOF: çerçeve başı imzası --- */
    out[i++] = SOF0;   // 0xAA
    out[i++] = SOF1;   // 0x55

    const size_t core_start = i; // "çekirdek" alanların başladığı index (MSG_ID)

    /* --- Çekirdek başlık: MSG_ID + LEN --- */
    out[i++] = msgId;  // Mesaj ID

    const uint8_t len_hi = (uint8_t)((payload_len >> 8) & 0xFF);
    const uint8_t len_lo = (uint8_t)(payload_len & 0xFF);
    out[i++] = len_hi; // Payload uzunluğu (üst byte)
    out[i++] = len_lo; // Payload uzunluğu (alt byte)

    /* --- Payload kopyası --- */
    if (payload && payload_len > 0) {
        for (uint16_t k = 0; k < payload_len; ++k) {
            out[i++] = payload[k]; // Payload baytlarını araya koy
        }
    }

    /* --- CRC16 hesabı (çekirdek alanlar üzerinden) --- */
    // Not: CRC, MSG_ID + LEN_H + LEN_L + PAYLOAD sırasıyla hesaplanır.
    uint16_t crc = 0xFFFF;
    crc = crc16_step(crc, msgId);
    crc = crc16_step(crc, len_hi);
    crc = crc16_step(crc, len_lo);
    for (uint16_t k = 0; k < payload_len; ++k) {
        // Payload, core_start + 3 (MSG_ID + LEN_H + LEN_L) sonrasından başlar
        crc = crc16_step(crc, out[core_start + 3 + k]);
    }

    /* --- CRC16 ekle (büyük endian) --- */
    out[i++] = (uint8_t)((crc >> 8) & 0xFF); // CRC üst byte
    out[i++] = (uint8_t)(crc & 0xFF);        // CRC alt byte

    return i; // Toplam paket uzunluğu
}
