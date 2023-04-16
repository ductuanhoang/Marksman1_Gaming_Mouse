/*********************************************************************************************************//**
 * @file    spi_lcd.c
 * @version $Rev:: 5551         $
 * @date    $Date:: 2021-08-04 #$
 * @brief   This file provides a set of functions needed to manage the
 *          communication between SPI peripheral and LCD.
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
#include "common.h"
#include "pmw3360.h"
#include "pmw3360_reg.h"
/* Private constants ---------------------------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------------------------------------*/
extern const uint8_t pmw33660_firmware_data[4094];

/* Private function prototypes -----------------------------------------------------------------------------*/
static void     _pmw3360_spi_init(void);

static void     _pmw3360_select(void);
static void     _pmw3360_unselect(void);
static uint8_t  _pmw3360_spi_transfer(uint8_t byte);
static uint8_t  _pmw3360_read(uint8_t address);
static void     _pmw3360_write(uint8_t address, uint8_t value);

static void     _pmw3360_delay_us(uint16_t usec);
static void     _pmw3360_delay_ms(uint16_t msec);
// static motion_burst_t pmw3360_read_motion_burst(void);
static void     _pmw3360_hard_reset(void);
static uint8_t  _pmw3360_load_fw(void);

/* Global functions ----------------------------------------------------------------------------------------*/

/**
 * @brief Initialize the PMW3360 mouse sensor
 *
 * @return uint8_t
 */
uint8_t drv_mouse_sensor_init(void)
{
  /* Initialize the SPI bus */
  _pmw3360_spi_init();
  _pmw3360_hard_reset();

  _pmw3360_unselect();
  _pmw3360_delay_ms(1);

  uint8_t srom_id = _pmw3360_load_fw();
  printf("[PMW3360] Fw %x loaded", srom_id);

   _pmw3360_delay_ms(100);

  /* Set the default CPI to 2400 */
  // drv_mouse_set_cpi(12000);
  // test
  // drv_mouse_use_difference_dip(true);
  // drv_mouse_set_cpiY(2400);
  // drv_mouse_set_cpiX(12000);
  return srom_id;
}

uint8_t drv_mouse_product_id_read(void)
{
  return _pmw3360_read(PMW3360_REG_PID);
}

uint8_t drv_mouse_sensor_read(int16_t* delta_x, int16_t* delta_y)
{
  uint8_t motion = 0;
  uint8_t dx_l = 0;
  uint8_t dx_h = 0;
  uint8_t dy_l = 0;
  uint8_t dy_h = 0;

  motion = _pmw3360_read(PMW3360_REG_MOTION);
  dx_l   = _pmw3360_read(PMW3360_REG_DELTA_X_L);
  dx_h   = _pmw3360_read(PMW3360_REG_DELTA_X_H);
  dy_l   = _pmw3360_read(PMW3360_REG_DELTA_Y_L);
  dy_h   = _pmw3360_read(PMW3360_REG_DELTA_Y_H);

  *delta_x = (int16_t)((uint16_t)(dx_h << 8) | dx_l);
  *delta_y = (int16_t)((uint16_t)(dy_h << 8) | dy_l);

  return (motion & 0x80);
}

motion_burst_t pmw3360_read_motion_burst(void)
{
    motion_burst_t motionBurst = {0};

    uint8_t buf[6];
    uint8_t* buf_ptr = buf;

    _pmw3360_select();

    _pmw3360_spi_transfer(PMW3360_REG_MOT_BURST); // 7 bit address + read bit(0)
    // _pmw3360_delay_us(35); // Tsrad_motbr
    for(uint8_t i = 0; i < 6; i++)
        *buf_ptr++ = _pmw3360_spi_transfer(0x00);

    _pmw3360_unselect();

    motionBurst.motion = buf[0];
    motionBurst.observation = buf[1];
    motionBurst.deltaX |= buf[2];
    motionBurst.deltaX |= (buf[3] << 8);
    motionBurst.deltaY |= buf[4];
    motionBurst.deltaY |= (buf[5] << 8);

    // _pmw3360_delay_us(160);

    return motionBurst;
}
uint8_t drv_mouse_use_difference_dip(bool _enable)
{
  if(_enable == true)
  {
    // set normal set CPI
    _pmw3360_write(PMW3360_REG_CONFIG2, 0x04);
  }
  else {
    // : CPI setting for delta Y is defined by Config1 (address 0x0F). CPI setting for deltaX is  defined by Config5 (address 0x2F)
     _pmw3360_write(PMW3360_REG_CONFIG2, 0x00);
  }
}

