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
#include "rgb_matrix.h"

/* Private constants ---------------------------------------------------------------------------------------*/
#define CLASS_DESC_OFFSET                   (DESC_LEN_CONFN + DESC_LEN_INF)
#define CLASS_DESC_OFFSET1                  (CLASS_DESC_OFFSET + DESC_LEN_HID + DESC_LEN_EPT + DESC_LEN_INF)
#define CLASS_DESC_OFFSET2                  (CLASS_DESC_OFFSET1 + DESC_LEN_HID + DESC_LEN_EPT + DESC_LEN_INF)

#define CLASS_REQ_01_GET_RPOT               (u16)(0x1 << 8)
#define CLASS_REQ_02_GET_IDLE               (u16)(0x2 << 8)
#define CLASS_REQ_03_GET_PTCO               (u16)(0x3 << 8)
#define CLASS_REQ_09_SET_RPOT               (u16)(0x9 << 8)
#define CLASS_REQ_0A_SET_IDLE               (u16)(0xA << 8)
#define CLASS_REQ_0B_SET_PTCO               (u16)(0xB << 8)

#define HID_RPOT_TYPE_01_INPUT              (0x01)
#define HID_RPOT_TYPE_02_OUTPUT             (0x02)
#define HID_RPOT_TYPE_03_FEATURE            (0x03)

/* Private types -------------------------------------------------------------------------------------------*/

/* Following define skip the address-of-packed-member warning of GUN compiler                               */
#if defined (__GNUC__)
#if (__GNUC__ >= 9)
  #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif
#endif

/* Global variables ----------------------------------------------------------------------------------------*/

/* USB HID Report Descriptor                                                                                */
__ALIGN4 static uc8 USB_HID_MouseReportDesc[] = {
    0x05, 0x01,                     // Usage Page (Generic Desktop)
    0x09, 0x02,                     // Usage (Mouse)
    0xA1, 0x01,                     // Collection (Application)

    0x85, HID_REPORT_MOUSE_ID,      //   REPORT_ID (1)

    // Mouse buttons
    0x09, 0x01,                     //     Usage (Pointer)
    0xA1, 0x00,                     //     Collection (Physical)
    0x05, 0x09,                     //         Usage Page (Buttons)
    0x19, 0x01,                     //             Usage Minimum (01)
    0x29, 0x05,                     //             Usage Maximum (05)
    0x15, 0x00,                     //             Logical Minimum (0)
    0x25, 0x01,                     //             Logical Maximum (1)
    0x75, 0x01,                     //             REPORT_SIZE (1)
    0x95, 0x05,                     //             REPORT_COUNT (5)
    0x81, 0x02,                     //             INPUT (Data, Var, Abs)
    /* Padding (3 bits) */
    0x95, 0x01,                     //             Report Count (1)
    0x75, 0x03,                     //             Report Size (3)
    0x81, 0x01,                     //             Input (Const, Variable, Absolute)

    /* Mouse data */
    0x05, 0x01,                     //         USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                     //             USAGE (X)
    0x09, 0x31,                     //             USAGE (Y)
    0x16, 0x00, 0xF0,               //             LOGICAL_MINIMUM (-4096)
    0x26, 0xFF, 0x0F,               //             LOGICAL_MAXIMUM (4095)
    0x75, 0x10,                     //             REPORT_SIZE (16)
    0x95, 0x02,                     //             REPORT_COUNT (2)
    0x81, 0x06,                     //             INPUT (Data, Var, Rel)

    /* Wheel */
    0x05, 0x01,                     //         USAGE_PAGE (Generic Desktop)
    0x09, 0x38,                     //             Usage (Wheel)
    0x15, 0x81,                     //             LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                     //             LOGICAL_MAXIMUM (127)
    0x75, 0x08,                     //             REPORT_SIZE (8)
    0x95, 0x01,                     //             REPORT_COUNT (1)
    0x81, 0x06,                     //             INPUT (Data, Var, Rel)

    0xC0,                           //     End Collection (Physical)
    0xc0                            // END_COLLECTION
};

