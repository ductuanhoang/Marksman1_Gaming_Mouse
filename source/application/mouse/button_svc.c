/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_time.h"
#include "ht32_board.h"
#include "mouse_data.h"
#include "button.h"
#include "pmw3360.h"
#include "usb_svc.h"

/* Private constants ---------------------------------------------------------------------------------------*/
#define BTN_AUX_BASE_IDX BTN_DPI

/* Private types -------------------------------------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------------------------------------*/

/* USB HID Report Descriptor                                                                                */

/* Private function prototypes -----------------------------------------------------------------------------*/
static uint8_t aux_btn_read(uint8_t aux_btn_idx);
static uint32_t get_tick_ms(void);

static void on_btn_pressed(int btn_idx, int event, void *args);
extern uint8_t buffer_dpi_idx_xy;
extern uint8_t buffer_dpi_idx_x;
extern uint8_t buffer_dpi_mode;

/* Private variables ---------------------------------------------------------------------------------------*/
static button_config_t aux_btn_cfg[] = {
    /*  last_state    idle_level    type          debounce_en   read_fn */
    {1, 1, 1, 0, aux_btn_read}, /* BTN_DPI */
    {1, 1, 1, 0, aux_btn_read}, /* BTN_DPI_MINUS */
    {1, 1, 1, 0, aux_btn_read}, /* BTN_SNIPER */
    {1, 1, 1, 0, aux_btn_read}, /* BTN_POOLING_RATE */
};

/* Global Function -----------------------------------------------------------------------------------------*/
/*********************************************************************************************************/ /**
                                                                                                             * @brief  Initialize button services
                                                                                                             * @param  None
                                                                                                             * @retval None
                                                                                                             ***********************************************************************************************************/
void button_svc_init(void)
{
    button_init(aux_btn_cfg, sizeof(aux_btn_cfg) / sizeof(button_config_t));
    button_set_gettick(get_tick_ms);
    button_set_callback(E_BUTTON_PRESS, on_btn_pressed, NULL);
    button_set_callback(E_BUTTON_RELEASE, on_btn_pressed, NULL);
}

/*********************************************************************************************************/ /**
                                                                                                             * @brief  Handle sensor pooling
                                                                                                             * @param  None
                                                                                                             * @retval None
                                                                                                             ***********************************************************************************************************/
void button_svc(void)
{
    button_prcess_loop();
    if (buffer_dpi_mode == 0)
    {
        if (buffer_dpi_idx_xy != curr_dpi_idx_xy)
        {
            drv_mouse_use_difference_dip(false);
            curr_dpi_idx_xy = buffer_dpi_idx_xy;
            drv_mouse_set_cpi(dpi[curr_dpi_idx_xy]);
        }
    }
    else
    {
        if ((buffer_dpi_idx_xy != curr_dpi_idx_xy) || (buffer_dpi_idx_x != curr_dpi_idx_x))
        {
            printf("[button_svc] set DPI %d --- %d", buffer_dpi_idx_xy, buffer_dpi_idx_x);
            curr_dpi_idx_xy = buffer_dpi_idx_xy;
            curr_dpi_idx_x = buffer_dpi_idx_x;

            drv_mouse_use_difference_dip(true);
            drv_mouse_set_cpiY(dpi[curr_dpi_idx_xy]);
            drv_mouse_set_cpiX(dpi[curr_dpi_idx_x]);
        }
    }
}

/*********************************************************************************************************/ /**
                                                                                                             * @brief  Handle sensor pooling
                                                                                                             * @param  None
                                                                                                             * @retval None
                                                                                                             ***********************************************************************************************************/

static uint8_t aux_btn_read(uint8_t aux_btn_idx)
{
    return board_button_read((bsp_btn_id_t)(aux_btn_idx + (uint8_t)BTN_DPI));
}

static uint32_t get_tick_ms(void)
{
    uint32_t tick = Time_GetTick();
    return TIME_TICK2MS(tick);
}
/*********************************************************************************************************/ /**
                                                                                                             * @brief  button pressed callback
                                                                                                             * @param  None
                                                                                                             * @retval None
                                                                                                             ***********************************************************************************************************/
