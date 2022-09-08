/*******************************************************************************
 *  @FILE NAME:    eeprom.c
 *  @DESCRIPTION:  flash based eeprom implementation
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

/****************************** Include Files *********************************/
#include "ht32.h"
#include "eeprom.h"

/*************************** Constant Definitions *****************************/

/***************************** Type Definitions *******************************/

/****************** Macros (Inline Functions) Definitions *********************/

/*********************** Internal Function Prototypes *************************/

/*************************** Variable Definitions *****************************/
static uint8_t _eeprom_buffer[EEPROM_LENGTH];

/********************** Exported Function Definitions *************************/
/***************************************************************************//**
  * @brief  Function init emulated eeprom (flash)
  * @retval none
  *****************************************************************************/
uint8_t eeprom_init(void)
{
  uint16_t i;
  uint16_t length = EEPROM_LENGTH >> 2;
  uint32_t *ptr = (uint32_t*) _eeprom_buffer;

  for (i = 0; i < length; i++)
  {
    *ptr++ = rw(EEPROM_START_ADDR + (i << 2));
  }
  return 0;
}

/***************************************************************************//**
  * @brief  Function reads a byte from emulated eeprom (flash)
  * @param  addr : Address to read
  * @retval data : Data read from eeprom
  *****************************************************************************/
uint8_t eeprom_read(const uint32_t addr)
{
  uint8_t data = 0;
  if (addr < EEPROM_LENGTH)
  {
    data = _eeprom_buffer[addr];
  }
  return data;
}

/***************************************************************************//**
  * @brief  Function writes a byte to emulated eeprom (flash)
  * @param  addr : Address to write
  * @param  data : value to write
  * @retval none
  *****************************************************************************/
void eeprom_write(uint32_t addr, uint8_t data)
{
  uint16_t i;
  uint16_t curr_len = addr >> 2;
  uint16_t len = EEPROM_LENGTH >> 2;
  uint8_t  buff_data = _eeprom_buffer[addr];
  uint32_t *ptr = (uint32_t*) _eeprom_buffer;
  FLASH_State FLASHState;

  if (addr < EEPROM_LENGTH)
  {
    if (buff_data == data)
    {
      return;
    }

    //Update SRAM Buffer Data
    _eeprom_buffer[addr] = data;

    if (buff_data == 0xFF)
    {
      //Write 4 Byte
      uint32_t *ptr_wr = (uint32_t*)((uint32_t)ptr + (curr_len << 2));
      FLASHState = FLASH_ProgramWordData((uint32_t)(EEPROM_START_ADDR + (curr_len << 2)), *ptr_wr);

      if (FLASHState != FLASH_COMPLETE)
      {
      #if 0  // For Debug
        while(1);
      #endif
      }
    }
    else
    {
      //Clear Flash Page
      FLASHState = FLASH_ErasePage(EEPROM_START_ADDR);

      //Write Page Flash
      for (i = 0; i < len; i++)
      {
        uint32_t *ptr_wr = (uint32_t*)((uint32_t)ptr + (i << 2));
        FLASHState = FLASH_ProgramWordData((uint32_t)(EEPROM_START_ADDR + (i << 2)), *ptr_wr);

        if (FLASHState != FLASH_COMPLETE)
        {
        #if 0  // For Debug
          while(1);
        #endif
        }
      }
    }
  }
}


/***************************************************************************//**
  * @brief  Function read eeprom data block
  * @param  buf:      buffer to store read data
  * @param  addr :    address to read
  * @param  len :     number of byte to read
  * @retval none
  *****************************************************************************/
void eeprom_read_block(void *buf, uint32_t addr, uint32_t len)
{
    uint8_t *      dest = (uint8_t *)buf;
    while (len--) {
        *dest++ = eeprom_read(addr++);
    }
}

/***************************************************************************//**
  * @brief  Function write eeprom data block
  * @param  buf:      data to be written
  * @param  addr :    address to write
  * @param  len :     number of byte to write
  * @retval none
  *****************************************************************************/
void eeprom_write_block(const void *buf, uint32_t addr, uint32_t len)
{
    const uint8_t *src = (const uint8_t *)buf;
    while (len--) {
        eeprom_write(addr++, *src++);
    }
}

/********************** Internal Function Definitions *************************/

/******************************************************************************/