__ALIGN4 static uc8 USB_HID_KeyboardReportDesc[] = {
    /* Keyboard data */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)

    0x85, HID_REPORT_KEYBOARD_ID,  //   REPORT_ID (2)

    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data, Var, Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data, Ary, Abs)

    0xc0,                          // END_COLLECTION

  /* Vendor specific data */
    0x06, 0x00, 0xff,                 // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                       // USAGE (Vendor Usage 1)
    0xa1, 0x01,                       // COLLECTION (Application)

    0x85, HID_REPORT_MOUSE_SETTINGS_ID,             //   REPORT_ID (n)
    0x09, 0x02,                       //   USAGE (Vendor Usage 2)
    0x15, 0x00,                       //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,                 //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                       //   REPORT_SIZE (8)
    0x96, DESC_H2B(HID_REPORT_MOUSE_SETTINGS_LEN),  //   REPORT_COUNT (n)
    0xB1, 0x02,                       //   INPUT/OUTPUT/FEATURE (Data, Var, Abs)

    0x85, HID_REPORT_MACROS_SETTINGS_ID,             //   REPORT_ID (n)
    0x09, 0x02,                       //   USAGE (Vendor Usage 2)
    0x15, 0x00,                       //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,                 //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                       //   REPORT_SIZE (8)
    0x96, DESC_H2B(HID_REPORT_MACROS_SETTINGS_LEN),  //   REPORT_COUNT (n)
    0xB1, 0x02,                       //   INPUT/OUTPUT/FEATURE (Data, Var, Abs)
    0xc0                              // END_COLLECTION
};

/* Private variables ---------------------------------------------------------------------------------------*/
static u32 gOutputReportBuffer;
__ALIGN4 static u8 gInputReportBuffer[64];
__ALIGN4 static u8 gFeatureReport1Buffer[1 + HID_REPORT_MOUSE_SETTINGS_LEN] = {0};
__ALIGN4 static u8 gFeatureReport2Buffer[1 + HID_REPORT_MACROS_SETTINGS_LEN] = {0};

/* Private function prototypes -----------------------------------------------------------------------------*/
static void USBDClass_Reset(u32 uPara);
static void USBDClass_SetReportCallBack0(u32 uPara);

static void USBDClass_Standard_GetDescriptor(USBDCore_Device_TypeDef *pDev);
static void USBDClass_Request(USBDCore_Device_TypeDef *pDev);
static void USBDClass_GetReport(USBDCore_Device_TypeDef *pDev);
static void USBDClass_GetIdle(USBDCore_Device_TypeDef *pDev);
static void USBDClass_GetProtocol(USBDCore_Device_TypeDef *pDev);
static void USBDClass_SetReport(USBDCore_Device_TypeDef *pDev);
static void USBDClass_SetIdle(USBDCore_Device_TypeDef *pDev);
static void USBDClass_SetProtocol(USBDCore_Device_TypeDef *pDev);

/* Global Function -----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  USB Class initialization.
  * @param  pClass: pointer of USBDCore_Class_TypeDef
  * @retval None
  ***********************************************************************************************************/
void USBDClass_Init(USBDCore_Class_TypeDef *pClass)
{
  pClass->CallBack_MainRoutine.func = NULL;

  pClass->CallBack_Reset.func = USBDClass_Reset;

  pClass->CallBack_ClassGetDescriptor = USBDClass_Standard_GetDescriptor;

  pClass->CallBack_ClassRequest = USBDClass_Request;

  #ifdef RETARGET_IS_USB
  pClass->CallBack_EPTn[RETARGET_RX_EPT] = SERIAL_USBDClass_RXHandler;
  #endif

  return;
}

/* Private functions ---------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  USB Class Reset.
  * @param  uPara: Parameter for Class Reset.
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_Reset(u32 uPara)
{

}

/*********************************************************************************************************//**
  * @brief  USB Device Class Request
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_Request(USBDCore_Device_TypeDef *pDev)
{
  u16 uUSBCmd = *((u16 *)(&(pDev->Request)));

#ifdef RETARGET_IS_USB
  SERIAL_USBDClass_Request(pDev);
#endif

  switch (uUSBCmd)
  {
    /*------------------------------------------------------------------------------------------------------*/
    /* | bRequest             | Data transfer direction | Type                | Recipient   | Data          */
    /*------------------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------------------*/
    /* | 01_Get Report        | 80_Device-to-Host       | 20_Class Request    | 1_Interface | 01A1h         */
    /*------------------------------------------------------------------------------------------------------*/
    case (CLASS_REQ_01_GET_RPOT | REQ_DIR_01_D2H | REQ_TYPE_01_CLS | REQ_REC_01_INF):
    {
      __DBG_USBPrintf("%06ld GET RPOT\t[%02d][%02d]\r\n", __DBG_USBCount, pDev->Request.wValueH, pDev->Request.wLength );
      USBDClass_GetReport(pDev);
      break;
    }
    /*------------------------------------------------------------------------------------------------------*/
    /* | 02_Get Idle          | 80_Device-to-Host       | 20_Class Request    | 1_Interface | 02A1h         */
    /*------------------------------------------------------------------------------------------------------*/
    case (CLASS_REQ_02_GET_IDLE | REQ_DIR_01_D2H | REQ_TYPE_01_CLS | REQ_REC_01_INF):
    {
      __DBG_USBPrintf("%06ld GET IDLE\r\n", __DBG_USBCount);
      USBDClass_GetIdle(pDev);
      break;
    }
    /*------------------------------------------------------------------------------------------------------*/
    /* | 03_Get Protocol      | 80_Device-to-Host       | 20_Class Request    | 1_Interface | 03A1h         */
    /*------------------------------------------------------------------------------------------------------*/
    case (CLASS_REQ_03_GET_PTCO | REQ_DIR_01_D2H | REQ_TYPE_01_CLS | REQ_REC_01_INF):
    {
      __DBG_USBPrintf("%06ld GET PTCO\r\n", __DBG_USBCount);
      USBDClass_GetProtocol(pDev);
      break;
    }
    /*------------------------------------------------------------------------------------------------------*/
    /* | 09_Set Report        | 00_Host-to-Device       | 20_Class Request    | 1_Interface | 0921h         */
    /*------------------------------------------------------------------------------------------------------*/
    case (CLASS_REQ_09_SET_RPOT | REQ_DIR_00_H2D | REQ_TYPE_01_CLS | REQ_REC_01_INF):
    {
      __DBG_USBPrintf("%06ld SET RPOT\t[%02d][%02d]\r\n", __DBG_USBCount, pDev->Request.wValueH, pDev->Request.wLength);
      USBDClass_SetReport(pDev);
      break;
    }
    /*------------------------------------------------------------------------------------------------------*/
    /* | 0A_Set Idle          | 00_Host-to-Device       | 20_Class Request    | 1_Interface | 0A21h         */
    /*------------------------------------------------------------------------------------------------------*/
    case (CLASS_REQ_0A_SET_IDLE | REQ_DIR_00_H2D | REQ_TYPE_01_CLS | REQ_REC_01_INF):
    {
      __DBG_USBPrintf("%06ld SET IDLE\r\n", __DBG_USBCount);
      USBDClass_SetIdle(pDev);
      break;
    }
    /*------------------------------------------------------------------------------------------------------*/
    /* | 0B_Set Protocol      | 00_Host-to-Device       | 20_Class Request    | 1_Interface | 0B21h         */
    /*------------------------------------------------------------------------------------------------------*/
    case (CLASS_REQ_0B_SET_PTCO | REQ_DIR_00_H2D | REQ_TYPE_01_CLS | REQ_REC_01_INF):
    {
      __DBG_USBPrintf("%06ld SET PTCO\r\n", __DBG_USBCount);
      USBDClass_SetProtocol(pDev);
      break;
    }
  }

  return;
}

/*********************************************************************************************************//**
  * @brief  USB Device Class Standard Request - GET_DESCRIPTOR
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_Standard_GetDescriptor(USBDCore_Device_TypeDef *pDev)
{
  u32 type  = pDev->Request.wValueH;
  u32 index = pDev->Request.wIndex;

  switch (type)
  {
    case DESC_TYPE_21_HID:
    {
      if (index == 0)
      {
        pDev->Transfer.pData = (uc8 *)((pDev->Desc.pConfnDesc) + CLASS_DESC_OFFSET);
      }
      else if (index == 1)
      {
        pDev->Transfer.pData = (uc8 *)((pDev->Desc.pConfnDesc) + CLASS_DESC_OFFSET1);
      }
      //  else if (index == 2)
      //  {
      //    pDev->Transfer.pData = (uc8 *)((pDev->Desc.pConfnDesc) + CLASS_DESC_OFFSET2);
      //  }

      else
      {
        break;
      }
      pDev->Transfer.sByteLength = DESC_LEN_HID;
      pDev->Transfer.Action = USB_ACTION_DATAIN;
      break;
    }
    case DESC_TYPE_22_RPOT:
    {
      if (index == 0)
      {
        pDev->Transfer.pData = (uc8 *)(USB_HID_MouseReportDesc);
        pDev->Transfer.sByteLength = DESC_LEN_MOUSE_REPORT;
      }
      else if (index == 1)
      {
        pDev->Transfer.pData = (uc8 *)(USB_HID_KeyboardReportDesc);
        pDev->Transfer.sByteLength = DESC_LEN_KB_REPORT + DESC_LEN_VENDOR_REPORT;
      }
      //  else if (index == 2)
      //  {
      //    pDev->Transfer.pData = (uc8 *)(USB_HID_VendorSpecificReportDesc);
      //    pDev->Transfer.sByteLength = DESC_LEN_VENDOR_REPORT;
      //  }
      else
      {
        break;
      }
      pDev->Transfer.Action = USB_ACTION_DATAIN;
      break;
    }
    case DESC_TYPE_23_PHY:
    {
      break;
    }
  } /* switch (type)                                                                                        */

  return;
}


