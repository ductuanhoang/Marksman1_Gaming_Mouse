/* Define to prevent recursive inclusion -------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "is31fl3246.h"

/* Private constants ---------------------------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------------------------------------*/
static uint8_t _led_pwm_buffer[36] = {0};
static uint8_t _led_pwm_buffer_changed[36] = {0};

/* Private function prototypes -----------------------------------------------------------------------------*/
static void _is31fl3246_delay_ms(uint16_t msec);
static void _is31fl3246_ack_polling(uint8_t dev_addr);
static void _is31fl3246_write_register(uint8_t dev_addr, uint8_t reg, uint8_t data);
static void _is31fl3246_init_ll(void);

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Initialize led driver.
  * @retval None
  ***********************************************************************************************************/
void is31fl3246_init(void)
{

    _is31fl3246_init_ll();

    // Reset the Driver IC
    _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_RESET, 0x00);

    // Wait 10ms to ensure the device has woken up.
    _is31fl3246_delay_ms(10);

    // Current settings
    _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_GCG, 0xFF); // set red current maximum
    _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_GCR, 0xFF); // set green current maximum
    _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_GCB, 0xFF); // set blue current maximum

    /* Mode settings
        * 0b01000001 (0x01)
        *    | |  ||
        *    | |  |+-> SSD = 1:    Normal mode
        *    | |  +--> PMS = 0:    PWM 8 bit mode
        *    | +-----> HFPS = 00:  32kHz
        *    +-------> RGBM = 0:   36 leds mode
    */
    _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_CTRL, 0x01);

    // Channel settings
    for (uint8_t ii = 0; ii < 36; ii++) {
        /* HFP settings */
        _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_HFP_BASE + (ii * 2), 0x00);                                  /* HFP_L */
        _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_HFP_BASE + (ii * 2) + 1, (IS31_FMS_HFP_ONLY) | 0x00);        /* HFP_H */

        /* LFP settings */
        _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_LFP_BASE + ii, 0x00);        /* LFP */
    }

    /* Update changes */
    _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_UPDATE, 0x00);

    // Wait 10ms to ensure the device has woken up.
    _is31fl3246_delay_ms(10);

}

/*********************************************************************************************************//**
  * @brief  Set all led brightness level
  * @param  level
  * @retval None
  ***********************************************************************************************************/
void is31fl3246_set_color(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (led_index >= 12) {
      return;
    }

    _led_pwm_buffer_changed[led_index*3]      = (uint8_t)(red   != _led_pwm_buffer[led_index*3]);
    _led_pwm_buffer_changed[led_index*3 + 1]  = (uint8_t)(green != _led_pwm_buffer[led_index*3 + 1]);
    _led_pwm_buffer_changed[led_index*3 + 2]  = (uint8_t)(blue  != _led_pwm_buffer[led_index*3 + 2]);

    _led_pwm_buffer[led_index*3]      = red;
    _led_pwm_buffer[led_index*3 + 1]  = green;
    _led_pwm_buffer[led_index*3 + 2]  = blue;
}

void is31fl3246_set_led_all(uint8_t r, uint8_t g, uint8_t b)
{
  for (uint8_t ii = 0; ii < 12; ii++) {
    is31fl3246_set_color(ii, r, g, b);
  }
}

void is31fl3246_commit(void)
{
    // Channel settings
    for (uint8_t ii = 0; ii < 36; ii++) {
      if (_led_pwm_buffer_changed[ii] == 1) {
          /* HFP settings */
          _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_HFP_BASE + (ii * 2), _led_pwm_buffer[ii]);
          _led_pwm_buffer_changed[ii] = 0;
      }
    }

    /* Update changes */
    _is31fl3246_write_register(DRIVER_I2C_ADDRESS, IS31_REG_UPDATE, 0x00);
}
/* Private functions ---------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Delay miliseconds.
  * @retval None
  ***********************************************************************************************************/
static void _is31fl3246_delay_ms(uint16_t msec)
{
    Time_Delay(TIME_MS2TICK(msec));
}

/*********************************************************************************************************//**
  * @brief  Led driver acknowledge polling.
  * @retval None
  ***********************************************************************************************************/
