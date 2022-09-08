/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "button.h"

/* Private constants ---------------------------------------------------------------------------------------*/

/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/

/* USB HID Report Descriptor                                                                                */

/* Private variables ---------------------------------------------------------------------------------------*/
static uint8_t  u8_button_raw_data[MAX_BTN_SUPPORT] = {0};
static uint32_t u32_button_pressed_event;
static uint32_t u32_button_released_event;
static uint32_t u32_button_hold_event = 0;
static uint32_t u32_button_hold_event_exec = 0;
static uint32_t u32_button_on_hold_event = 0;
static uint8_t  u8_button_count;
static uint32_t u32_button_hold_time[MAX_BTN_SUPPORT] = {0};
static uint32_t u32_button_on_hold_time[MAX_BTN_SUPPORT] = {0};
static uint32_t u32_button_last_pressed_time[MAX_BTN_SUPPORT] = {0};
static uint8_t  u8_button_pressed_count[MAX_BTN_SUPPORT] = {0};
static uint32_t u32_button_has_hold_event = 0;

static button_config_t *hwcfg;
static button_event_callback event_callback_table[E_BUTTON_MAX] = {0};
static void* event_callback_arg[E_BUTTON_MAX] = {0};
static uint32_t(*get_tick_fn)(void);

/* Private function prototypes -----------------------------------------------------------------------------*/

/* Global Function -----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Initialize data for all buttons
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void button_init(button_config_t* cfg, uint8_t btn_nbr)
{
    uint8_t i = 0;

    if (NULL == cfg)
    {
        printf("Invalid init pointer");
        return;
    }

    if (MAX_BTN_SUPPORT < btn_nbr)
    {
        printf("Not support %d buttons", btn_nbr);
        return;
    }

    hwcfg = cfg;

    u8_button_count = btn_nbr;

    for(i = 0; i < btn_nbr; i ++)
    {
        /* Set Idle state */
        hwcfg[i].last_state = hwcfg[i].idle_level;
        u8_button_raw_data[i] = hwcfg[i].idle_level;
    }
}

/*********************************************************************************************************//**
  * @brief  Set the get tick function
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void button_set_gettick(uint32_t(*get_tick)(void))
{
    get_tick_fn = get_tick;
}

/*********************************************************************************************************//**
  * @brief  Set the event callback
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void button_set_callback(button_event_t event, button_event_callback cb, void* args)
{
    if(NULL != cb) {
        event_callback_table[event] = cb;
        event_callback_arg[event] = args;
    }
}

/*********************************************************************************************************//**
  * @brief  Main process loop
  * @param  None
  * @retval None
  ***********************************************************************************************************/