/*********************************************************************************************************//**
  * @brief  USB Device Class Request - GET_REPORT
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_GetReport(USBDCore_Device_TypeDef *pDev)
{
  u32 value = pDev->Request.wValueH;
  u32 len = pDev->Request.wLength;
  u32 id = pDev->Request.wValueL;

  switch (value)
  {
    case HID_RPOT_TYPE_01_INPUT:
    {
      pDev->Transfer.pData = (uc8 *)&(gInputReportBuffer);
      pDev->Transfer.sByteLength = len;
      pDev->Transfer.Action= USB_ACTION_DATAIN;
      break;
    }
    case HID_RPOT_TYPE_03_FEATURE:
    {
      if (id == HID_REPORT_MOUSE_SETTINGS_ID) {
        memcpy((void*)&gFeatureReport1Buffer[1], (void*)&rgb_matrix_config, sizeof(rgb_config_t));
        pDev->Transfer.pData = (uc8 *)&(gFeatureReport1Buffer);
        pDev->Transfer.sByteLength = HID_REPORT_MOUSE_SETTINGS_LEN + 1;
        pDev->Transfer.Action= USB_ACTION_DATAIN;
      }
      else if (id == HID_REPORT_MACROS_SETTINGS_ID) {
        pDev->Transfer.pData = (uc8 *)&(gFeatureReport2Buffer);
        pDev->Transfer.sByteLength = HID_REPORT_MACROS_SETTINGS_LEN + 1;
        pDev->Transfer.Action= USB_ACTION_DATAIN;
      }
      break;
    }
  }

  return;
}

/*********************************************************************************************************//**
  * @brief  USB Device Class Request - GET_IDLE
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_GetIdle(USBDCore_Device_TypeDef *pDev)
{
  return;
}

/*********************************************************************************************************//**
  * @brief  USB Device Class Request - GET_PROTOCOL
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_GetProtocol(USBDCore_Device_TypeDef *pDev)
{
  return;
}

/*********************************************************************************************************//**
  * @brief  USB Device Class Request - SET_REPORT
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_SetReport(USBDCore_Device_TypeDef *pDev)
{
  u32 value = pDev->Request.wValueH;
  u32 len = pDev->Request.wLength;
  u32 id = pDev->Request.wValueL;

  switch (value)
  {
    case HID_RPOT_TYPE_02_OUTPUT:
    {
      break;
    }
    case HID_RPOT_TYPE_03_FEATURE:
    {
      if (id == HID_REPORT_MOUSE_SETTINGS_ID)
      {
        pDev->Transfer.pData = (uc8 *)&(gFeatureReport1Buffer);
        pDev->Transfer.sByteLength = len;
        pDev->Transfer.Action= USB_ACTION_DATAOUT;
        pDev->Transfer.CallBack_OUT.func = USBDClass_SetReportCallBack0;
        pDev->Transfer.CallBack_OUT.uPara = id;
      }
      else if (id == HID_REPORT_MACROS_SETTINGS_ID)
      {
        pDev->Transfer.pData = (uc8 *)&(gFeatureReport2Buffer);
        pDev->Transfer.sByteLength = len;
        pDev->Transfer.Action= USB_ACTION_DATAOUT;
        pDev->Transfer.CallBack_OUT.func = USBDClass_SetReportCallBack0;
        pDev->Transfer.CallBack_OUT.uPara = id;
      }
      break;
    }
  }

  return;
}

/*********************************************************************************************************//**
  * @brief  USB Device Class Request - SET_IDLE
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_SetIdle(USBDCore_Device_TypeDef *pDev)
{
  return;
}

/*********************************************************************************************************//**
  * @brief  USB Device Class Request - SET_PROTOCOL
  * @param  pDev: pointer of USB Device
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_SetProtocol(USBDCore_Device_TypeDef *pDev)
{
  return;
}


/*********************************************************************************************************//**
  * @brief  USB Device SET_REPORT call Back function
  * @param  uPara: Parameter for SET_REPORT call back function (SET_REPORT wLength)
  * @retval None
  ***********************************************************************************************************/
static void USBDClass_SetReportCallBack0(u32 uPara)
{
  switch (uPara)
  {
    /* Report ID #1 for RGB led configurations */
    case HID_REPORT_MOUSE_SETTINGS_ID: {
      memcpy((void*)&rgb_matrix_config, &gFeatureReport1Buffer[1], sizeof(rgb_config_t));
      eeconfig_update_rgb_matrix();
      break;
    }

    /* Report ID #2 for Mouse configurations */
    case HID_REPORT_MACROS_SETTINGS_ID:

      break;

    default:
      break;
  }
  return;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
