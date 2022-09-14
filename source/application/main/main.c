/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_usbd_core.h"
#include "ht32_usbd_class.h"
#include "ht32_usbd_descriptor.h"
#include "mouse_data.h"
#include "sensor_svc.h"
#include "usb_svc.h"
#include "button_svc.h"
#include "led_svc.h"
#include "common.h"
/* Private function prototypes -----------------------------------------------------------------------------*/
void CKCU_Configuration(void);
void GPIO_Configuration(void);

/* handle debugging print */
static void debug_svc(void);

/* Global variables ----------------------------------------------------------------------------------------*/

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  CKCU_Configuration();               /* System Related configuration                                       */
  Time_Init();

  GPIO_Configuration();               /* GPIO Related configuration                                         */
  RETARGET_Configuration();           /* Retarget Related configuration                                     */
  button_svc_init();
  sensor_init();
  led_init();
  usb_init();                /* USB Related configuration                                          */
  usb_connect();
  APP_LOGD("tuan123");
  while (1)
  {
    /* USB core hande and hid report */
    usb_svc();

    /* Mouse sensor polling */
    sensor_svc();

    /* Button polling */
    button_svc();

    /* build and send report */
    usb_hid_build_and_send_report();

    /* LED service */
    led_svc();

    /* Debug */
    debug_svc();
  }
}

/*********************************************************************************************************//**
  * @brief  Configure the system clocks.
  * @retval None
  ***********************************************************************************************************/
void CKCU_Configuration(void)
{
#if 1
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
  CKCUClock.Bit.USBD       = 1;
  CKCUClock.Bit.PB         = 1;
  CKCUClock.Bit.PC         = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.EXTI       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
#endif
}

/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports.
  * @retval None
  ***********************************************************************************************************/
void GPIO_Configuration(void)
{
  /*--------------------------------------------------------------------------------------------------------*/
  /* Configure GPIO as input for button                                                                     */
  /*--------------------------------------------------------------------------------------------------------*/
  board_button_init_all();

  /*--------------------------------------------------------------------------------------------------------*/
  /* Configure GPIO as input for TTC Encoder                                                                */
  /*--------------------------------------------------------------------------------------------------------*/
  board_encoder_init();
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif

/* handle the de control */
static void debug_svc(void)
{
  static uint32_t status_pooling_tick = 0;
  uint32_t sys_tick = Time_GetTick();

  /* Debug */
  if( sys_tick - status_pooling_tick > TIME_MS2TICK(3000)) {
    status_pooling_tick = sys_tick;

    extern uint32_t     g_rgb_timer;
    APP_LOGI("g_rgb_timer: %d, ms: %d", g_rgb_timer, TIME_TICK2MS(g_rgb_timer));
    // usb_hid_send_key(0, 0x04);
  }
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
