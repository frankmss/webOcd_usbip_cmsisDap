#pragma once

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <stdint.h>


/*- Definitions -------------------------------------------------------------*/
#define DAP_CONFIG_ENABLE_JTAG

#define DAP_CONFIG_DEFAULT_PORT DAP_PORT_SWD
#define DAP_CONFIG_DEFAULT_CLOCK 8000000 // Hz

#define DAP_CONFIG_PACKET_SIZE 64
#define DAP_CONFIG_PACKET_COUNT 1

#define DAP_CONFIG_JTAG_DEV_COUNT 8

// DAP_CONFIG_PRODUCT_STR must contain "CMSIS-DAP" to be compatible with the standard
#define DAP_CONFIG_VENDOR_STR "Dumbo Devices"
#define DAP_CONFIG_PRODUCT_STR "CMSIS-DAP Adapter"
#define DAP_CONFIG_SER_NUM_STR dap_serial_number
#define DAP_CONFIG_CMSIS_DAP_VER_STR "2.0.0"

// Attribute to use for performance-critical functions
// #define DAP_CONFIG_PERFORMANCE_ATTR IRAM_ATTR
#define DAP_CONFIG_PERFORMANCE_ATTR
// A value at which dap_clock_test() produces 1 kHz output on the SWCLK pin
#define DAP_CONFIG_DELAY_CONSTANT 24000

// A threshold for switching to fast clock (no added delays)
// This is the frequency produced by dap_clock_test(1) on the SWCLK pin
#define DAP_CONFIG_FAST_CLOCK 8000000 // Hz

#if 0
#define TCK_PIN (12)
#define TMS_PIN (11)
#define TDI_PIN (21)
#define TDO_PIN (1)

#undef PLATFORM_HAS_TRACESWO
#define TRACESWO_PIN 18

// ON ESP32 we dont have the PORTS, this is dummy value until code is corrected
#define SWCLK_PORT (0)
#define SWCLK_PIN (12)
#define SWDIO_PIN (11)
#define TMS_DIR_PIN  (10)
#endif
#define ESP_TDI_PIN (21)
#define ESP_TDO_PIN (1)
#define ESP_TCK_PIN (12)
#define ESP_TMS_PIN (11)

#define ESP_SWCLK_PIN (12)
#define ESP_SWDIO_PIN (11)
#define ESP_TMS_DIR_PIN (10)


/*- Prototypes --------------------------------------------------------------*/
void dap_callback_connect(void);
void dap_callback_disconnect(void);
char dap_serial_number[32];
/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWCLK_TCK_write(int value) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWDIO_TMS_write(int value) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_TDI_write(int value) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_TDO_write(int value) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_nTRST_write(int value) {
    // Do nothing
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_nRESET_write(int value) {
    // Do nothing
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_SWCLK_TCK_read(void) {
    int level =  0x1;
    return level;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_SWDIO_TMS_read(void) {


    int level = 0x1;
    return level;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_TDO_read(void) {
    int level = 0x1;
    return level;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_TDI_read(void) {
    int level = 0x1;
    return level;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_nTRST_read(void) {
    // Do nothing
    return 0;
}

//-----------------------------------------------------------------------------
static inline int DAP_CONFIG_nRESET_read(void) {
    // Do nothing
    return 0;
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWCLK_TCK_set(void) {
    
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWCLK_TCK_clr(void) {
    
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWDIO_TMS_in(void) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SWDIO_TMS_out(void) {
   
}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_SETUP(void) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_DISCONNECT(void) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_CONNECT_SWD(void) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_CONNECT_JTAG(void) {

}

//-----------------------------------------------------------------------------
static inline void DAP_CONFIG_LED(int index, int state) {
    // Do nothing
}

//-----------------------------------------------------------------------------
__attribute__((always_inline)) static inline void DAP_CONFIG_DELAY(uint32_t cycles) {
    register int32_t cnt;
    for(cnt = cycles; --cnt > 0;)
        ;
}