/*********************************************************************************************************//**
 * @file    USBD/HID_Mouse/ht32_usbd_class.c
 * @version $Rev:: 4874         $
 * @date    $Date:: 2020-08-05 #$
 * @brief   The USB Device Class.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "mouse_data.h"

/* Private constants ---------------------------------------------------------------------------------------*/

/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/
__ALIGN4 mouse_hid_data_t mouse_hid_data;
__ALIGN4 kb_hid_data_t kb_hid_data;
uint8_t mouse_status_changed = 0;
int32_t exti_cnt = 0;

uint16_t dpi[MAX_DPI_INDEX] = {1000, 1600, 2400, 3200, 4000, 8000, 10000, 12000};
uint8_t curr_dpi_idx_xy = 2;  // default: 24000 dpi
uint8_t curr_dpi_idx_x = 2;  // default: 24000 dpi
RGB dpi_backlight_color[MAX_DPI_INDEX] = {
  {0, 128, 0},    // RED
  {128, 128, 0},  // yellow
  {128, 0, 0},    // green
  {128, 0, 64},  // spring green
  {64, 0, 128},  // Azure
  {0, 0, 128},    // blue
  {0, 128, 128},  // magenta
  {128, 128, 128} // white
};


uint16_t polling_rate[MAX_POLLING_RATE_INDEX] = {1000, 500, 250};
uint8_t curr_pool_idx = 1;  // default: 500Hz


/* USB HID Report Descriptor                                                                                */

/* Private variables ---------------------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------------------------------------*/

/* Global Function -----------------------------------------------------------------------------------------*/
