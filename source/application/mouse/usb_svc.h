#ifndef __USB_SVC_H__
#define __USB_SVC_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32_config.h"
#include "ht32.h"
#include "ht32_board.h"
#include "mouse_data.h"

/* Private constants ---------------------------------------------------------------------------------------*/

/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/

/* Exported typedefs ---------------------------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------------------------------------*/
void usb_init(void);
void usb_connect(void);
void usb_disconnect(void);
void usb_svc(void);
void usb_hid_build_and_send_report(void);
void usb_hid_send_key(u8 modifier, u8 keycode);

#ifdef __cplusplus
}
#endif

#endif /* __USB_SVC_H__ */
