/*
 *  Kullanıcı alanı flash işlemleri (init/erase/program/read).
 */

#include "uart/uart.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "flash.h"

/* NXP SDK flash sürücüsü konfigürasyon yapısı. */
static flash_config_t s_flash;


status_t flash_init(void)
{
    status_t st = FLASH_Init(&s_flash);
    return st;
}

status_t flash_erase(void)
{
    status_t st;
    st = FLASH_Erase(&s_flash, USER_FLASH_BASE, USER_FLASH_SIZE, kFLASH_ApiEraseKey);
    return st;
}

/**
 * @brief Flash’a veri yazar (gerekirse hizasız baş/sonu tamponlayarak).
 * @param offset USER_FLASH_BASE’ten uzaklık (henüz implemente edilmedi).
 * @param data   Kaynak RAM tamponu.
 * @param length Yazılacak byte sayısı.
 * @return Başarılıysa kStatus_Success, aksi halde hata kodu.
 */
status_t flash_program(uint32_t offset, const void *data, uint32_t length)
{
    if (!data && length)                   return kStatus_InvalidArgument;   // Geçersiz argüman
    if (length > USER_FLASH_SIZE) 		   return kStatus_OutOfRange;       // Bölge dışı erişim

    const uint8_t *src = (const uint8_t*)data;
    uint32_t addr = USER_FLASH_BASE;

    if (length == 0) return kStatus_Success;                                // Yazacak bir şey yok

    /* Tam hizalı phrase’leri direkt yaz */
    while (length >= FLASH_PHRASE_SIZE) {
        status_t st = FLASH_Program(&s_flash, addr, (uint8_t*)src, FLASH_PHRASE_SIZE);
        if (st != kStatus_Success) return st;
        addr   += FLASH_PHRASE_SIZE;
        src    += FLASH_PHRASE_SIZE;
        length -= FLASH_PHRASE_SIZE;
    }

    /* Hizasız kuyruk: kalan baytları phrase’e alıp yaz */
    if (length) {
        uint8_t phrase[FLASH_PHRASE_SIZE];
        memset(phrase, 0xFF, sizeof(phrase));
        memcpy(phrase, src, length);

        status_t st = FLASH_Program(&s_flash, addr, phrase, FLASH_PHRASE_SIZE);
        if (st != kStatus_Success) return st;
    }

    return kStatus_Success;
}

/**
 * @brief Kullanıcı flash bölgesinden veri okur (basit memcpy).
 * @param offset USER_FLASH_BASE’ten uzaklık.
 * @param dst    Hedef RAM tamponu.
 * @param length Okunacak byte sayısı.
 * @return Başarılıysa kStatus_Success, aksi halde hata kodu.
 */
status_t flash_read(uint32_t offset, void *dst, uint32_t length)
{
    if (!dst && length)                    return kStatus_InvalidArgument;  // Geçersiz argüman
    if (offset + length > USER_FLASH_SIZE) return kStatus_OutOfRange;      // Bölge dışı erişim

    memcpy(dst, (const void*)(USER_FLASH_BASE + offset), length);          // Doğrudan kopyala
    return kStatus_Success;
}
