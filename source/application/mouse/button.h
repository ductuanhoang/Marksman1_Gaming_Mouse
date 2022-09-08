/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"

/* Private constants ---------------------------------------------------------------------------------------*/
#define MAX_BTN_SUPPORT                     (8U)
#define PERIOD_SCAN_IN_MS                   (20)
#define BUTTON_HOLD_TIME                    (1000)
#define BUTTON_ON_HOLD_TIME_FIRE_EVENT      (200)
#define IDLE_TIME_COUNT_IN_MS               (30000)
#define BUTTON_DOUBLE_CLICK_TIME            (500)
#define BUTTON_PRESS_DEBOUND_TIME           (50)
/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/

/* USB HID Report Descriptor                                                                                */

/* Private variables ---------------------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------------------------------------*/

/* Global Function -----------------------------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------------------------------------*/
typedef uint8_t (*p_btn_read)(uint8_t);
typedef void (*button_event_callback)(int, int, void*);     // cb(btn_index, event, args)

/**
 * Enum to define the event type of hard button
 */
typedef enum
{
    E_BUTTON_PRESS = 0,
    E_BUTTON_RELEASE,
    E_BUTTON_HOLD,
    E_BUTTON_ON_HOLD,
    E_BUTTON_DOUBLE_CLICK,
    E_BUTTON_TRIPLE_CLICK,
    E_BUTTON_ILDE,
    E_BUTTON_ILDE_BREAK,
    E_BUTTON_MAX
} button_event_t;

typedef struct
{
    uint8_t         last_state;
    uint32_t        idle_level;
    uint8_t         type;     // hardware button = 1, ADC = 0, One level button = 2
    uint8_t         debounce_en; // Debound enable;
    p_btn_read      read_fn;
} button_config_t;

/* Exported functions --------------------------------------------------------------------------------------*/
void button_init(button_config_t* cfg, uint8_t btn_nbr);
void button_prcess_loop(void);
void button_set_callback(button_event_t event, button_event_callback cb, void* args);
void button_set_gettick(uint32_t(*get_tick)(void));

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */
