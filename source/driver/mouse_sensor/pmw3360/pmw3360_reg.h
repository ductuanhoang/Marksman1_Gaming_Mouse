#ifndef __PMW3360_H__
#define __PMW3360_H__

//--------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------

// general registers
#define PMW3360_REG_PID           0x00
#define PMW3360_REG_REV           0x01
#define PMW3360_REG_INV_PID       0x3F


#define PMW3360_REG_PWR_UP_RST    0x3A
#define PMW3360_REG_SHUT_DWN      0x3B

// motion registers
#define PMW3360_REG_MOTION        0x02
#define PMW3360_REG_DELTA_X_L     0x03
#define PMW3360_REG_DELTA_X_H     0x04
#define PMW3360_REG_DELTA_Y_L     0x05
#define PMW3360_REG_DELTA_Y_H     0x06
#define PMW3360_REG_MOT_BURST     0x50

// srom registers
#define PMW3360_REG_SROM_EN       0x13
#define PMW3360_REG_SROM_ID       0x2A
#define PMW3360_REG_SROM_BURST    0x62

// config registers
#define PMW3360_REG_CONFIG1       0x0F
#define PMW3360_REG_CONFIG2       0x10
#define PMW3360_REG_OBSERVATION   0x24
#define PMW3360_REG_DOUT_L        0x25
#define PMW3360_REG_DOUT_H        0x26

// motion bits
#define PMW3360_MOTION            0x80
#define PMW3360_OPMODE_RUN        0x00
#define PMW3360_OPMODE_REST1      0x02
#define PMW3360_OPMODE_REST2      0x04
#define PMW3360_OPMODE_REST3      0x06

// config2 bits
#define PMW3360_RESTEN            0x20
#define PMW3360_RPT_MOD           0x04       // XY CPI reporting mode (different cpis)

// observation bits
#define PMW3360_SROM_RUN          0x40

// srom cmds
#define PMW3360_SROM_CRC_CMD          0x15
#define PMW3360_SROM_DWNLD_CMD        0x1D
#define PMW3360_SROM_DWNLD_START_CMD  0x18

// power up reset cmd
#define PMW3360_RESET_CMD         0x5A

#define PMW3360_SHUTDOWN_CMD         0xB6

// cpis
#define PMW3360_CPI(cpi)          ((cpi / 100) - 1)   // 100-12000, 100 cpi LSb
#define PMW3360_CPI_DEFAULT       0x31                // 5000 cpi
#define PMW3360_CPI_MAX           0x77                // 12000 cpi
#define PMW3360_CPI_MIN           0x00                // 100 cpi

// product unique identifier
#define PMW3360_PID   0x42

//--------------------------------------------------------------------
// Target Porting API
//--------------------------------------------------------------------

#endif // __PMW3360_H__
