#ifndef __OPR_AXIBLASTER_H__
#define __OPR_AXIBLASTER_H__

#include <stdint.h>
typedef struct {
  uint32_t jtag_clk_offset;  // reg0 jtag clk = 25000/n
  uint32_t enable_offset;    // reg1
  uint32_t lenght_offset;    // reg2
  uint32_t tms_offset;       // reg3
  uint32_t tdi_offset;       // reg4
  uint32_t wrback_offset;    // reg5 //0x55aa0000, readback
  uint32_t full_offset;   // reg6 [3:0] [11:8]=len,tms,tdi,wb fifo empty signal;
  uint32_t empty_offset;  // reg7 [0]
  uint32_t tdo_offset;    // reg8
  uint32_t reset_offset;  // reg9 //0x0000aaaa reset jtag, 0x0000 do nothing
                          // except clear enable, reset
} axiBlaster_reg;

struct axiBlasterMod{
  void *map_base;
  axiBlaster_reg *offset;

};
typedef struct axiBlasterMod axiBlaster;

int axiBlaster_init(axiBlaster *mod );
void oper_affJtag_mainLoop(volatile axiBlaster_reg *ptr, int bytesLeft, int bitsLeft,
                           int nr_bytes,  // should be send byte
                           unsigned char *buffer, unsigned char *result);
void reset_affjtag(volatile axiBlaster_reg *ptr, int32_t N_ns);
void reset_only_affjtag(volatile axiBlaster_reg *ptr);
void write32bit_queue(volatile axiBlaster_reg *jtr, uint32_t len, uint32_t tms,
                      uint32_t tdi, uint32_t cb);

                      
#endif