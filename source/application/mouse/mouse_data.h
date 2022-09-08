#ifndef __MOUSE_DATA_H__
#define __MOUSE_DATA_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32_config.h"
#include "ht32.h"
#include "color.h"

/* Exported typedefs ---------------------------------------------------------------------------------------*/
typedef __PACKED_H struct
{
  u8 BIT0 :1;
  u8 BIT1 :1;
  u8 BIT2 :1;
  u8 BIT3 :1;
  u8 BIT4 :1;
  u8 BIT5 :1;
  u8 BIT6 :1;
  u8 BIT7 :1;
} __PACKED_F byte_t;

typedef __PACKED_H union
{
  u8 byte;
  byte_t bits;
} __PACKED_F mouse_button_t;

typedef __PACKED_H struct
{
  u8 report_id;
  mouse_button_t button;
  s16 x;
  s16 y;
  s8 wheel;
} __PACKED_F mouse_hid_data_t;

typedef __PACKED_H struct
{
  u8 report_id;
  u8 modifier;
  u8 keycode[6];
} __PACKED_F kb_hid_data_t;


/* Exported constants --------------------------------------------------------------------------------------*/
#define MAX_DPI_INDEX   (8U)
#define MAX_POLLING_RATE_INDEX   (3U)

/* Exported functions --------------------------------------------------------------------------------------*/

/* Exported variables---------------------------------------------------------------------------------------*/
extern uint8_t mouse_status_changed;
extern mouse_hid_data_t mouse_hid_data;
extern kb_hid_data_t kb_hid_data;
extern int32_t exti_cnt;

extern uint16_t dpi[MAX_DPI_INDEX];
extern uint8_t curr_dpi_idx;
extern RGB dpi_backlight_color[MAX_DPI_INDEX];

extern uint16_t polling_rate[MAX_POLLING_RATE_INDEX];
extern uint8_t curr_pool_idx;

#ifdef __cplusplus
}
#endif

#endif /* __MOUSE_DATA_H__ */
