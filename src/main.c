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

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void showUsageInfo(void){
  
    struct ifreq ifr;
    char iface[] = "eth0"; // 你要查询的网络接口，可以根据实际情况修改
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    // printf("IP Address: %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    fprintf(stderr, "Info : usbip.exe --tcp-port %d attach -r %s -b 1-1\n", TCP_SERV_PORT, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}


int main() {
  printf("start webOcd_usbip_cmsisDap\n");
  if(axi_ffjtag_init() != ERROR_OK){
    fprintf(stderr, "axiFFJtag init error!\n");
  }
 
  //usage: tip
  // fprintf(stderr,"Info : usbip.exe --tcp-port %d attach -r ");
  showUsageInfo();


  dap_init();
  vstub_main();

  #if 0
  affJtag_pkg pkg;
  int32_t i=0;
  printf("for test axiFFjtager basic function\n");
  int clkNs=0;
  int freq = 10000;
  #define MAINCLK (1000000000)
  if(freq>10000000){freq=10000000;}
  clkNs = (MAINCLK/freq);
  // fprintf(stderr, "dap_setup_clock:freq=%d  clkNs=%d\n",freq, clkNs);
  setAxiFFJtagFreq_api(clkNs);
  do {
    i++;
    do{
    pkg.bytesLeft = 4;
    pkg.len[0].u32 = 0x16<<8;
    pkg.tms[0].u32 = 0x55;
    pkg.tdi[0].u32 = 0x0;
    // pkg.needBereadBytes = 4;
    // pkg.wcb[0].u32 = REQUEST_WCB;
    pkg.needBereadBytes = 0;
    pkg.wcb[0].u32 = NO_REQUEST_WCB;
    axi_ffjtag_opr_queue(&pkg);
    }while(0);
    fprintf(stderr, "i=%d,pkg.tdo[0].u32=0x%x\n",i,pkg.tdo[0].u32);
    
    do{
    pkg.bytesLeft = 4;
    pkg.len[0].u32 = 0x16<<8;
    pkg.tms[0].u32 = 0x55;
    pkg.tdi[0].u32 = 0x0;
    // pkg.needBereadBytes = 4;
    // pkg.wcb[0].u32 = REQUEST_WCB;
    pkg.needBereadBytes = 0;
    pkg.wcb[0].u32 = NO_REQUEST_WCB;
    axi_ffjtag_opr_queue(&pkg);
    }while(0);
    fprintf(stderr, "i=%d,pkg.tdo[0].u32=0x%x\n",i,pkg.tdo[0].u32);

    do{
    pkg.bytesLeft = 4;
    pkg.len[0].u32 = 0xf<<16;
    pkg.tms[0].u32 = 0x55;
    pkg.tdi[0].u32 = 0x0;
    pkg.needBereadBytes = 4;
    pkg.wcb[0].u32 = REQUEST_WCB;
    // pkg.needBereadBytes = 0;
    // pkg.wcb[0].u32 = NO_REQUEST_WCB;
    axi_ffjtag_opr_queue(&pkg);
    }while(0);
    fprintf(stderr, "i=%d,pkg.tdo[0].u32=0x%x\n",i,pkg.tdo[0].u32);
    //sleep(1);
  }while(i<1);
  #endif
}