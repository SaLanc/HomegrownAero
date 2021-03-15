#include "util.h"

#define DATA_FORMAT   0x31  // data format register address
#define DATA_FORMAT_B 0x0B  // data format bytes: +/- 16g range, 13-bit resolution (p. 26 of ADXL345 datasheet)
#define READ_BIT      0x80
#define MULTI_BIT     0x40
#define BW_RATE       0x2C
#define POWER_CTL     0x2D
#define DATAX0        0x32

#define ADXL345_MG2G_MULTIPLIER (0.0039)  // 4mg per lsb
/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL345_REG_DEVID               0x00    // Device ID
#define ADXL345_REG_THRESH_TAP          (0x1D)    // Tap threshold
#define ADXL345_REG_OFSX                (0x1E)    // X-axis offset
#define ADXL345_REG_OFSY                (0x1F)    // Y-axis offset
#define ADXL345_REG_OFSZ                (0x20)    // Z-axis offset
#define ADXL345_REG_DUR                 (0x21)    // Tap duration
#define ADXL345_REG_LATENT              (0x22)    // Tap latency
#define ADXL345_REG_WINDOW              (0x23)    // Tap window
#define ADXL345_REG_THRESH_ACT          (0x24)    // Activity threshold
#define ADXL345_REG_THRESH_INACT        (0x25)    // Inactivity threshold
#define ADXL345_REG_TIME_INACT          (0x26)    // Inactivity time
#define ADXL345_REG_ACT_INACT_CTL       (0x27)    // Axis enable control for activity and inactivity detection
#define ADXL345_REG_THRESH_FF           (0x28)    // Free-fall threshold
#define ADXL345_REG_TIME_FF             (0x29)    // Free-fall time
#define ADXL345_REG_TAP_AXES            (0x2A)    // Axis control for single/double tap
#define ADXL345_REG_ACT_TAP_STATUS      (0x2B)    // Source for single/double tap
#define ADXL345_REG_BW_RATE             0x2C    // Data rate and power mode control
#define ADXL345_REG_POWER_CTL           0x2D    // Power-saving features control
#define ADXL345_REG_INT_ENABLE          (0x2E)    // Interrupt enable control
#define ADXL345_REG_INT_MAP             (0x2F)    // Interrupt mapping control
#define ADXL345_REG_INT_SOURCE          (0x30)    // Source of interrupts
#define ADXL345_REG_DATA_FORMAT         (0x31)    // Data format control
#define ADXL345_REG_DATAX0              0x32    // X-axis data 0
#define ADXL345_REG_DATAX1              (0x33)    // X-axis data 1
#define ADXL345_REG_DATAY0              (0x34)    // Y-axis data 0
#define ADXL345_REG_DATAY1              (0x35)    // Y-axis data 1
#define ADXL345_REG_DATAZ0              (0x36)    // Z-axis data 0
#define ADXL345_REG_DATAZ1              (0x37)    // Z-axis data 1
#define ADXL345_REG_FIFO_CTL            (0x38)    // FIFO control
#define ADXL345_REG_FIFO_STATUS         (0x39)    // FIFO status
/*=========================================================================*/

/* Used with register 0x2C (ADXL345_REG_BW_RATE) to set bandwidth */
typedef enum {
    ADXL345_DATARATE_3200_HZ    = 0b1111, // 1600Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_1600_HZ    = 0b1110, //  800Hz Bandwidth    90�A IDD
    ADXL345_DATARATE_800_HZ     = 0b1101, //  400Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_400_HZ     = 0b1100, //  200Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_200_HZ     = 0b1011, //  100Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_100_HZ     = 0b1010, //   50Hz Bandwidth   140�A IDD
    ADXL345_DATARATE_50_HZ      = 0b1001, //   25Hz Bandwidth    90�A IDD
    ADXL345_DATARATE_25_HZ      = 0b1000, // 12.5Hz Bandwidth    60�A IDD
    ADXL345_DATARATE_12_5_HZ    = 0b0111, // 6.25Hz Bandwidth    50�A IDD
    ADXL345_DATARATE_6_25HZ     = 0b0110, // 3.13Hz Bandwidth    45�A IDD
    ADXL345_DATARATE_3_13_HZ    = 0b0101, // 1.56Hz Bandwidth    40�A IDD
    ADXL345_DATARATE_1_56_HZ    = 0b0100, // 0.78Hz Bandwidth    34�A IDD
    ADXL345_DATARATE_0_78_HZ    = 0b0011, // 0.39Hz Bandwidth    23�A IDD
    ADXL345_DATARATE_0_39_HZ    = 0b0010, // 0.20Hz Bandwidth    23�A IDD
    ADXL345_DATARATE_0_20_HZ    = 0b0001, // 0.10Hz Bandwidth    23�A IDD
    ADXL345_DATARATE_0_10_HZ    = 0b0000  // 0.05Hz Bandwidth    23�A IDD (default value)
} ADXL345DATARATE;


/* Used with register 0x31 (ADXL345_REG_DATA_FORMAT) to set g range */
typedef enum {
    ADXL345_RANGE_16_G          = 0b11,   // +/- 16g
    ADXL345_RANGE_8_G           = 0b10,   // +/- 8g
    ADXL345_RANGE_4_G           = 0b01,   // +/- 4g
    ADXL345_RANGE_2_G           = 0b00,   // +/- 2g (default value)
    ADXL345_RANGE_FULL_RANGE    = 0b1000  //sets altimeter to full range
} ADXL345Range;