void button_prcess_loop(void)
{
    uint8_t i = 0;

    if (NULL == hwcfg)
    {
        printf("Invalid init pointer");
        return;
    }

    if (MAX_BTN_SUPPORT < u8_button_count)
    {
        printf("Not support %d buttons", u8_button_count);
        return;
    }

    // if (u8ButtonStateUpdateFlag)
    // {
    //     u8ButtonStateUpdateFlag = 0;

        for(i = 0; i < u8_button_count; i ++)
        {
            if (hwcfg[i].read_fn == NULL)
            {
                continue;
            }
            u8_button_raw_data[i] = hwcfg[i].read_fn(i);

            // APP_DEBUG("button status: %d", u8_button_raw_data[i]);
            if (u8_button_raw_data[i] ^ hwcfg[i].last_state)
            {
                if (u8_button_raw_data[i] != hwcfg[i].idle_level)
                {
                    if(hwcfg[i].debounce_en == 0)
                    {
                        u32_button_pressed_event |= (1 << i);
                    }
                    u32_button_hold_time[i] = get_tick_fn();
                }
                else
                {
                    if(hwcfg[i].debounce_en == 1)
                    {
                        uint32_t execTime = get_tick_fn() - u32_button_hold_time[i];
                        if(execTime > BUTTON_PRESS_DEBOUND_TIME)
                        {
                            u32_button_released_event |= 1 << i;
                        }
                    }
                    else
                    {
                        u32_button_released_event |= 1 << i;
                    }

                   u32_button_hold_time[i] = 0;
                   u32_button_on_hold_time[i] = 0;
                }
            }
            else
            {
                if(hwcfg[i].debounce_en == 1)
                {
                    if (u8_button_raw_data[i] != hwcfg[i].idle_level)
                    {
                        uint32_t execTime = get_tick_fn() - u32_button_hold_time[i];
                        if(execTime > BUTTON_PRESS_DEBOUND_TIME)
                        {
                            u32_button_pressed_event |= 1 << i;
                        }
                    }
                }
            }

            hwcfg[i].last_state = u8_button_raw_data[i];
        }
    // }

    for(i = 0; i < u8_button_count; i ++)
    {
        // if(hwcfg[i].button_type == 2) // one level button, ingore hold and onhold event
        // {
        //     continue;
        // }
        if (u8_button_raw_data[i] != hwcfg[i].idle_level)
        {
            if (get_tick_fn() - u32_button_hold_time[i] >= BUTTON_HOLD_TIME)
            {
                if (((u32_button_hold_event >> i) & 0x1))
                {
                    if (get_tick_fn() - u32_button_on_hold_time[i] >= BUTTON_ON_HOLD_TIME_FIRE_EVENT)
                    {
                        u32_button_on_hold_time[i] = get_tick_fn();
                        u32_button_on_hold_event |= (1 << i);
                        u32_button_has_hold_event |= (1 << i);
                    }
                }
                else
                {
                    u32_button_on_hold_time[i] = get_tick_fn();
                    u32_button_hold_event |= (1 << i);
                    u32_button_hold_event_exec &= ~(1 << i); // clear executed flag
                }
            }
        }

        if ((u32_button_pressed_event >> i) & 0x01)
        {
            // printf("Hard button press event, button idx [%d]", i);
            if (u32_button_last_pressed_time[i] == 0)
            {
                u32_button_last_pressed_time[i] = get_tick_fn();
            }

            if (get_tick_fn() - u32_button_last_pressed_time[i] >= BUTTON_DOUBLE_CLICK_TIME)
            {
                u8_button_pressed_count[i] = 0;
            }
            else
            {
                u8_button_pressed_count[i] += 1;

                uint32_t u32Event = E_BUTTON_MAX;

                if (u8_button_pressed_count[i] == 1) // double click
                {
                    u32Event = E_BUTTON_DOUBLE_CLICK;
                }
                else if (u8_button_pressed_count[i] == 2)
                {
                    u32Event = E_BUTTON_TRIPLE_CLICK;
                    u8_button_pressed_count[i] = 0;
                }

                if (u32Event != E_BUTTON_MAX)
                {
                    // printf("Hard button event[%d], button idx [%d]", u32Event, i);
                    if (NULL != event_callback_table[u32Event])
                    {
                        event_callback_table[u32Event](i, u32Event, event_callback_arg[u32Event]);
                    }
                }
            }

            u32_button_last_pressed_time[i] = get_tick_fn();

            if (NULL != event_callback_table[E_BUTTON_PRESS])
            {
                event_callback_table[E_BUTTON_PRESS](i, E_BUTTON_PRESS, event_callback_arg[E_BUTTON_PRESS]);
            }
            u32_button_pressed_event &= ~(1 << i);
        }
        if ((u32_button_released_event >> i) & 0x01)
        {
            // APP_DEBUG("Hard button release event, button idx [%d]", i);
            if (NULL != event_callback_table[E_BUTTON_RELEASE])
            {
                event_callback_table[E_BUTTON_RELEASE](i, E_BUTTON_RELEASE, (void*)u32_button_has_hold_event);
            }
            u32_button_released_event &= ~(1 << i);
            u32_button_hold_event &= ~(1 << i); // clear
            u32_button_has_hold_event &= !(1 << i);
        }

        if (!((u32_button_hold_event_exec >> i) & 0x1) &&  ((u32_button_hold_event >> i) & 0x1))
        {
            u32_button_hold_event_exec |= (1 << i);
            // APP_DEBUG("E_BUTTON_HOLD exec event %d %d", i, E_BUTTON_HOLD);
            uint32_t u32HoldTime = get_tick_fn() - u32_button_hold_time[i];
           if(NULL != event_callback_table[E_BUTTON_HOLD]) {
                event_callback_table[E_BUTTON_HOLD](i, E_BUTTON_HOLD, (void*)u32HoldTime);
           }
        }

        if ((u32_button_on_hold_event >> i) & 0x1)
        {
             uint32_t u32HoldTime = get_tick_fn() - u32_button_hold_time[i];
            // APP_DEBUG(" E_BUTTON_ON_HOLD exec event %d %d", i, E_BUTTON_ON_HOLD);
           if(NULL != event_callback_table[E_BUTTON_ON_HOLD]) {
                event_callback_table[E_BUTTON_ON_HOLD](i, E_BUTTON_ON_HOLD, (void*)u32HoldTime);
           }
           u32_button_on_hold_event &= ~(1 << i);
        }
    }
}
