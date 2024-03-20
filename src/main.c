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
  uint8_t buffer[2],result[2];
  // void oper_affJtag_mainLoop(volatile axiBlaster_reg *ptr, int bytesLeft, int bitsLeft,
  //                          int nr_bytes,  // should be send byte
  //                          unsigned char *buffer, unsigned char *result);
  oper_affJtag_mainLoop(axiBlasterMod->offset, 1,1,1,buffer,result);
}
