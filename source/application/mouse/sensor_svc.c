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
#include "pmw3360.h"
#include "mouse_data.h"

/* Private constants ---------------------------------------------------------------------------------------*/

/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/

/* USB HID Report Descriptor                                                                                */

/* Private variables ---------------------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------------------------------------*/

/* Global Function -----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Mouse sensor initialization
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void sensor_init(void)
{
  drv_mouse_sensor_init();
}

/*********************************************************************************************************//**
  * @brief  Handle sensor pooling
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void sensor_svc(void)
{
  static uint32_t sensor_pooling_tick = 0;
  uint32_t sys_tick = Time_GetTick();
  static uint8_t prev_button_state = 0;
  static int32_t prev_enc_cnt = 0;
  int16_t dx = 0;
  int16_t dy = 0;
  uint8_t motion = 0;

  /* Pooling */
  if( sys_tick - sensor_pooling_tick > TIME_MS2TICK(1000/(polling_rate[curr_pool_idx]))) {
    sensor_pooling_tick = sys_tick;

    /* Sensor pooling */
    motion = drv_mouse_sensor_read(&dx, &dy);
    mouse_hid_data.x = dx;
    mouse_hid_data.y = dy;

    /* Button pooling */
    mouse_hid_data.button.bits.BIT0 = !board_button_read(BTN_LEFT);
    mouse_hid_data.button.bits.BIT1 = !board_button_read(BTN_RIGHT);
    mouse_hid_data.button.bits.BIT2 = !board_button_read(BTN_MID);
    mouse_hid_data.button.bits.BIT3 = !board_button_read(BTN_BACK);
    mouse_hid_data.button.bits.BIT4 = !board_button_read(BTN_FWD);

    /* Wheel (encoder) */
    mouse_hid_data.wheel = (int8_t)(exti_cnt - prev_enc_cnt);

    /* Validate data changed */
    if( (motion != 0) || (prev_button_state != mouse_hid_data.button.byte) || (mouse_hid_data.wheel != 0) )
    {
      mouse_status_changed = 1;
    }

    /* Save status */
    prev_button_state = mouse_hid_data.button.byte;
    prev_enc_cnt = exti_cnt;
  }
}
