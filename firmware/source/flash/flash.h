#ifndef FLASH_FLASH_H_
#define FLASH_FLASH_H_

#include "fsl_flash.h"

/* USER_FLASH bölgesinin başlangıç adresi . */
#define USER_FLASH_BASE   (0x00010000u)

/* USER_FLASH bölgesinin toplam boyutu (byte cinsinden). */
#define USER_FLASH_SIZE   (0x00010000u)

#define FLASH_PHRASE_SIZE 8u

/* Kullanıcı flashındaki veriyi doğrula ve varsa çalıştır. */
void check_flash(uint8_t *flash_cache, uint32_t length);

status_t flash_init(void);

status_t flash_program(uint32_t offset, const void *data, uint32_t length);

status_t flash_erase(void);

status_t flash_read(uint32_t offset, void *dst, uint32_t length);

#endif /* FLASH_FLASH_H_ */
