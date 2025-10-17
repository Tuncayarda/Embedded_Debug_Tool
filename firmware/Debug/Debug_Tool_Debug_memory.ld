/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright 2008-2013 Code Red Technologies Ltd,
 * Copyright 2013-2025 NXP
 * SPDX-License-Identifier: BSD-3-Clause
 * Generated linker script file for MK02FN128xxx10
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v25.6 [Build 136] [2025-06-27] on Oct 3, 2025, 12:49:19 PM
 */

MEMORY
{
  /* Define each memory region */
  PROGRAM_FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x10000 /* 64K bytes (alias Flash) */  
  USER_FLASH (rx) : ORIGIN = 0x10000, LENGTH = 0x10000 /* 64K bytes (alias Flash2) */  
  SRAM_LOWER (rwx) : ORIGIN = 0x1fffe000, LENGTH = 0x2000 /* 8K bytes (alias RAM) */  
  SRAM_UPPER (rwx) : ORIGIN = 0x20000000, LENGTH = 0x2000 /* 8K bytes (alias RAM2) */  
}

  /* Define a symbol for the top of each memory region */
  __base_PROGRAM_FLASH = 0x0  ; /* PROGRAM_FLASH */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_PROGRAM_FLASH = 0x0 + 0x10000 ; /* 64K bytes */  
  __top_Flash = 0x0 + 0x10000 ; /* 64K bytes */  
  __base_USER_FLASH = 0x10000  ; /* USER_FLASH */  
  __base_Flash2 = 0x10000 ; /* Flash2 */  
  __top_USER_FLASH = 0x10000 + 0x10000 ; /* 64K bytes */  
  __top_Flash2 = 0x10000 + 0x10000 ; /* 64K bytes */  
  __base_SRAM_LOWER = 0x1fffe000  ; /* SRAM_LOWER */  
  __base_RAM = 0x1fffe000 ; /* RAM */  
  __top_SRAM_LOWER = 0x1fffe000 + 0x2000 ; /* 8K bytes */  
  __top_RAM = 0x1fffe000 + 0x2000 ; /* 8K bytes */  
  __base_SRAM_UPPER = 0x20000000  ; /* SRAM_UPPER */  
  __base_RAM2 = 0x20000000 ; /* RAM2 */  
  __top_SRAM_UPPER = 0x20000000 + 0x2000 ; /* 8K bytes */  
  __top_RAM2 = 0x20000000 + 0x2000 ; /* 8K bytes */  