static void _is31fl3246_ack_polling(uint8_t dev_addr)
{
  u32 reg;

  /* wait if bus busy                                                                                       */
  while (I2C_GetFlagStatus(LED_DRV_I2C, I2C_FLAG_BUSBUSY));

  while (1)
  {
    /* send slave address                                                                                   */
    I2C_TargetAddressConfig(LED_DRV_I2C, dev_addr, I2C_MASTER_WRITE);

    /* check status                                                                                         */
    while (1)
    {
      reg = LED_DRV_I2C->SR;

      if (reg & I2C_FLAG_ADRS)
      {
        return;
      }

      if (reg & I2C_FLAG_RXNACK)
      {
        I2C_ClearFlag(LED_DRV_I2C, I2C_FLAG_RXNACK);
        break;
      }
    }
  }
}

static void _is31fl3246_write_register(uint8_t dev_addr, uint8_t reg, uint8_t data)
{
    // i2c_transmit(addr << 1, g_twi_transfer_buffer, 2, IS31_TIMEOUT);

    /* acknowledge polling                                                                                    */
    _is31fl3246_ack_polling(dev_addr);

    /* set address                                                                                            */
    while (!I2C_CheckStatus(LED_DRV_I2C, I2C_MASTER_TX_EMPTY));
    I2C_SendData(LED_DRV_I2C, reg);

    /* write data                                                                                             */
    while (!I2C_CheckStatus(LED_DRV_I2C, I2C_MASTER_TX_EMPTY));
    I2C_SendData(LED_DRV_I2C, data);

    /* end of write                                                                                           */
    while (!I2C_CheckStatus(LED_DRV_I2C, I2C_MASTER_TX_EMPTY));
    I2C_GenerateSTOP(LED_DRV_I2C);

}

static void _is31fl3246_init_ll(void)
{
    /* !!! NOTICE !!!
        Notice that the local variable (structure) did not have an initial value.
        Please confirm that there are no missing members in the parameter settings below in this function.
    */
    I2C_InitTypeDef I2C_InitStructure;

    /* Enable AFIO & I2C clock                                                                                */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    LED_DRV_I2C_CLK(CKCUClock) = 1;
    /* Enable the GPIO Clock                                                                                  */
    LED_DRV_ENABLE_SEL_CLK(CKCUClock) = 1;
    CKCUClock.Bit.AFIO        = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);

    /* Configure LED_DRV_ENABLE pin as output                                                                 */
    AFIO_GPxConfig(LED_DRV_ENABLE_GPIO_ID, LED_DRV_ENABLE_GPIO_PIN, LED_DRV_ENABLE_AFIO_MODE);
    GPIO_SetOutBits(LED_DRV_ENABLE_GPIO_PORT, LED_DRV_ENABLE_GPIO_PIN);
    GPIO_DirectionConfig(LED_DRV_ENABLE_GPIO_PORT, LED_DRV_ENABLE_GPIO_PIN, GPIO_DIR_OUT);

    /* Configure I2C SCL pin, I2C SDA pin                                                                     */
    AFIO_GPxConfig(LED_DRV_I2C_SCL_GPIO_ID, LED_DRV_I2C_SCL_AFIO_PIN, LED_DRV_I2C_SCL_AFIO_MODE);
    AFIO_GPxConfig(LED_DRV_I2C_SDA_GPIO_ID, LED_DRV_I2C_SDA_AFIO_PIN, LED_DRV_I2C_SDA_AFIO_MODE);

    /* I2C configuration                                                                                      */
    I2C_InitStructure.I2C_GeneralCall = I2C_GENERALCALL_DISABLE;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;
    I2C_InitStructure.I2C_Acknowledge = I2C_ACK_DISABLE;
    I2C_InitStructure.I2C_OwnAddress = 0x00;
    I2C_InitStructure.I2C_Speed = 400000;
    I2C_InitStructure.I2C_SpeedOffset = 0;
    I2C_Init(LED_DRV_I2C, &I2C_InitStructure);

    I2C_Cmd(LED_DRV_I2C, ENABLE);
}
