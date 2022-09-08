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
#include "ht32_usbd_core.h"
#include "ht32_usbd_class.h"
#include "ht32_usbd_descriptor.h"
#include "mouse_data.h"
#include <string.h>

/* Private constants ---------------------------------------------------------------------------------------*/

/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/

/* USB HID Report Descriptor                                                                                */

/* Private variables ---------------------------------------------------------------------------------------*/
__ALIGN4 static USBDCore_TypeDef usb_core;
static USBD_Driver_TypeDef usb_drv;
static u32 usb_is_low_power_enabled = TRUE;

/* Private function prototypes -----------------------------------------------------------------------------*/
static void usb_suspend(u32 uPara);
static void usb_config_pll(void);
static void usb_init_vreg(void);

/* Global Function -----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Configure USB.
  * @retval None
  ***********************************************************************************************************/
void usb_init(void)
{
  #if (LIBCFG_CKCU_USB_PLL)
  usb_config_pll();
  #endif

  #if (LIBCFG_PWRCU_VREG)
  usb_init_vreg();                               /* Voltage of USB setting                           */
  #endif

  usb_core.pDriver = (u32 *)&usb_drv;                /* Initiate memory pointer of USB driver            */
  usb_core.Power.CallBack_Suspend.func  = usb_suspend;      /* Install suspend call back function into USB core */
  //usb_core.Power.CallBack_Suspend.uPara = (u32)NULL;

  USBDDesc_Init(&usb_core.Device.Desc);                 /* Initiate memory pointer of descriptor            */
  USBDClass_Init(&usb_core.Class);                      /* Initiate USB Class layer                         */
  USBDCore_Init(&usb_core);                             /* Initiate USB Core layer                          */

  /* !!! NOTICE !!!
     Must turn on if the USB clock source is from HSI (PLL clock Source)
  */
  #if 0
  { /* Turn on HSI auto trim function                                                                       */
    CKCU_HSIAutoTrimClkConfig(CKCU_ATC_USB);
    CKCU_HSIAutoTrimCmd(ENABLE);
  }
  #endif

  NVIC_EnableIRQ(USB_IRQn);                             /* Enable USB device interrupt                      */
}

/*********************************************************************************************************//**
  * @brief  USB Connect.
  * @retval None
  ***********************************************************************************************************/
void usb_connect(void)
{
  USBD_DPpullupCmd(ENABLE);
}

/*********************************************************************************************************//**
  * @brief  USB Disconnect.
  * @retval None
  ***********************************************************************************************************/
void usb_disconnect(void)
{
  USBD_DPpullupCmd(DISABLE);
}

/*********************************************************************************************************//**
 * @brief   This function handles USB interrupt.
 * @retval  None
 ************************************************************************************************************/
void USB_IRQHandler(void)
{
  USBDCore_IRQHandler(&usb_core);
}

/*********************************************************************************************************//**
  * @brief  Handle the usb core, hid report
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void usb_svc(void)
{
  USBDCore_MainRoutine(&usb_core);  /* USB core main routine                                              */

  /* TODO: combine and send hid report                                                                    */
}

/*********************************************************************************************************//**
  * @brief  Send HID report.
  * @param  pData   Data to be sent
  * @param  length  Leng of data to be sent
  * @retval None
  ***********************************************************************************************************/
void usb_hid_build_and_send_report(void)
{
  if(mouse_status_changed != 0) {
    mouse_hid_data.report_id = HID_REPORT_MOUSE_ID;
    USBDCore_EPTWriteINData(USBD_EPT1, (u32 *)&mouse_hid_data, sizeof(mouse_hid_data_t));
    memset(&mouse_hid_data, 0, sizeof(mouse_hid_data_t));
    mouse_status_changed = 0;
  }
}

/*********************************************************************************************************//**
  * @brief  Send HID report.
  * @param  pData   Data to be sent
  * @param  length  Leng of data to be sent
  * @retval None
  ***********************************************************************************************************/
void usb_hid_send_key(u8 modifier, u8 keycode)
{
  kb_hid_data_t kb_data = {0};
  s32 i = 0;

  for (i = 5; i >=0; i--)
  {
    kb_data.keycode[i] = 0;
  }

  kb_data.report_id = HID_REPORT_KEYBOARD_ID;
  kb_data.keycode[0] = keycode;

  USBDCore_EPTWriteINData(USBD_EPT2, (u32 *)&kb_data, sizeof(kb_data));

  Time_Delay(TIME_MS2TICK(50));

  for (i = 5; i >=0; i--)
  {
    kb_data.keycode[i] = 0;
  }

  USBDCore_EPTWriteINData(USBD_EPT2, (u32 *)&kb_data, sizeof(kb_data));

  Time_Delay(TIME_MS2TICK(50));

}

/* Private Function -----------------------------------------------------------------------------------------*/

