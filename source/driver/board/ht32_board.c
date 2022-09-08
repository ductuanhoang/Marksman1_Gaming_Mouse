/*********************************************************************************************************//**
 * @file    ht32_board.c
 * @version $Rev:: 5658         $
 * @date    $Date:: 2021-11-26 #$
 * @brief   HT32 target board related file.
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
#include "ht32_config.h"
#include "ht32_board.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup HT32_Board
  * @{
  */

/** @defgroup HT32F5xxxx_BOARD HT32F5xxxx_BOARD
  * @brief Provides firmware functions to manage LEDs, push-buttons
  *        and COM ports and is available on HT32F5xxxx Starter kit/Development board.
  * @{
  */


/* Private types -------------------------------------------------------------------------------------------*/
/** @defgroup HT32F5xxxx_BOARD_Private_TypesDefinitions HT32F5xxxx Board private types definitions
  * @{
  */

typedef struct
{
  u8 ID;
  u16 Pin;
  AFIO_MODE_Enum Mode;
} HT32_Board_GPIO_TypeDef;

typedef struct
{
  HT_USART_TypeDef* USARTx;
  HT32_Board_GPIO_TypeDef Tx_IO;
  HT32_Board_GPIO_TypeDef Rx_IO;
} HT32_Board_COM_TypeDef;

typedef struct
{
  u8                port_id;
  HT_GPIO_TypeDef*  port;
  u16               pin;
  AFIO_MODE_Enum    mode;
  u8                exti_ch;
  GPIO_PR_Enum      pull_mode;
} board_button_config_t;


/**
  * @}
  */

/* Private function prototypes -----------------------------------------------------------------------------*/
static void _HT32F_DVB_ClockConfig(u32 GpioId);

/* Global variables ----------------------------------------------------------------------------------------*/
/** @defgroup HT32F5xxxx_BOARD_Global_Variable HT32F5xxxx Board global variables
  * @{
  */

const board_button_config_t bsp_hw_button_cfg[] = {
/* port_id                      port                           pin                           mode                            exti_ch                           pull_mode */
  {MOUSE_LEFT_BUTTON_GPIO_ID,   MOUSE_LEFT_BUTTON_GPIO_PORT,   MOUSE_LEFT_BUTTON_GPIO_PIN,   MOUSE_LEFT_BUTTON_AFIO_MODE,    MOUSE_LEFT_BUTTON_EXTI_CHANNEL,   GPIO_PR_UP},
  {MOUSE_RIGHT_BUTTON_GPIO_ID,  MOUSE_RIGHT_BUTTON_GPIO_PORT,  MOUSE_RIGHT_BUTTON_GPIO_PIN,  MOUSE_RIGHT_BUTTON_AFIO_MODE,   MOUSE_RIGHT_BUTTON_EXTI_CHANNEL,  GPIO_PR_UP},
  {MOUSE_MID_BUTTON_GPIO_ID,    MOUSE_MID_BUTTON_GPIO_PORT,    MOUSE_MID_BUTTON_GPIO_PIN,    MOUSE_MID_BUTTON_AFIO_MODE,     MOUSE_MID_BUTTON_EXTI_CHANNEL,    GPIO_PR_UP},
  {MOUSE_FWD_BUTTON_GPIO_ID,    MOUSE_FWD_BUTTON_GPIO_PORT,    MOUSE_FWD_BUTTON_GPIO_PIN,    MOUSE_FWD_BUTTON_AFIO_MODE,     MOUSE_FWD_BUTTON_EXTI_CHANNEL,    GPIO_PR_UP},
  {MOUSE_BACK_BUTTON_GPIO_ID,   MOUSE_BACK_BUTTON_GPIO_PORT,   MOUSE_BACK_BUTTON_GPIO_PIN,   MOUSE_BACK_BUTTON_AFIO_MODE,    MOUSE_BACK_BUTTON_EXTI_CHANNEL,   GPIO_PR_UP},
  {MOUSE_DPI_BUTTON_GPIO_ID,    MOUSE_DPI_BUTTON_GPIO_PORT,    MOUSE_DPI_BUTTON_GPIO_PIN,    MOUSE_DPI_BUTTON_AFIO_MODE,     MOUSE_DPI_BUTTON_EXTI_CHANNEL,    GPIO_PR_UP},
  {MOUSE_LED_BUTTON_GPIO_ID,    MOUSE_LED_BUTTON_GPIO_PORT,    MOUSE_LED_BUTTON_GPIO_PIN,    MOUSE_LED_BUTTON_AFIO_MODE,     MOUSE_LED_BUTTON_EXTI_CHANNEL,    GPIO_PR_UP},
  {MOUSE_SNIP_BUTTON_GPIO_ID,   MOUSE_SNIP_BUTTON_GPIO_PORT,   MOUSE_SNIP_BUTTON_GPIO_PIN,   MOUSE_SNIP_BUTTON_AFIO_MODE,    MOUSE_SNIP_BUTTON_EXTI_CHANNEL,   GPIO_PR_UP},
  {MOUSE_POOL_BUTTON_GPIO_ID,   MOUSE_POOL_BUTTON_GPIO_PORT,   MOUSE_POOL_BUTTON_GPIO_PIN,   MOUSE_POOL_BUTTON_AFIO_MODE,    MOUSE_POOL_BUTTON_EXTI_CHANNEL,   GPIO_PR_UP}
};

  const HT32_Board_COM_TypeDef gCOMInfo[COM_NUM] =
  {
    {
      COM1_PORT,
      { COM1_TX_GPIO_ID, COM1_TX_AFIO_PIN, COM1_TX_AFIO_MODE },
      { COM1_RX_GPIO_ID, COM1_RX_AFIO_PIN, COM1_RX_AFIO_MODE },
    },
  };

