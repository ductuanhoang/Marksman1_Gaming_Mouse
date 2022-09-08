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
#include "is31fl3246.h"
#include "mouse_data.h"
#include "led_svc.h"
#include "rgb_matrix.h"

/* Private constants ---------------------------------------------------------------------------------------*/

/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/

/* USB HID Report Descriptor                                                                                */

/* Private variables ---------------------------------------------------------------------------------------*/
// static rgb_color_t backlight_color[MAX_BACKLIGHT_COLOR] = {
//   {128, 0, 0},    // RED
//   {128, 128, 0},  // yellow
//   {0, 128, 0},    // green
//   {0, 128, 64},  // spring green
//   {0, 64, 128},  // Azure
//   {0, 0, 128},    // blue
//   {128, 0, 128},  // magenta
//   {128, 128, 128} // white
// };

/* Private function prototypes -----------------------------------------------------------------------------*/

/* Global Function -----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  LED initialization
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void led_init(void)
{
  rgb_matrix_init();
  // is31fl3246_init();
  is31fl3246_set_led_all(100, 0, 0);
  is31fl3246_commit();
}

/*********************************************************************************************************//**
  * @brief  Handle led pooling
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void led_svc(void)
{
  static uint32_t led_pooling_tick = 0;
  uint32_t sys_tick = Time_GetTick();

  /* Pooling */
  if( sys_tick - led_pooling_tick > TIME_MS2TICK(10)) {
    led_pooling_tick = sys_tick;

    rgb_matrix_task();

  }


}
