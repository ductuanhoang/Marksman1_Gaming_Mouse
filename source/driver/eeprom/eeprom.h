/*******************************************************************************
 *  @FILE NAME:    eeprom.h
 *  @DESCRIPTION:  header for EEPROM emulation
 *
 *  Copyright (c) 2022 BTM Technologies.
 *  All Rights Reserved This program is the confidential and proprietary
 *  product of BTM Technologies. Any Unauthorized use, reproduction or transfer
 *  of this program is strictly prohibited.
 *
 *  @Author: ManhBT
 *  @NOTE:   No Note at the moment
 *  @BUG:    No known bugs.
 *
 *<pre>
 *  MODIFICATION HISTORY:
 *
 *  Ver   Who       Date                Changes
 *  ----- --------- ------------------  ----------------------------------------
 *  1.00  ManhBT    Jan 12, 2022        First version
 *
 *
 *</pre>
*******************************************************************************/

#ifndef __EEPROM_H__
#define __EEPROM_H__

/****************************** Include Files *********************************/
#include "ht32.h"

/*************************** Constant Definitions *****************************/
#ifndef EEPROM_LENGTH
#define EEPROM_LENGTH         (EEPROM_PAGE_SIZE)
#endif

#ifndef EEPROM_START_ADDR
#define EEPROM_START_ADDR     (LIBCFG_FLASH_SIZE - EEPROM_PAGE_SIZE * 1)
#endif

#ifndef EEPROM_END_ADDR
#define EEPROM_END_ADDR       (EEPROM_START_ADDR + EEPROM_PAGE_SIZE)
#endif

#define EEPROM_PAGE_SIZE      (LIBCFG_FLASH_PAGESIZE)

/***************************** Type Definitions *******************************/

/****************** Macros (Inline Functions) Definitions *********************/

/*************************** Variable Definitions *****************************/

/******************************************************************************/

#ifndef ARDUINO
#ifdef __cplusplus
extern "C" {
#endif
#endif
/*************************** Function Prototypes ******************************/
uint8_t eeprom_init(void);
uint8_t eeprom_read(const uint32_t addr);
void eeprom_write(uint32_t addr, uint8_t data);
void eeprom_read_block(void *buf, uint32_t addr, uint32_t len);
void eeprom_write_block(const void *buf, uint32_t addr, uint32_t len);

/******************************************************************************/

#ifndef ARDUINO
#ifdef __cplusplus
}
#endif
#endif

#endif /* __EEPROM_H__ */
