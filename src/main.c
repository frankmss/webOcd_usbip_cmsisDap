#include <stdio.h>
#include "opr_axiBlaster.h"
#include <stdlib.h>
#include <string.h>

int main(void ){
  printf("start webOcd_usbip_cmsisDap\n");
  axiBlaster *axiBlasterMod = malloc(sizeof(axiBlaster));
  
  memset(axiBlasterMod, 0, sizeof(axiBlaster));
  int32_t rsp = axiBlaster_init(axiBlasterMod);
  if(rsp!=0){fprintf(stderr, "axiBlaster init error !\n");}
  else{
    fprintf(stderr, "axiBlaster init ok ... in main.c\n");
  }
  uint8_t buffer[32],result[32];
  // void oper_affJtag_mainLoop(volatile axiBlaster_reg *ptr, int bytesLeft, int bitsLeft,
  //                          int nr_bytes,  // should be send byte
  //                          unsigned char *buffer, unsigned char *result);
  reset_affjtag(axiBlasterMod->offset, 20);
  reset_only_affjtag(axiBlasterMod->offset);
  oper_affJtag_mainLoop(axiBlasterMod->offset, 8,8,2,buffer,result);

  // void write32bit_queue(volatile axiBlaster_reg *jtr, uint32_t len, uint32_t tms,
  //                     uint32_t tdi, uint32_t cb)
 // write32bit_queue(axiBlasterMod->offset, 32, 0x55555555, 0xaaaaaaaa, 0x0);
}
