/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __IS31FL3246_H__
#define __IS31FL3246_H__

/* Includes ------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------------------------------------*/
// This is a 7-bit address, that gets left-shifted and bit 0
// set to 0 for write, 1 for read (as per I2C protocol)
// The address will vary depending on your wiring:
// 00 <-> GND
// 01 <-> SCL
// 10 <-> SDA
// 11 <-> VCC
// ADDR1 represents A1:A0 of the 7-bit address.
// ADDR2 represents A3:A2 of the 7-bit address.
// The result is: 0b101(ADDR2)(ADDR1)
#define DRIVER_I2C_ADDRESS              (0x30U)

#define IS31_REG_CTRL                   0x00        /* Control register */
#define IS31_REG_HFP_BASE               0x01        /* PWM frequency H register */
#define IS31_REG_LFP_BASE               0x49        /* PWM frequency L register */

#define IS31_REG_UPDATE                 0x6D        /* Update LFP & HFP data register */

/* Current control */
#define IS31_REG_GCG                    0x6E        /* Global current for Green channels Control register */
#define IS31_REG_GCR                    0x6F        /* Global current for Red channels Control register */
#define IS31_REG_GCB                    0x70        /* Global current for Blue channels Control register */

#define IS31_REG_DLY                    0x71        /* Phase Delay and Clock Phase Control Register */
#define IS31_REG_RESET                  0x7F        /* Reset all register */

/* FMS PWM Frequency Mode Select */
#define IS31_FMS_HFP_LFP    (0U << 2)   /* HFP + LFP */
#define IS31_FMS_HFP_ONLY   (1U << 2)   /* Only HFP, LFP=256 */
#define IS31_FMS_DC_MODE    (2U << 2)   /* DC Mode, no PWM, output always on */
#define IS31_FMS_SHUTDOWN   (3U << 2)   /* Channel Shutdown mode */

/* Exported macro ------------------------------------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------------------------------------*/
void is31fl3246_init(void);
void is31fl3246_set_led_all(uint8_t red, uint8_t green, uint8_t blue);
void is31fl3246_set_color(uint8_t rgb_index, uint8_t red, uint8_t green, uint8_t blue);
void is31fl3246_commit(void);
#endif /* __IS31FL3246_H__ */