uint8_t drv_mouse_set_cpi(uint16_t cpi)
{
  if (cpi > 10800) {
    return 1;
  }
  _pmw3360_write(PMW3360_REG_CONFIG1, PMW3360_CPI(cpi));
  return 0;
}

// add new config dpi

uint8_t drv_mouse_set_cpiX(uint16_t cpi)
{
  if (cpi > 10800) {
    return 1;
  }
  _pmw3360_write(PMW3360_REG_CONFIG5, PMW3360_CPI(cpi));
  return 0;
}

uint8_t drv_mouse_set_cpiY(uint16_t cpi)
{
  if (cpi > 10800) {
    return 1;
  }
  _pmw3360_write(PMW3360_REG_CONFIG1, PMW3360_CPI(cpi));
  return 0;
}
uint16_t drv_mouse_get_cpi()
{
  uint8_t reg = _pmw3360_read(PMW3360_REG_CONFIG1);

  return (uint16_t)(reg + 1) * 100 ;
}

/* Private functions ---------------------------------------------------------------------------------------*/
/**
 * @brief Delay microseconds
 *
 * @param usec
 */
static void _pmw3360_delay_us(uint16_t usec)
{
  // uint32_t i;

  // for (i = 0; i < 50 * usec; i++)
  // {
  // }

  Time_Delay(TIME_US2TICK(usec));
}

/**
 * @brief Delay miliseconds
 *
 * @param msec
 */
static void _pmw3360_delay_ms(uint16_t msec)
{
  // uint16_t i;

  // for (i = 0; i < msec; i++)
  // {
  //   _pmw3360_delay_us(1000);
  // }
  Time_Delay(TIME_MS2TICK(msec));
}

/**
 * @brief Active the SPI CS (CS = LOW)
 *
 */
static void _pmw3360_select(void)
{
  // SENSOR_SEL_CMD_ACTIVE();

#if (SENSOR_SPI_SEL_AFIO_MODE == AFIO_FUN_GPIO)
  GPIO_ClearOutBits(SENSOR_SPI_SEL_GPIO_PORT, SENSOR_SPI_SEL_GPIO_PIN);
#else
  SPI_SoftwareSELCmd(SENSOR_SPI, SPI_SEL_ACTIVE);
#endif

}

/**
 * @brief Unactive SPI CS line (CS = High)
 *
 */
static void _pmw3360_unselect(void)
{
  // SENSOR_SEL_CMD_INACTIVE();

#if (SENSOR_SPI_SEL_AFIO_MODE == AFIO_FUN_GPIO)
  GPIO_SetOutBits(SENSOR_SPI_SEL_GPIO_PORT, SENSOR_SPI_SEL_GPIO_PIN);
#else
  SPI_SoftwareSELCmd(SENSOR_SPI, SPI_SEL_INACTIVE);
#endif
}

/**
 * @brief Transfer and read a byte to/from SPI bus
 *
 * @param byte
 * @return uint8_t
 */
static uint8_t _pmw3360_spi_transfer(uint8_t byte)
{
  /* Loop while Tx buffer register is empty                                                                 */
  while (!SPI_GetFlagStatus(SENSOR_SPI, SPI_FLAG_TXBE));

  /* Send byte through the SPIx peripheral                                                                  */
  SPI_SendData(SENSOR_SPI, byte);

  /* Loop while Rx is not empty                                                                             */
  while (!SPI_GetFlagStatus(SENSOR_SPI, SPI_FLAG_RXBNE));

  /* Return the byte read from the SPI                                                                      */
  return (uint8_t)SPI_ReceiveData(SENSOR_SPI);
}

/**
 * @brief Read register value
 *
 * @param address
 * @return uint8_t
 */
static uint8_t _pmw3360_read(uint8_t address)
{
    uint8_t value;

    _pmw3360_select();

    _pmw3360_spi_transfer(address & 0x7F); // 7 bit address + read bit(0)
    _pmw3360_delay_us(50); // Tsrad
    value = _pmw3360_spi_transfer(0xFF);

    _pmw3360_unselect();
    _pmw3360_delay_us(160);
    return value;
}

/**
 * @brief Write a value to PMW3360 register
 *
 * @param address
 * @param value
 */
static void _pmw3360_write(uint8_t address, uint8_t value)
{
    _pmw3360_select();

    _pmw3360_spi_transfer((address & 0x7F) | 0x80); // 7 bit address + write bit(1)
    _pmw3360_delay_us(40); // Tsrad
    _pmw3360_spi_transfer(value);

    _pmw3360_unselect();
    _pmw3360_delay_us(160);
}

/**
 * @brief Initialize the SPI bus
 *
 */