/**
  * @}
  */

/* Global functions ----------------------------------------------------------------------------------------*/
/** @defgroup HT32F5xxxx_BOARD_Global_Functions HT32F5xxxx Board global functions
  * @{
  */


/*********************************************************************************************************//**
 * @brief Configures Button GPIO and EXTI.
 * @param  BUTTON_x: Specifies the Button to be configured.
 *   This parameter can be one of following parameters:
 *     @arg BUTTON_WAKEUP: Wakeup Push Button
 *     @arg BUTTON_KEY1: Key1 Push Button
 *     @arg BUTTON_KEY2: Key2 Push Button
 * @param  BUTTON_MODE_x: Specifies Button mode.
 *   This parameter can be one of following parameters:
 *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO
 *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI channel with interrupt generation capability
 * @retval None
 ************************************************************************************************************/
void board_button_init_all(void)
{
  for (u8 ii = 0; ii < sizeof(bsp_hw_button_cfg)/sizeof(board_button_config_t); ii ++) {
    u32 bGpioId  = bsp_hw_button_cfg[ii].port_id;
    u32 uGpioPin = bsp_hw_button_cfg[ii].pin;
    HT_GPIO_TypeDef* GPIOx = bsp_hw_button_cfg[ii].port;

    /* Enable the GPIO Clock                                                                                  */
    _HT32F_DVB_ClockConfig(bGpioId);
    /* Enable the AFIO Clock                                                                                  */

    /* Configure Button pin as input floating                                                                 */
    AFIO_GPxConfig(bGpioId, uGpioPin, bsp_hw_button_cfg[ii].mode);
    GPIO_PullResistorConfig(GPIOx, uGpioPin, bsp_hw_button_cfg[ii].pull_mode);
    GPIO_DirectionConfig(GPIOx, uGpioPin, GPIO_DIR_IN);
    GPIO_InputConfig(GPIOx, uGpioPin, ENABLE);
  }
}

/*********************************************************************************************************//**
 * @brief Returns the selected Button state.
 * @param  BUTTON_x: Specifies the Button to be configured.
 *   This parameter can be one of following parameters:
 *     @arg BUTTON_WAKEUP: Wakeup Push Button
 *     @arg BUTTON_KEY1: Key1 Push Button
 *     @arg BUTTON_KEY2: Key2 Push Button
 * @return The Button GPIO pin value.
 ************************************************************************************************************/
u32 board_button_read(bsp_btn_id_t BUTTON_x)
{
  return GPIO_ReadInBit(bsp_hw_button_cfg[BUTTON_x].port, bsp_hw_button_cfg[BUTTON_x].pin);
}

/*********************************************************************************************************//**
 * @brief Configures COM port.
 * @param  COMn: Specifies the COM port to be configured.
 *   This parameter can be one of following parameters:
 *     @arg COM1
 *     @arg COM2
 * @param USART_InitStructure: Pointer to a USART_InitTypeDef structure that contains the configuration
 *        information for the specified USART peripheral.
 * @return None
 ************************************************************************************************************/
