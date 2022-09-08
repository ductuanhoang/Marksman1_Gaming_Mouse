/*********************************************************************************************************//**
 * @file    ht32_board.h
 * @version $Rev:: 5658         $
 * @date    $Date:: 2021-11-26 #$
 * @brief   HT32 target board definition file.
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

/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __HT32_BOARD_H
#define __HT32_BOARD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32_config.h"
#include "ht32.h"

/* Exported typedefs ---------------------------------------------------------------------------------------*/
typedef enum
{
  BTN_LEFT = 0,
  BTN_RIGHT,
  BTN_MID,
  BTN_FWD,
  BTN_BACK,
  BTN_DPI,
  BTN_LED,
  BTN_SNIPER,
  BTN_POOLING_RATE,
} bsp_btn_id_t;

typedef enum
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;

// extern HT_GPIO_TypeDef* const GPIO_PORT[GPIO_PORT_NUM];

/* Exported constants ---------------------------------------------------------------------------------------*/
#define LEDn                        (3)

#define LED1_GPIO_ID                (GPIO_PB)
#define LED1_GPIO_PIN               (GPIO_PIN_4)
#define LED1_AFIO_MODE              (AFIO_FUN_GPIO)

#define LED2_GPIO_ID                (GPIO_PA)
#define LED2_GPIO_PIN               (GPIO_PIN_14)
#define LED2_AFIO_MODE              (AFIO_FUN_GPIO)

#define LED3_GPIO_ID                (GPIO_PA)
#define LED3_GPIO_PIN               (GPIO_PIN_15)
#define LED3_AFIO_MODE              (AFIO_FUN_GPIO)

/* Pin assignment for buttons */
#define BUTTONn                     (9)

/* Left Mouse */
#define MOUSE_LEFT_BUTTON_GPIO_ID             (GPIO_PB)
#define MOUSE_LEFT_BUTTON_GPIO_PORT           (HT_GPIOB)
#define MOUSE_LEFT_BUTTON_GPIO_PIN            (GPIO_PIN_0)
#define MOUSE_LEFT_BUTTON_AFIO_MODE           (AFIO_FUN_GPIO)
#define MOUSE_LEFT_BUTTON_EXTI_CHANNEL        (0)
/* Right Mouse */
#define MOUSE_RIGHT_BUTTON_GPIO_ID            (GPIO_PC)
#define MOUSE_RIGHT_BUTTON_GPIO_PORT          (HT_GPIOC)
#define MOUSE_RIGHT_BUTTON_GPIO_PIN           (GPIO_PIN_3)
#define MOUSE_RIGHT_BUTTON_AFIO_MODE          (AFIO_FUN_GPIO)
#define MOUSE_RIGHT_BUTTON_EXTI_CHANNEL       (3)
/* Middle Mouse */
#define MOUSE_MID_BUTTON_GPIO_ID              (GPIO_PC)
#define MOUSE_MID_BUTTON_GPIO_PORT            (HT_GPIOC)
#define MOUSE_MID_BUTTON_GPIO_PIN             (GPIO_PIN_2)
#define MOUSE_MID_BUTTON_AFIO_MODE            (AFIO_FUN_GPIO)
#define MOUSE_MID_BUTTON_EXTI_CHANNEL         (2)
/* Forward Button */
#define MOUSE_FWD_BUTTON_GPIO_ID              (GPIO_PB)
#define MOUSE_FWD_BUTTON_GPIO_PORT            (HT_GPIOB)
#define MOUSE_FWD_BUTTON_GPIO_PIN             (GPIO_PIN_3)
#define MOUSE_FWD_BUTTON_AFIO_MODE            (AFIO_FUN_GPIO)
#define MOUSE_FWD_BUTTON_EXTI_CHANNEL         (3)
/* Back button */
#define MOUSE_BACK_BUTTON_GPIO_ID             (GPIO_PB)
#define MOUSE_BACK_BUTTON_GPIO_PORT           (HT_GPIOB)
#define MOUSE_BACK_BUTTON_GPIO_PIN            (GPIO_PIN_4)
#define MOUSE_BACK_BUTTON_AFIO_MODE           (AFIO_FUN_GPIO)
#define MOUSE_BACK_BUTTON_EXTI_CHANNEL        (4)
/* DPI button */
#define MOUSE_DPI_BUTTON_GPIO_ID              (GPIO_PC)
#define MOUSE_DPI_BUTTON_GPIO_PORT            (HT_GPIOC)
#define MOUSE_DPI_BUTTON_GPIO_PIN             (GPIO_PIN_1)
#define MOUSE_DPI_BUTTON_AFIO_MODE            (AFIO_FUN_GPIO)
#define MOUSE_DPI_BUTTON_EXTI_CHANNEL         (1)
/* LED button */
#define MOUSE_LED_BUTTON_GPIO_ID              (GPIO_PB)
#define MOUSE_LED_BUTTON_GPIO_PORT            (HT_GPIOB)
#define MOUSE_LED_BUTTON_GPIO_PIN             (GPIO_PIN_7)
#define MOUSE_LED_BUTTON_AFIO_MODE            (AFIO_FUN_GPIO)
#define MOUSE_LED_BUTTON_EXTI_CHANNEL         (7)
/* Sniper button */
#define MOUSE_SNIP_BUTTON_GPIO_ID             (GPIO_PB)
#define MOUSE_SNIP_BUTTON_GPIO_PORT           (HT_GPIOB)
#define MOUSE_SNIP_BUTTON_GPIO_PIN            (GPIO_PIN_2)
#define MOUSE_SNIP_BUTTON_AFIO_MODE           (AFIO_FUN_GPIO)
#define MOUSE_SNIP_BUTTON_EXTI_CHANNEL        (2)
/* Polling rate adjustment button */
#define MOUSE_POOL_BUTTON_GPIO_ID             (GPIO_PB)
#define MOUSE_POOL_BUTTON_GPIO_PORT           (HT_GPIOB)
#define MOUSE_POOL_BUTTON_GPIO_PIN            (GPIO_PIN_8)
#define MOUSE_POOL_BUTTON_AFIO_MODE           (AFIO_FUN_GPIO)
#define MOUSE_POOL_BUTTON_EXTI_CHANNEL        (8)

