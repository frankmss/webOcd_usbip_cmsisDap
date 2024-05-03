#ifndef __AXIFFJTAG_H__
#define __AXIFFJTAG_H__

#include <stdint.h>

#define ERROR_JTAG_INIT_FAILED       (-100)
#define ERROR_JTAG_INVALID_INTERFACE (-101)
#define ERROR_JTAG_NOT_IMPLEMENTED   (-102)
#define ERROR_JTAG_TRST_ASSERTED     (-103)
#define ERROR_JTAG_QUEUE_FAILED      (-104)
#define ERROR_JTAG_NOT_STABLE_STATE  (-105)
#define ERROR_JTAG_DEVICE_ERROR      (-107)
#define ERROR_JTAG_STATE_INVALID     (-108)
#define ERROR_JTAG_TRANSITION_INVALID (-109)
#define ERROR_JTAG_INIT_SOFT_FAIL    (-110)
#define ERROR_OK						(0)


#define CLK_T 40  // 40ns
#define RESET_CMD_CLK (0x0000aaaa)
#define RESET_CMD_JTAG (0x0000aaa5)
#define REQUEST_WCB (0x55aa0000)
#define NO_REQUEST_WCB (0x00000000)
#define REAL_SEND (0x00000001)
#define NOT_REAL_SEND (0x00000000)

#define AFF_PKG_32SIZE 64
#define AFF_PKG_8SIZE (AFF_PKG_32SIZE * 4)
union u4B2B {
  uint32_t u32;
  uint8_t u8[4];
};
typedef struct {
  union u4B2B len[AFF_PKG_32SIZE];
  union u4B2B tms[AFF_PKG_32SIZE];
  union u4B2B tdi[AFF_PKG_32SIZE];
  union u4B2B wcb[AFF_PKG_32SIZE];
  union u4B2B tdo[AFF_PKG_32SIZE];
  int32_t bytesLeft;
  int32_t needBereadBytes;
} affJtag_pkg;

typedef struct {
  uint32_t jtag_clk_offset;  // reg0 jtag clk = 25000/n
  uint32_t enable_offset;    // reg1 nc
  uint32_t lenght_offset;    // reg2
  uint32_t tms_offset;       // reg3
  uint32_t tdi_offset;       // reg4
  uint32_t wrback_offset;    // reg5 //0x55aa0000, readback
  uint32_t full_offset;   // reg6 [3:0] [11:8]=len,tms,tdi,wb fifo empty signal;
  uint32_t empty_offset;  // reg7 [0]
  uint32_t tdo_offset;    // reg8
  uint32_t reset_offset;  // reg9 //0x0000aaaa reset jtag, 0x0000 do nothing
                          // except clear enable, reset
} affJtag_t;

struct xlnx_axiffjtag_xvc {
  int fd;
  uint32_t *baseaddr;
  volatile affJtag_t *ptr;
  char *deviceaddr;
  // struct xlnx_axi_signal trst;
  // struct xlnx_axi_signal srst;
};


int axi_ffjtag_init(void) ;
void axi_ffjtag_opr_queue(affJtag_pkg *affjpkg) ;
void setAxiFFJtagFreq_api(int clkNs);


  //0x43c30000->dap_1
  //0x43c40000->dap_0

//this is for usbip0
// #define DAP0
// #define TCP_SERV_PORT 3240
// #define AXIFFJTAG_REGBASE 0X43C40000
// #endif

// this is for usbip1
#define DAP1
#define TCP_SERV_PORT 3241
#define AXIFFJTAG_REGBASE 0X43C30000
#endif