static void _pmw3360_spi_init(void)
{

  SPI_InitTypeDef  SPI_InitStructure;

  /*  Enable AFIO clock                                                                                     */
  /*  Enable EBI & EBI CS port clock                                                                        */
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  SENSOR_SPI_SEL_CLK(CKCUClock)   = 1;
  SENSOR_SPI_CLK(CKCUClock)       = 1;
  SENSOR_RESET_SEL_CLK(CKCUClock) = 1;
  CKCUClock.Bit.AFIO              = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

#if (USE_HT32F50343) || (USE_HT32F65240_SK)
  // GPIO_DriveConfig(GPIO_PORT[SENSOR_SPI_SEL_GPIO_ID], SENSOR_SPI_SEL_GPIO_PIN, GPIO_DV_8MA);
  // GPIO_DriveConfig(GPIO_PORT[SENSOR_SPI_SCK_GPIO_ID], SENSOR_SPI_SCK_AFIO_PIN, GPIO_DV_8MA);
  // GPIO_DriveConfig(GPIO_PORT[SENSOR_SPI_MOSI_GPIO_ID], SENSOR_SPI_MOSI_AFIO_PIN, GPIO_DV_8MA);

  GPIO_DriveConfig(SENSOR_SPI_SEL_GPIO_PORT, SENSOR_SPI_SEL_GPIO_PIN, GPIO_DV_12MA);
  GPIO_DriveConfig(SENSOR_SPI_SCK_GPIO_PORT, SENSOR_SPI_SCK_AFIO_PIN, GPIO_DV_12MA);
  GPIO_DriveConfig(SENSOR_SPI_MOSI_GPIO_PORT, SENSOR_SPI_MOSI_AFIO_PIN, GPIO_DV_12MA);
  GPIO_DriveConfig(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN, GPIO_DV_12MA);
#endif

  GPIO_OpenDrainConfig(SENSOR_SPI_SEL_GPIO_PORT, SENSOR_SPI_SEL_GPIO_PIN, DISABLE);
  GPIO_OpenDrainConfig(SENSOR_SPI_SCK_GPIO_PORT, SENSOR_SPI_SCK_AFIO_PIN, DISABLE);
  GPIO_OpenDrainConfig(SENSOR_SPI_MOSI_GPIO_PORT, SENSOR_SPI_MOSI_AFIO_PIN, DISABLE);
  GPIO_OpenDrainConfig(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN, DISABLE);

  /* Configure SPI pins, SEL, SCK, MISO ,MOSI, BL & RESET                                                   */
  /* SPI CS */
  AFIO_GPxConfig(SENSOR_SPI_SEL_GPIO_ID, SENSOR_SPI_SEL_GPIO_PIN, SENSOR_SPI_SEL_AFIO_MODE);
#if (SENSOR_SPI_SEL_AFIO_MODE == AFIO_FUN_GPIO)
  GPIO_SetOutBits(SENSOR_SPI_SEL_GPIO_PORT, SENSOR_SPI_SEL_GPIO_PIN);
  GPIO_DirectionConfig(SENSOR_SPI_SEL_GPIO_PORT, SENSOR_SPI_SEL_GPIO_PIN, GPIO_DIR_OUT);
#endif

  /* SPI SCK, MOSI, MISO */
  AFIO_GPxConfig(SENSOR_SPI_SCK_GPIO_ID, SENSOR_SPI_SCK_AFIO_PIN, SENSOR_SPI_SCK_AFIO_MODE);
  AFIO_GPxConfig(SENSOR_SPI_MISO_GPIO_ID, SENSOR_SPI_MISO_AFIO_PIN, SENSOR_SPI_MISO_AFIO_MODE);
  AFIO_GPxConfig(SENSOR_SPI_MOSI_GPIO_ID, SENSOR_SPI_MOSI_AFIO_PIN, SENSOR_SPI_MOSI_AFIO_MODE);

  /* Sensor RST */
  AFIO_GPxConfig(SENSOR_RESET_GPIO_ID, SENSOR_RESET_GPIO_PIN, SENSOR_RESET_AFIO_MODE);
#if (SENSOR_RESET_AFIO_MODE == AFIO_FUN_GPIO)
  GPIO_SetOutBits(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN);
  GPIO_DirectionConfig(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN, GPIO_DIR_OUT);
#endif

  /* Motion pin */
  AFIO_GPxConfig(SENSOR_RESET_GPIO_ID, SENSOR_RESET_GPIO_PIN, SENSOR_RESET_AFIO_MODE);
#if (SENSOR_RESET_AFIO_MODE == AFIO_FUN_GPIO)
  GPIO_SetOutBits(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN);
  GPIO_DirectionConfig(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN, GPIO_DIR_OUT);
#endif

  /* SPI Configuration                                                                                      */
  SPI_InitStructure.SPI_Mode = SPI_MASTER;
  SPI_InitStructure.SPI_FIFO = SPI_FIFO_DISABLE;
  SPI_InitStructure.SPI_DataLength = SPI_DATALENGTH_8;
  SPI_InitStructure.SPI_SELMode = SPI_SEL_SOFTWARE;
  SPI_InitStructure.SPI_SELPolarity = SPI_SELPOLARITY_LOW;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_SECOND;
  SPI_InitStructure.SPI_RxFIFOTriggerLevel = 0;
  SPI_InitStructure.SPI_TxFIFOTriggerLevel = 0;
  SPI_InitStructure.SPI_ClockPrescaler = 8;
  SPI_Init(SENSOR_SPI, &SPI_InitStructure);

  SPI_SELOutputCmd(SENSOR_SPI, ENABLE);

  SPI_Cmd(SENSOR_SPI, ENABLE);
}