void board_debug_init(COM_TypeDef COMn, USART_InitTypeDef* USART_InitStructure)
{
  const HT32_Board_COM_TypeDef *COMInfo = &gCOMInfo[COMn];
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  /* Enable the AFIO Clock                                                                                  */
  /* Enable USART clock                                                                                     */
  COM1_CLK(CKCUClock) = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  // GPIO_DriveConfig(HT_GPIOA, COMInfo->Tx_IO.Pin, GPIO_DV_8MA);
  // GPIO_DriveConfig(HT_GPIOA, COMInfo->Rx_IO.Pin, GPIO_DV_8MA);

  /* Configure USART Tx/Rx as alternate function                                                            */
  AFIO_GPxConfig(COMInfo->Tx_IO.ID, COMInfo->Tx_IO.Pin, COMInfo->Tx_IO.Mode);
  AFIO_GPxConfig(COMInfo->Rx_IO.ID, COMInfo->Rx_IO.Pin, COMInfo->Rx_IO.Mode);

  /* USART configuration                                                                                    */
  USART_Init(COMInfo->USARTx, USART_InitStructure);

  /* USART Tx enable                                                                                        */
  USART_TxCmd(COMInfo->USARTx, ENABLE);
  /* USART Rx enable                                                                                        */
  USART_RxCmd(COMInfo->USARTx, ENABLE);

  #if (RETARGET_INT_MODE == 1)
  NVIC_EnableIRQ(COM1_IRQn);
  #endif
}

/**
  * @}
  */

/* Private functions ---------------------------------------------------------------------------------------*/
/** @defgroup HT32F5xxxx_BOARD_Private_Functions HT32F5xxxx Board private functions
  * @{
  */
/*********************************************************************************************************//**
 * @brief Enable GPIO and AFIO Clock.
 * @param  GpioId: Specifies the GPIO ID.
 * @retval None
 ************************************************************************************************************/
static void _HT32F_DVB_ClockConfig(u32 GpioId)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  u8 RegCK[GPIO_PORT_NUM] = {0};
  RegCK[GpioId] = 1;

  CKCUClock.Bit.PA         = RegCK[0];
  CKCUClock.Bit.PB         = RegCK[1];
  #if (LIBCFG_GPIOC)
  CKCUClock.Bit.PC         = RegCK[2];
  #endif
  #if (LIBCFG_GPIOD)
  CKCUClock.Bit.PD         = RegCK[3];
  #endif
  #if (LIBCFG_GPIOE)
  CKCUClock.Bit.PE         = RegCK[4];
  #endif
  #if (LIBCFG_GPIOF)
  CKCUClock.Bit.PF         = RegCK[5];
  #endif
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
}
/**
  * @}
  */

void board_encoder_init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  /* Enable the GPIO Clock                                                                                  */
  CKCUClock.Bit.PC         = 1;
  /* Enable the AFIO Clock                                                                                  */
  CKCUClock.Bit.AFIO       = 1;
  /* Enable the EXTI Clock                                                                                */
  CKCUClock.Bit.EXTI       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* Configure AFIO mode of input pins                                                                      */
  AFIO_GPxConfig(GPIO_PC, GPIO_PIN_4, AFIO_FUN_GPIO);
  AFIO_GPxConfig(GPIO_PC, GPIO_PIN_5, AFIO_FUN_GPIO);

  /* Enable GPIO Input Function                                                                             */
  GPIO_InputConfig(HT_GPIOC, GPIO_PIN_4, ENABLE);
  GPIO_InputConfig(HT_GPIOC, GPIO_PIN_5, ENABLE);

  /* Configure GPIO pull resistor of input pins                                                             */
  GPIO_PullResistorConfig(HT_GPIOC, GPIO_PIN_4, GPIO_PR_DISABLE);
  GPIO_PullResistorConfig(HT_GPIOC, GPIO_PIN_5, GPIO_PR_DISABLE);

  /* Select Port as EXTI Trigger Source                                                                     */
  AFIO_EXTISourceConfig(AFIO_EXTI_CH_4, AFIO_ESS_PC);
  AFIO_EXTISourceConfig(AFIO_EXTI_CH_5, AFIO_ESS_PC);

  { /* Configure EXTI Channel n as falling edge trigger                                                     */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    EXTI_InitTypeDef EXTI_InitStruct;

    /* PC4 --> Falling edge interrupt */
    EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_4;
    EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;
    EXTI_InitStruct.EXTI_DebounceCnt = 10000;   // Debounce 1ms = 48000 / 48MHz
    EXTI_InitStruct.EXTI_IntType = EXTI_NEGATIVE_EDGE;
    EXTI_Init(&EXTI_InitStruct);

    /* PC5 --> Rising edge interrupt */
    EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_5;
    EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;
    EXTI_InitStruct.EXTI_DebounceCnt = 10000;   // Debounce 1ms = 48000 / 48MHz
    EXTI_InitStruct.EXTI_IntType = EXTI_POSITIVE_EDGE;
    EXTI_Init(&EXTI_InitStruct);

    // EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_5;
    // EXTI_Init(&EXTI_InitStruct);
  }

  /* Enable EXTI & NVIC line Interrupt                                                                      */
  EXTI_IntConfig(EXTI_CHANNEL_4, ENABLE);
  EXTI_IntConfig(EXTI_CHANNEL_5, ENABLE);
  NVIC_EnableIRQ(EXTI4_15_IRQn);
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
