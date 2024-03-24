#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "axiFFJtag.h"



#include <fcntl.h>  // O_RDWR | O_SYNC
#include <sys/mman.h>
#include <unistd.h>  //_SC_PAGESIZE

#include "free-dap/dap.h"
#include "vstub_main.h"

#define MAP_SIZE 0x10000

int main() {
  printf("start webOcd_usbip_cmsisDap\n");
  if(axi_ffjtag_init() != ERROR_OK){
    fprintf(stderr, "axiFFJtag init error!\n");
  }
 
  dap_init();
  vstub_main();
  #if 0
  affJtag_pkg pkg;
  int32_t i=5;
  do {
    i++;

    pkg.bytesLeft = 4;
    pkg.needBereadBytes = 4;
    pkg.len[0].u32 = 8;
    pkg.tms[0].u8[0] = 0x3;
    pkg.tdi[0].u32 = 0x1;
    pkg.wcb[0].u32 = REQUEST_WCB;

    axi_ffjtag_opr_queue(&pkg);
    if(i>=100){
      i=0;
    }
    //sleep(1);
  }while(0);
  #endif
}