/* Encoder GPIO pin */
/* Encoder-A Wheel */
#define ENC_A_GPIO_ID                         (GPIO_PC)
#define ENC_A_GPIO_PIN                        (GPIO_PIN_4)
#define ENC_A_AFIO_MODE                       (AFIO_FUN_GPIO)
#define ENC_A_EXTI_CHANNEL                    (4)
/* Encoder-B Wheel */
#define ENC_B_GPIO_ID                         (GPIO_PC)
#define ENC_B_GPIO_PIN                        (GPIO_PIN_5)
#define ENC_B_AFIO_MODE                       (AFIO_FUN_GPIO)
#define ENC_B_EXTI_CHANNEL                    (5)

/**
  * Mouse sensor pin connection (PMW3336/PMW3360)
  */
#define SENSOR_SPI_CLK(CK)                    (CK.Bit.SPI0)
#define SENSOR_SPI                            (HT_SPI0)
/* SENSOR_SCLK <--> PA4 */
#define SENSOR_SPI_SCK_GPIO_ID                (GPIO_PA)
#define SENSOR_SPI_SCK_GPIO_PORT              (HT_GPIOA)
#define SENSOR_SPI_SCK_AFIO_PIN               (AFIO_PIN_4)
#define SENSOR_SPI_SCK_AFIO_MODE              (AFIO_FUN_SPI)
/* SENSOR_MOSI <--> PA5 */
#define SENSOR_SPI_MOSI_GPIO_ID               (GPIO_PA)
#define SENSOR_SPI_MOSI_GPIO_PORT             (HT_GPIOA)
#define SENSOR_SPI_MOSI_AFIO_PIN              (AFIO_PIN_5)
#define SENSOR_SPI_MOSI_AFIO_MODE             (AFIO_FUN_SPI)
/* SENSOR_MISO <--> PA6 */
#define SENSOR_SPI_MISO_GPIO_ID               (GPIO_PA)
#define SENSOR_SPI_MISO_GPIO_PORT             (HT_GPIOA)
#define SENSOR_SPI_MISO_AFIO_PIN              (AFIO_PIN_6)
#define SENSOR_SPI_MISO_AFIO_MODE             (AFIO_FUN_SPI)
/* SENSOR_CS <--> PA7 */
#define SENSOR_SPI_SEL_GPIO_ID                (GPIO_PA)
#define SENSOR_SPI_SEL_GPIO_PORT              (HT_GPIOA)
#define SENSOR_SPI_SEL_GPIO_PIN               (GPIO_PIN_7)
#define SENSOR_SPI_SEL_AFIO_MODE              (AFIO_FUN_GPIO)
#define SENSOR_SPI_SEL_CLK(CK)                (CK.Bit.PA)
/* SENSOR MOTION <--> PA2 */
#define SENSOR_MOTION_GPIO_ID                 (GPIO_PA)
#define SENSOR_MOTION_GPIO_PORT               (HT_GPIOA)
#define SENSOR_MOTION_GPIO_PIN                (GPIO_PIN_2)
#define SENSOR_MOTION_AFIO_MODE               (AFIO_FUN_GPIO)
#define SENSOR_MOTION_CLK(CK)                 (CK.Bit.PA)
/* SENSOR RST <--> PA3 */
#define SENSOR_RESET_GPIO_ID                  (GPIO_PA)
#define SENSOR_RESET_GPIO_PORT                (HT_GPIOA)
#define SENSOR_RESET_GPIO_PIN                 (GPIO_PIN_3)
#define SENSOR_RESET_AFIO_MODE                (AFIO_FUN_GPIO)
#define SENSOR_RESET_SEL_CLK(CK)              (CK.Bit.PA)