static void _pmw3360_hard_reset(void)
{
  GPIO_ClearOutBits(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN);
  _pmw3360_delay_ms(10);
  GPIO_SetOutBits(SENSOR_RESET_GPIO_PORT, SENSOR_RESET_GPIO_PIN);
  _pmw3360_delay_ms(50);
}

static uint8_t  _pmw3360_load_fw(void)
{
  uint8_t* firmware = (uint8_t*)pmw33660_firmware_data;

  _pmw3360_write(PMW3360_REG_PWR_UP_RST, PMW3360_RESET_CMD);
  _pmw3360_delay_ms(50); // wait for it to reboot

  _pmw3360_read(PMW3360_REG_MOTION);
  _pmw3360_read(PMW3360_REG_DELTA_X_L);
  _pmw3360_read(PMW3360_REG_DELTA_X_H);
  _pmw3360_read(PMW3360_REG_DELTA_Y_L);
  _pmw3360_read(PMW3360_REG_DELTA_Y_H);

  uint8_t pid = 0;
  uint8_t i = 0;
  for( i = 0; i < 100; i ++) {
      pid =         _pmw3360_read(PMW3360_REG_PID);
      printf("[pmw3360] read pid = 0x%x", pid);
      if(pid != 0x42) // confirm comm link
      {
          printf("[pmw3360] unknown sensor (0x%x)!", pid);
          // return 0;
      } else {
          break;
      }
      _pmw3360_delay_ms(100);
  }
  if (i == 100) {
      printf("[pmw3360] Init failed!");
      return 0;
  }

  /* Download SROM */
  _pmw3360_write(PMW3360_REG_CONFIG2, 0x00); // clear REST enable bit

  _pmw3360_write(PMW3360_REG_SROM_EN, PMW3360_SROM_DWNLD_CMD); // initialize SROM download
  _pmw3360_delay_ms(15);

  _pmw3360_write(PMW3360_REG_SROM_EN, PMW3360_SROM_DWNLD_START_CMD); // start SROM download

  _pmw3360_select();

  _pmw3360_spi_transfer(PMW3360_REG_SROM_BURST | 0x80); // 7 bit srom_burst address + write bit(1)
  _pmw3360_delay_us(15);

  for(uint16_t b = 0; b < 4094; b++) // write firmware image (4094 bytes)
  {
      _pmw3360_spi_transfer(*firmware++);
      _pmw3360_delay_us(15);
  }

  _pmw3360_unselect();

  _pmw3360_delay_ms(2); // Tbexit

  uint8_t sromId = _pmw3360_read(PMW3360_REG_SROM_ID); // read srom firmware id

  if(!sromId) {
      printf("[pmw3360] unknown srom id!");
      return 0;
  }

  _pmw3360_write(PMW3360_REG_SROM_EN, PMW3360_SROM_CRC_CMD); // initialize CRC check
  _pmw3360_delay_ms(15);

  volatile uint16_t sromCrc = 0;

  sromCrc |= (_pmw3360_read(PMW3360_REG_DOUT_H) << 8);
  sromCrc |= _pmw3360_read(PMW3360_REG_DOUT_L);

  if(!sromCrc) //TODO: check CRC
      return 0;

  if(!(_pmw3360_read(PMW3360_REG_OBSERVATION) & PMW3360_SROM_RUN)) // check SROM running bit
      return 0;

  _pmw3360_write(PMW3360_REG_CONFIG2, 0x00); // clear REST enable bit

  _pmw3360_write(PMW3360_REG_MOT_BURST, 0x01);

  _pmw3360_delay_ms(500);

  pmw3360_read_motion_burst();

  _pmw3360_write(PMW3360_REG_OBSERVATION, 0x00); // clear observation register

  pmw3360_read_motion_burst();

  return sromId;
}
