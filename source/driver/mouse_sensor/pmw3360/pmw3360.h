/*********************************************************************************************************//**
 * @file    spi_lcd.h
 * @version $Rev:: 5105         $
 * @date    $Date:: 2020-12-10 #$
 * @brief   The header file of spi_lcd.c module.
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
#ifndef __PMW3336_H__
#define __PMW3336_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32_board.h"
#include "pmw3360_reg.h"

/* Exported types ------------------------------------------------------------------------------------------*/
typedef struct {
    uint8_t motion;
    uint8_t observation;
    int16_t deltaX;
    int16_t deltaY;
    uint8_t squal;
} motion_burst_t;

typedef struct {
    int16_t dx;
    int16_t dy;
} deltas_t;

typedef enum
{
  MOUSE_RESOLUTION_400DPI  = 1, /*!< 400  dpi resolution */
  MOUSE_RESOLUTION_800DPI  = 2, /*!< 800  dpi resolution */
  MOUSE_RESOLUTION_1600DPI = 0, /*!< 1600 dpi resolution (Default) */
  MOUSE_RESOLUTION_3200DPI = 3, /*!< 3200 dpi resolution */
} mouse_resolution_t;

/* Exported constants --------------------------------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------------------------------------*/
// #if (SENSOR_SPI_SEL_AFIO_MODE == AFIO_FUN_GPIO)
//   #define SENSOR_SEL_CMD_INACTIVE()    GPIO_SetOutBits(GPIO_PORT[SENSOR_SPI_SEL_GPIO_ID], SENSOR_SPI_SEL_GPIO_PIN)
//   #define SENSOR_SEL_CMD_ACTIVE()      GPIO_ClearOutBits(GPIO_PORT[SENSOR_SPI_SEL_GPIO_ID], SENSOR_SPI_SEL_GPIO_PIN)
// #else
//   #define SENSOR_SEL_CMD_INACTIVE()    SPI_SoftwareSELCmd(SENSOR_SPI, SPI_SEL_INACTIVE)
//   #define SENSOR_SEL_CMD_ACTIVE()      SPI_SoftwareSELCmd(SENSOR_SPI, SPI_SEL_ACTIVE)
// #endif


/* Exported functions --------------------------------------------------------------------------------------*/
uint8_t drv_mouse_sensor_init(void);
uint8_t drv_mouse_product_id_read(void);
uint8_t drv_mouse_sensor_read(int16_t* delta_x, int16_t* delta_y);
uint8_t drv_mouse_set_cpi(uint16_t cpi);
uint16_t drv_mouse_get_cpi();
motion_burst_t pmw3360_read_motion_burst(void);
#ifdef __cplusplus
}
#endif

#endif