static void on_btn_pressed(int btn_idx, int event, void *args)
{
    /* Process Sniper button */
    if (btn_idx == (BTN_SNIPER - BTN_AUX_BASE_IDX))
    {
        if (event == E_BUTTON_PRESS)
        {
            printf("[SNIP] Set DPI to %d\r\n", 700);
            drv_mouse_set_cpi(700);
        }
        else if (event == E_BUTTON_RELEASE)
        {
            printf("[SNIP] Set DPI to %d\r\n", dpi[curr_dpi_idx_xy]);
            drv_mouse_set_cpi(dpi[curr_dpi_idx_xy]);
        }
    }

    /* DPI+ Sniper button */
    if (btn_idx == (BTN_DPI - BTN_AUX_BASE_IDX))
    {
        if (event == E_BUTTON_PRESS)
        {
            if (buffer_dpi_mode == 0)
            {
                curr_dpi_idx_xy++;
                if (curr_dpi_idx_xy >= MAX_DPI_INDEX)
                    curr_dpi_idx_xy = MAX_DPI_INDEX - 1;
                // curr_dpi_idx_xy = (curr_dpi_idx_xy == MAX_DPI_INDEX) ? 0 : (curr_dpi_idx_xy + 1);
                printf("[DPI+] Set DPI to %d\r\n", dpi[curr_dpi_idx_xy]);
                drv_mouse_set_cpi(dpi[curr_dpi_idx_xy]);
                buffer_dpi_idx_xy = curr_dpi_idx_xy;
            }
            else
            {
                curr_dpi_idx_xy++;
                curr_dpi_idx_x++;

                if (curr_dpi_idx_xy >= MAX_DPI_INDEX)
                    curr_dpi_idx_xy = MAX_DPI_INDEX - 1;

                if (curr_dpi_idx_x >= MAX_DPI_INDEX)
                    curr_dpi_idx_x = MAX_DPI_INDEX - 1;

                // curr_dpi_idx_xy = (curr_dpi_idx_xy == MAX_DPI_INDEX) ? 0 : (curr_dpi_idx_xy + 1);
                printf("[DPI+] Set DPI xy to %d\r\n", dpi[curr_dpi_idx_xy]);
                printf("[DPI+] Set DPI x to %d\r\n", dpi[curr_dpi_idx_x]);

                drv_mouse_use_difference_dip(true);
                drv_mouse_set_cpiY(dpi[curr_dpi_idx_xy]);
                drv_mouse_set_cpiX(dpi[curr_dpi_idx_x]);

                buffer_dpi_idx_xy = curr_dpi_idx_xy;
                buffer_dpi_idx_x = curr_dpi_idx_x;
            }
        }
    }

    /* DPI- minus button */
    if (btn_idx == (BTN_DPI_MINUS - BTN_AUX_BASE_IDX))
    {

        if (event == E_BUTTON_PRESS)
        {
            if (buffer_dpi_mode == 0)
            {
                if (curr_dpi_idx_xy == 0)
                    curr_dpi_idx_xy = 0;
                else
                    curr_dpi_idx_xy--;
                printf("[DPI+] Set DPI to %d\r\n", dpi[curr_dpi_idx_xy]);
                drv_mouse_set_cpi(dpi[curr_dpi_idx_xy]);
                buffer_dpi_idx_xy = curr_dpi_idx_xy;
            }
            else
            {
                if (curr_dpi_idx_xy == 0)
                    curr_dpi_idx_xy = 0;
                else
                    curr_dpi_idx_xy--;

                if (curr_dpi_idx_x == 0)
                    curr_dpi_idx_x = 0;
                else
                    curr_dpi_idx_x--;

                printf("[DPI+] Set DPI xy to %d\r\n", dpi[curr_dpi_idx_xy]);
                printf("[DPI+] Set DPI x to %d\r\n", dpi[curr_dpi_idx_x]);

                drv_mouse_use_difference_dip(true);
                drv_mouse_set_cpiY(dpi[curr_dpi_idx_xy]);
                drv_mouse_set_cpiX(dpi[curr_dpi_idx_x]);

                buffer_dpi_idx_xy = curr_dpi_idx_xy;
                buffer_dpi_idx_x = curr_dpi_idx_x;
            }
            
        }
        else if (event == E_BUTTON_RELEASE)
        {
            // usb_hid_send_key(0, 0x00);
        }
    }

    /* Polling rate button */
    if (btn_idx == (BTN_POOLING_RATE - BTN_AUX_BASE_IDX))
    {
        if (event == E_BUTTON_PRESS)
        {
            curr_pool_idx = (curr_pool_idx == 0) ? (MAX_POLLING_RATE_INDEX - 1) : (curr_pool_idx - 1);
            printf("[RATE] Set polling rate to %d(idx: %d, loop: %d)\r\n", polling_rate[curr_pool_idx], curr_pool_idx, 1000 / polling_rate[curr_pool_idx]);
        }
    }
    // printf("Button index %d pressed\r\n", btn_idx);
}

/*********************************************************************************************************/ /**
                                                                                                             * @brief  Encoder exti interrupt handler
                                                                                                             * @param  None
                                                                                                             * @retval None
                                                                                                             ***********************************************************************************************************/
void EXTI4_15_IRQHandler(void)
{
    if (EXTI_GetEdgeFlag(EXTI_CHANNEL_4))
    {
        if (GPIO_ReadInBit(HT_GPIOC, GPIO_PIN_5) == 1)
        {
            exti_cnt--;
        }
        EXTI_ClearEdgeFlag(EXTI_CHANNEL_4);
    }

    if (EXTI_GetEdgeFlag(EXTI_CHANNEL_5))
    {
        if (GPIO_ReadInBit(HT_GPIOC, GPIO_PIN_4) == 0)
        {
            exti_cnt++;
        }
        EXTI_ClearEdgeFlag(EXTI_CHANNEL_5);
    }
}