/**
  * UART debug
  */
#define COM_NUM                       (1)

#define COM1_CLK(CK)                  (CK.Bit.UART1)
#define COM1_PORT                     (HT_UART1)
#define COM1_IRQn                     (UART1_IRQn)
#define COM1_IRQHandler               (UART1_IRQHandler)
/* U1_TX <--> PA0 */
#define COM1_TX_GPIO_ID               (GPIO_PA)
#define COM1_TX_AFIO_PIN              (AFIO_PIN_0)
#define COM1_TX_AFIO_MODE             (AFIO_FUN_USART_UART)
/* U1_RX <--> PA1 */
#define COM1_RX_GPIO_ID               (GPIO_PA)
#define COM1_RX_AFIO_PIN              (AFIO_PIN_1)
#define COM1_RX_AFIO_MODE             (AFIO_FUN_USART_UART)

/**
  * AWINIC Led driver
  */
#define LED_DRV_I2C_CLK(CK)           (CK.Bit.I2C1)
#define LED_DRV_I2C                   (HT_I2C1)
/* I2C1_SCL <--> PB15 */
#define LED_DRV_I2C_SCL_GPIO_ID       (GPIO_PB)
#define LED_DRV_I2C_SCL_AFIO_PIN      (AFIO_PIN_15)
#define LED_DRV_I2C_SCL_AFIO_MODE     (AFIO_FUN_I2C)
/* I2C1_SDA <--> PC0 */
#define LED_DRV_I2C_SDA_GPIO_ID       (GPIO_PC)
#define LED_DRV_I2C_SDA_AFIO_PIN      (AFIO_PIN_0)
#define LED_DRV_I2C_SDA_AFIO_MODE     (AFIO_FUN_I2C)
/* CHIP_ENABLE <--> PC10 */
#define LED_DRV_ENABLE_GPIO_ID        (GPIO_PC)
#define LED_DRV_ENABLE_GPIO_PORT      (HT_GPIOC)
#define LED_DRV_ENABLE_GPIO_PIN       (GPIO_PIN_10)
#define LED_DRV_ENABLE_AFIO_MODE      (AFIO_FUN_GPIO)
#define LED_DRV_ENABLE_SEL_CLK(CK)    (CK.Bit.PC)

/* Exported functions --------------------------------------------------------------------------------------*/
/** @defgroup HT32F52341_SK_Exported_Functions HT32F52341 Starter kit exported functions
  * @{
  */
void  board_led_init(void);
void  board_button_init_all(void);
u32   board_button_read(bsp_btn_id_t BUTTON_x);
void  board_encoder_init(void);
void  board_debug_init(COM_TypeDef COMn, USART_InitTypeDef* USART_InitStructure);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
}
#endif

#endif