#if (LIBCFG_CKCU_USB_PLL)
/*********************************************************************************************************//**
 * @brief  Configure USB PLL
 * @retval None
 ************************************************************************************************************/
static void usb_config_pll(void)
{
  { /* USB PLL configuration                                                                                */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    CKCU_PLLInitTypeDef PLLInit;

    PLLInit.ClockSource = CKCU_PLLSRC_HSE;  // CKCU_PLLSRC_HSE or CKCU_PLLSRC_HSI
    #if (LIBCFG_CKCU_USB_PLL_96M)
    PLLInit.CFG = CKCU_USBPLL_8M_96M;
    #else
    PLLInit.CFG = CKCU_USBPLL_8M_48M;
    #endif
    PLLInit.BYPASSCmd = DISABLE;
    CKCU_USBPLLInit(&PLLInit);
  }

  CKCU_USBPLLCmd(ENABLE);

  while (CKCU_GetClockReadyStatus(CKCU_FLAG_USBPLLRDY) == RESET);
  CKCU_USBClockConfig(CKCU_CKUSBPLL);
}
#endif

#if (LIBCFG_PWRCU_VREG)
/*********************************************************************************************************//**
 * @brief  Configure USB Voltage
 * @retval None
 ************************************************************************************************************/
static void usb_init_vreg(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  PWRCU_SetVREG(PWRCU_VREG_3V3);

  /* !!! NOTICE !!!
     USB LDO should be enabled (PWRCU_VREG_ENABLE) if the MCU VDD > 3.6 V.
  */
  PWRCU_VREGConfig(PWRCU_VREG_BYPASS);
}
#endif

#define REMOTE_WAKEUP      (0)
/*********************************************************************************************************//**
  * @brief  Suspend call back function which enter DeepSleep1
  * @param  uPara: Parameter for Call back function
  * @retval None
  ***********************************************************************************************************/
static void usb_suspend(u32 uPara)
{
  #if (REMOTE_WAKEUP == 1)
  u32 IsRemoteWakeupAllowed;
  #endif

  if (usb_is_low_power_enabled)
  {

    #if (REMOTE_WAKEUP == 1)
    /* Disable EXTI interrupt to prevent interrupt occurred after wakeup                                    */
    EXTI_IntConfig(KEY1_BUTTON_EXTI_CHANNEL, DISABLE);
    IsRemoteWakeupAllowed = USBDCore_GetRemoteWakeUpFeature(&usb_core);

    if (IsRemoteWakeupAllowed == TRUE)
    {
      /* Enable EXTI wake event and clear wakeup flag                                                       */
      EXTI_WakeupEventConfig(KEY1_BUTTON_EXTI_CHANNEL, EXTI_WAKEUP_LOW_LEVEL, ENABLE);
      EXTI_ClearWakeupFlag(KEY1_BUTTON_EXTI_CHANNEL);
    }
    #endif

    __DBG_USBPrintf("%06ld >DEEPSLEEP\r\n", ++__DBG_USBCount);

    // Add your procedure here which disable related IO to reduce power consumption
    // ..................
    //

    /* For Bus powered device, you must enter DeepSleep1 when device has been suspended. For self-powered   */
    /* device, you may decide to enter DeepSleep1 or not depended on your application.                      */

    /* For the convenient during debugging and evaluation stage, the USBDCore_LowPower() is map to a null   */
    /* function by default. In the real product, you must map this function to the low power function of    */
    /* firmware library by setting USBDCORE_ENABLE_LOW_POWER as 1 (in the ht32fxxxx_usbdconf.h file).       */
    USBDCore_LowPower();

    // Add your procedure here which recovery related IO for application
    // ..................
    //

    __DBG_USBPrintf("%06ld <DEEPSLEEP\r\n", ++__DBG_USBCount);

    #if (REMOTE_WAKEUP == 1)
    if (EXTI_GetWakeupFlagStatus(KEY1_BUTTON_EXTI_CHANNEL) == SET)
    {
      __DBG_USBPrintf("%06ld WAKEUP\r\n", ++__DBG_USBCount);
      if (IsRemoteWakeupAllowed == TRUE && USBDCore_IsSuspend(&usb_core) == TRUE)
      {
        USBDCore_TriggerRemoteWakeup();
      }
    }

    if (IsRemoteWakeupAllowed == TRUE)
    {
      /* Disable EXTI wake event and clear wakeup flag                                                      */
      EXTI_WakeupEventConfig(KEY1_BUTTON_EXTI_CHANNEL, EXTI_WAKEUP_LOW_LEVEL, DISABLE);
      EXTI_ClearWakeupFlag(KEY1_BUTTON_EXTI_CHANNEL);
    }

    /* Clear EXTI edge flag and enable EXTI interrupt                                                       */
    EXTI_ClearEdgeFlag(KEY1_BUTTON_EXTI_CHANNEL);
    EXTI_IntConfig(KEY1_BUTTON_EXTI_CHANNEL, ENABLE);
    #endif
  }

  return;
}
