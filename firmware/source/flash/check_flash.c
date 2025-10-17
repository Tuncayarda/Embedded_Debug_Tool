/*
 *  Amaç: Kullanıcı flash’ındaki çerçeveyi doğrula, geçerliyse aksiyonları çalıştır.
 *  Beklenen flash formatı (UART çerçevesiyle aynı):
 *    [0]=0xAA, [1]=0x55, [2]=MSG_ID, [3]=LEN_H, [4]=LEN_L, [5..]=PAYLOAD, [5+len]=CRC_H, [6+len]=CRC_L
 */

#include "flash.h"
#include "uart/uart.h"
#include "uart/uart_proto.h"
#include "execute/execute.h"
#include "pit/pit.h"
#include "debug.h"

/**
 * @brief Flash’teki çerçeveyi doğrular, boot işaretliyse parse edip çalıştırır.
 * @param flash_cache Okunan çerçeve için RAM tamponu (>= MAX_PAYLOAD + PROTO_CORE_SIZE).
 * @param length şimdilik kullanılmıyor.
 */
void check_flash(uint8_t *flash_cache, uint32_t length)
{
    (void)length; // Şimdilik kullanılmıyor.

    uart0_print("Checking flash...\r\n");  // Durum bilgisi

    // Flash’tan desteklenen en büyük çerçeve boyutunu oku (offset 0’dan tek çerçeve varsayımı).
    flash_read(0, flash_cache, MAX_PAYLOAD + PROTO_CORE_SIZE);

    // Preambl 0xAA55 mi?
    if (flash_cache[0] == 0xAA && flash_cache[1] == 0x55)
    {
        // MSG ID byte 2’de
        if (flash_cache[2] == MSG_ID_WRITE_FLASH_BOOT)  // Boot edilebilir çerçeve mi?
        {
            // PAYLOAD uzunluğu (BE16)
            uint16_t len = ((uint16_t)flash_cache[3] << 8) | flash_cache[4];

            // Çekirdek alan uzunluğu: ID(1) + LEN(2) + PAYLOAD(len)
            size_t core_len  = 1u + 2u + (size_t)len;

            // CRC’yi çekirdek alan üzerinden yeniden hesapla (başlangıç: index 2)
            uint16_t crc = 0xFFFF;
            const uint8_t *core = &flash_cache[2];
            for (size_t i = 0; i < core_len; ++i) {
                crc = crc16_step(crc, core[i]);
            }

            // Alınan CRC çekirdekten hemen sonra
            uint16_t rx_crc = ((uint16_t)flash_cache[2 + core_len] << 8)
                            |  (uint16_t)flash_cache[2 + core_len + 1];

            if (crc != rx_crc) {
                uart0_print("Flash CRC check failed\r\n");  // CRC hatası
            } else {
                uart0_print("Executing actions stored in flash...\r\n"); // Geçerli: çalıştır

                t_action_set set;
                // Aksiyon payload’ı byte 5’ten başlar (AA 55 ID LEN sonrası)
                int n = parse_actions(flash_cache + 5, len, &set);

                if (n > 0) {
                    //dump_action_set(&set);   // İsteğe bağlı debug çıktısı
                    execute(&set);
                    uart0_print("Execution from flash completed\r\n");
                } else {
                    uart0_print("Flash parse error ");
                    uart0_print_i32(n);
                    uart0_print("\r\n");
                }
            }
        }
        else if (flash_cache[2] == MSG_ID_WRITE_FLASH) {
            uart0_print("Flash contains a frame, but not marked as bootable\r\n"); // Boot bayraksız
        }
        else {
            uart0_print("Flash has unknown message ID\r\n"); // Tanımsız mesaj ID
        }
    }
    else {
        uart0_print("No valid data in flash\r\n"); // Geçerli preambl yok
    }
}
