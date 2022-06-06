
#include "hal.h"
#include "hal_types.h"
#include "ble_status.h"
#include "bluenrg_updater_aci.h"
#include "bluenrg_utils.h"
#include "stm32_bluenrg_ble.h"

/************** Do not change this define section ************/

#define BLUENRG_32_MHZ          1
#define BLUENRG_32_MHZ_RO       2
#define BLUENRG_16_MHZ          3
#define BLUENRG_16_MHZ_RO       4
#define BLUENRG_CUSTOM_CONFIG   5

#define MASTER_SCA_500ppm       0 // 251 ppm to 500 ppm
#define MASTER_SCA_250ppm       1 // 151 ppm to 250 ppm
#define MASTER_SCA_150ppm       2 // 101 ppm to 150 ppm
#define MASTER_SCA_100ppm       3 // 76 ppm to 100 ppm
#define MASTER_SCA_75ppm        4 // 51 ppm to 75 ppm
#define MASTER_SCA_50ppm        5 // 31 ppm to 50 ppm
#define MASTER_SCA_30ppm        6 // 21 ppm to 30 ppm
#define MASTER_SCA_20ppm        7 // 0 ppm to 20 ppm

#define SMPS_4MHz               0
#define SMPS_8MHz               1

#ifndef SMPS_FREQUENCY
#define SMPS_FREQUENCY          SMPS_4MHz
#endif

#if !BLUENRG_MS && (SMPS_FREQUENCY == SMPS_8MHz)
#error Unsupported SMPS_FREQUENCY
#endif

/************************************************************/


/************** Definitions that can be changed. ************/

#define STACK_MODE              2
#define SLAVE_SCA_PPM           100
#define MASTER_SCA              MASTER_SCA_100ppm
#define HS_STARTUP_TIME_US      800
#define DAY                     13
#define MONTH                   06
#define YEAR                    16
/************************************************************/

const IFR_config_TypeDef IFR_config = {
  /* Cold table */
  0x02,0x3A,0x44,0x02,
  0x34,0x1B,0x02,0x39,
  0xA2,0x02,0x3C,0x00,
  0x00,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  /* Hot table */
  0x02,0x1C,0x43,0x02,
  0x20,0xEC,0x02,0x1F,
  0xAF,0x00,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,
  0x02, /* Stack mode = Mode 2, RAM1+RAM2 (1 connection) */
  0xFF, /* GPIO config = Disabled */
  0xFF,0xFF,
  /* Reserved section */
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  htobl(0xFFFFFFFF), /* Max connection event time = 4000 ms */
  htobl(0xFFFFFFFF), /* LS crystal period */
  htobl(0xFFFFFFFF), /* LS crystal frequency autocalibration*/
  htobs(0x01F4), /* Slave SCA = 500 ppm */
  0x00, /* Master SCA = 500 ppm */
  0xFF,
  htobs(0x019A), /* HS startup time = 1000 us */
  0xFF,0xFF,
  htobl(0xFFFFFFFF), /* UID */
  0xFF,
  0x16,0x06,0x13,/* Year/Month/Day in BCD. */
  /* Free section (user data) */
  htobl(0xFFFFFFFF),
  htobl(0xFFFFFFFF),
  htobl(0xFFFFFFFF),
  htobl(0xFFFFFFFF),
  htobl(0xFFFFFFFF),
};