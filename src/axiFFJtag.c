
#include "axiFFJtag.h"

#include <fcntl.h>  // O_RDWR | O_SYNC
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>  //_SC_PAGESIZE
#include <string.h>

static struct xlnx_axiffjtag_xvc axi_ffjtag_state;
static struct xlnx_axiffjtag_xvc *xlnx_axiffjtag_xvc = &axi_ffjtag_state;



void reset_affjtag(volatile affJtag_t *ptr, int32_t N_ns) {
  // 25Mhz 40ns,
  N_ns = N_ns;
  uint32_t clk = N_ns / CLK_T;
  clk = clk;
  fprintf(stderr, "affjtag ptr addr(0x%08x)", (uint32_t)ptr);

  ptr->jtag_clk_offset = clk;
  ptr->reset_offset = RESET_CMD_CLK;
  usleep(10);
  ptr->reset_offset = 0x0000;
  usleep(10);
  fprintf(stderr, "affjtag ptr->clk_offset val(0x%08x)\n",
          (uint32_t)ptr->jtag_clk_offset);
}

void reset_only_affjtag(volatile affJtag_t *ptr) {
  ptr->reset_offset = RESET_CMD_JTAG;
  usleep(10);
  ptr->reset_offset = 0x0000;
  usleep(10);
}

void axi_ffjtag_clear_pkg(affJtag_pkg *affjpkg) {
  do {
    int32_t clen = 0;
    if (affjpkg->bytesLeft == 0) {
      clen = AFF_PKG_8SIZE;
    } else {
      clen = affjpkg->bytesLeft;
    }
    memset(affjpkg->len[0].u8, 0, clen);
    memset(affjpkg->tms[0].u8, 0, clen);
    memset(affjpkg->tdi[0].u8, 0, clen);
    memset(affjpkg->wcb[0].u8, 0, clen);
    memset(affjpkg->tdo[0].u8, 0, clen);
    affjpkg->needBereadBytes = 0;
    affjpkg->bytesLeft = 0;
  } while (0);
}

void axi_ffjtag_opr_queue(affJtag_pkg *affjpkg) {
  volatile affJtag_t *ptr = xlnx_axiffjtag_xvc->ptr;
  uint32_t tmp_tdo;
  int alread_RBn = 0;
  int alread_SBn = 0;
  uint32_t stms = 0, stdi = 0, slen = 0, scb = 0;
  // uint8_t *result = affjpkg->tdo[0].u8;
  int32_t bytesLeft = affjpkg->bytesLeft;
  int32_t i = 0;
  int32_t needBereadBytes = affjpkg->needBereadBytes;

  while (1) {                               // main loop
    if ((ptr->empty_offset & 0x01) != 1) {  // rec fifo is not empty
      while (1) {                           // rec all data
        if ((ptr->empty_offset & 0x01) == 1) {
          break;
        } else {
          tmp_tdo = ptr->tdo_offset;
          affjpkg->tdo[alread_RBn / 4].u32 = tmp_tdo;
          alread_RBn = alread_RBn + 4;
        }
      }

    }  //// send queue// sff_status

    else if (((ptr->full_offset & 0x0f) == 0x00) && (alread_SBn <= bytesLeft)) {
      while (1) {  // until ptr->full_offset & 0x0f == 0xf

        if (((ptr->full_offset & 0x0f) != 0x00) || (alread_SBn >= bytesLeft)) {
          break;
        } else {
          scb = affjpkg->wcb[alread_SBn / 4].u32;
          slen = affjpkg->len[alread_SBn / 4].u32;
          stms = affjpkg->tms[alread_SBn / 4].u32;
          stdi = affjpkg->tdi[alread_SBn / 4].u32;
          fprintf(stderr,
              "in while(1) affjtag write last "
              "slen(%d),stms(%d),stdi(%d),scb(%d) \n",
              slen, stms, stdi, scb);
          fprintf(stderr,"alread_SBn / 4 = 0X%08X \n", (alread_SBn / 4));
          ptr->lenght_offset = slen;
          ptr->tms_offset = stms;
          ptr->tdi_offset = stdi;
          ptr->wrback_offset = scb;

          alread_SBn += 4;
        }
      }
    }

    if ((alread_RBn >= needBereadBytes) && (alread_SBn >= bytesLeft) &&
        ((ptr->full_offset & 0x0f00) ==
         0x0f00)) {  // if rec num > bytesLeft ,return;
      fprintf(stderr,"already send bytes(%d)  should be sent bytes(%d)\n",
                   alread_SBn, bytesLeft);
      fprintf(stderr,"already rec  bytes(%d)  should be rec bytes(%d)\n",
                   alread_RBn, needBereadBytes);
      for (i = 0; i < needBereadBytes / 4; i++) {
        fprintf(stderr,"tdo:(%i)[0x%08x] \n", i, affjpkg->tdo[i].u32);
      }
      for (i = 0; i < bytesLeft / 4; i++) {
        fprintf(stderr,"(%i):tdi[0x%08x]tms[0x%08x]len[0x%08x]wcb[0x%08x] \n", i,
                     affjpkg->tdi[i].u32, affjpkg->tms[i].u32,
                     affjpkg->len[i].u32, affjpkg->wcb[i].u32);
      }
      break;  // break; main while(1)
    }
  }
}

int axi_ffjtag_init(void) {
  uint32_t baseaddr = 0X43C30000;
  xlnx_axiffjtag_xvc->fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (xlnx_axiffjtag_xvc->fd < 0) {
    fprintf(stderr, "Failed to open /dev/mem.  Check permissions.\n");
    return ERROR_JTAG_INIT_FAILED;
  }
#define MAP_SIZE 0x10000
  unsigned int pagesize = (unsigned)sysconf(_SC_PAGESIZE);
  xlnx_axiffjtag_xvc->ptr = (affJtag_t *)mmap(
      NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
      xlnx_axiffjtag_xvc->fd, baseaddr & ~((typeof(baseaddr))pagesize - 1));
  if (xlnx_axiffjtag_xvc->ptr == MAP_FAILED) {
    fprintf(stderr, "mmap() failed.  Check permissions.\n");
    close(xlnx_axiffjtag_xvc->fd);
    return ERROR_JTAG_INIT_FAILED;
  }
  fprintf(stderr, "Mapped axiFFJtag vaddr %p paddr 0x%x \n",
          xlnx_axiffjtag_xvc->ptr, baseaddr);
  reset_affjtag(xlnx_axiffjtag_xvc->ptr, 600);
  reset_only_affjtag(xlnx_axiffjtag_xvc->ptr);
  return ERROR_OK;
}