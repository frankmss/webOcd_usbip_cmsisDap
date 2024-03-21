#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opr_axiBlaster.h"

// int main(void ){
//   printf("start webOcd_usbip_cmsisDap\n");
//   axiBlaster *axiBlasterMod = malloc(sizeof(axiBlaster));

//   memset(axiBlasterMod, 0, sizeof(axiBlaster));

//   int32_t rsp = axiBlaster_init(axiBlasterMod);
//   if(rsp!=0){fprintf(stderr, "axiBlaster init error !\n");}
//   else{
//     fprintf(stderr, "axiBlaster init ok ... in main.c\n");
//     fprintf(stderr, "addBase:0x%p,0x%p\n", axiBlasterMod->map_base,
//     axiBlasterMod->offset);
//   }

//   reset_affjtag(axiBlasterMod->offset, 20);
//   reset_only_affjtag(axiBlasterMod->offset);

//     int len = 2;
//   int nr_bytes = (len + 7) / 8;
//   int bytesLeft = nr_bytes;
//   int bitsLeft = len;
//   int byteIndex = 0;
//   int tdi, tms, tdo;
//   unsigned char buffer[2048], result[1024];
//   oper_affJtag_mainLoop(axiBlasterMod->offset, bytesLeft, bitsLeft, nr_bytes,
//   buffer, result);

//   // oper_affJtag_mainLoop(axiBlasterMod->offset, 8,8,2,buffer,result);

//   // void write32bit_queue(volatile axiBlaster_reg *jtr, uint32_t len,
//   //uint32_t tms,
//   //                     uint32_t tdi, uint32_t cb)
//  // write32bit_queue(axiBlasterMod->offset, 32, 0x55555555, 0xaaaaaaaa, 0x0);
// }








#include <fcntl.h>  // O_RDWR | O_SYNC
#include <sys/mman.h>
#include <unistd.h>  //_SC_PAGESIZE

#define MAP_SIZE 0x10000
// int main_a(axiBlaster *axiBlasterMod) {
//   int i;
//   int s;
//   int c;
//   int fd_uio;

//   fd_uio = open("/dev/mem", O_RDWR | O_SYNC);
//   if (fd_uio < 1) {
//     fprintf(stderr, "Failed to Open UIO Device\n");
//     return -1;
//   }

//   uint64_t target = 0x43c30000;  // netOcd axiff_1
//   int16_t port = 2544;
//   // uint64_t target = 0x43c40000; //netOcd axiff_0
//   // int16_t port = 2543;
//   void *map_base;
//   unsigned int pagesize = (unsigned)sysconf(_SC_PAGESIZE);
//   axiBlasterMod->map_base =
//       (unsigned int *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
//                            fd_uio, target & ~((typeof(target))pagesize - 1));
//   unsigned int offset = (unsigned int)(target & (pagesize - 1));
//   axiBlasterMod->offset = (axiBlaster_reg *)(axiBlasterMod->map_base + offset);
//   if (axiBlasterMod->offset == MAP_FAILED) {
//     fprintf(stderr, "Error:MMAP Failed\n");
//     close(fd_uio);
//   } else {
//     fprintf(stderr, "\nInfo :map 0x%x->0x%x map_base:0x%x offset:0x%x\n",
//             target, axiBlasterMod->offset, map_base, offset);
//   }
//   // reset_affjtag(ptr, 5);
//   fprintf(stderr, "Info : Xilinx xvc init ok\n");
//   int len = 2;
//   int nr_bytes = (len + 7) / 8;
//   int bytesLeft = nr_bytes;
//   int bitsLeft = len;
//   int byteIndex = 0;
//   int tdi, tms, tdo;
//   unsigned char buffer[2048], result[1024];
//   // oper_affJtag_mainLoop(axiBlasterMod->offset, bytesLeft, bitsLeft, nr_bytes, buffer, result);

//   // munmap((void *)ptr, MAP_SIZE);
//   return 0;
// }
int main() {
  printf("start webOcd_usbip_cmsisDap\n");
  axiBlaster *axiBlasterMod = malloc(sizeof(axiBlaster));

  memset(axiBlasterMod, 0, sizeof(axiBlaster));
  //axiBlaster_reg *ptr;
  int rno = axiBlaster_init(axiBlasterMod);
  if(rno!=0){fprintf(stderr, "axiBlaster init error "); return -1;}
  int len = 9;
  int nr_bytes = (len + 7) / 8;
  int bytesLeft = nr_bytes;
  int bitsLeft = len;
  int byteIndex = 0;
  //int tdi, tms, tdo;
  unsigned char buffer[2048], result[1024];
  reset_affjtag(axiBlasterMod->offset, 100);
  reset_only_affjtag(axiBlasterMod->offset);
  //oper_affJtag_mainLoop(axiBlasterMod->offset, bytesLeft, bitsLeft, nr_bytes, buffer, result);
  write32bit_queue(axiBlasterMod->offset,8,0x0f,0xaa,0);
}