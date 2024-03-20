#include "opr_axiBlaster.h"

#include <fcntl.h>  // O_RDWR | O_SYNC
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>  //_SC_PAGESIZE

#define AXIBLASTER_BASE_ADDR 0X43C30000
#define MAP_SIZE 0x10000
static int verbose = 1;

void enable_send_queue(volatile axiBlaster_reg *jtr) {
  jtr->enable_offset = 0x01;
  jtr->enable_offset = 0;
}

void showAllReg_noTdo(volatile axiBlaster_reg *jtr) {
  fprintf(stderr, "jtr->jtag_clk_offset (reg0):0x%x \n", jtr->jtag_clk_offset);
  fprintf(stderr, "jtr->enable_offset   (reg1):0x%x \n", jtr->enable_offset);
  fprintf(stderr, "jtr->lenght_offset   (reg2):0x%x \n", jtr->lenght_offset);
  fprintf(stderr, "jtr->tms_offset      (reg3):0x%x \n", jtr->tms_offset);
  fprintf(stderr, "jtr->tdi_offset      (reg4):0x%x \n", jtr->tdi_offset);
  fprintf(stderr, "jtr->wrback_offset   (reg5):0x%x \n", jtr->wrback_offset);
  fprintf(stderr, "jtr->full_offset     (reg6):0x%x \n", jtr->full_offset);
  fprintf(stderr, "jtr->empty_offset    (reg7):0x%x \n", jtr->empty_offset);
  // fprintf( stderr,"jtr->tdo_offset      (reg8):0x%x \n", jtr->tdo_offset);
  fprintf(stderr, "jtr->reset_offset    (reg9):0x%x \n\n", jtr->reset_offset);
}

void waitFFNotFull(volatile axiBlaster_reg *jtr) {
  int waitTimes = 0;
  uint32_t once = 0;
  while (1) {
    uint32_t full_offset = jtr->full_offset;
    if ((full_offset & 0x0f) == 0) {
      break;
    } else {
      usleep(200 + (waitTimes * 10));
      waitTimes++;
      once++;
      if ((once == 1)) {
        if (verbose) {
          fprintf(stderr, "wait ff Full is long times %d\n", waitTimes);
        }
      } else {
        if (verbose) {
          fprintf(stderr, "in waitFFNot Full function\n");
          showAllReg_noTdo(jtr);
          sleep(1);
          break;
        }
      }
    }
  }
}

void waitFFNotEmpty(volatile axiBlaster_reg *jtr) {
  uint32_t times = 0;
  uint32_t once = 0;
  while (1) {
    if (((jtr->empty_offset) & 0x01) == 0) {
      break;
    } else {
      usleep(200 + (times));
      times++;
      once++;
      if (verbose && (once == 1)) {
        // fprintf( stderr,"wait ff Empty is long times %d\n", times);
      } else if (verbose && (once == 2)) {
        // showAllReg_noTdo(jtr);
      } else if (verbose && (once == 3)) {
        break;
      }
    }
  }
}

void reset_jtag(volatile axiBlaster_reg *jtr) {
  jtr->reset_offset = 0x0000aaaa;
  usleep(10);
  jtr->reset_offset = 0x0000;
  usleep(10);
}

void write32bit_queue(volatile axiBlaster_reg *jtr, uint32_t len, uint32_t tms,
                      uint32_t tdi, uint32_t cb) {
  waitFFNotFull(jtr);
  jtr->lenght_offset = len;
  jtr->tms_offset = tms;
  jtr->tdi_offset = tdi;
  jtr->wrback_offset = cb;
  while (1) {
    if ((jtr->full_offset & 0xf00) == 0) {
      break;
    } else {
      usleep(1);
    }
  }
  enable_send_queue(jtr);
}

void read32bit_queue(volatile axiBlaster_reg *jtr, int32_t nB,
                     uint32_t *result) {
  int32_t nrd = 0;
  uint32_t *u32p = (uint32_t *)result;
  while (1) {
    uint32_t t = jtr->empty_offset;
    // if(verbose){fprintf( stderr,"empty_offset:0x%x((t)&0x1:%x)
    // nrd:(%d)\n",t,(t)&0x1, nrd);}
    if (((t) & 0x1) != 1) {
      // if(verbose){fprintf( stderr," %dext[0x%x]",nrd++,jtr->tdo_offset);}
      *u32p = jtr->tdo_offset;
      nrd++;
    } else {
      usleep(1);
    }
    if (nrd == nB) {
      if ((jtr->empty_offset & 0x01) != 1) {
        fprintf(stderr, "read32bit_queue function read err!\n");
        showAllReg_noTdo(jtr);
        // while(1){nrd=nrd;}
        reset_jtag(jtr);

      } else {
        break;
      }
    }
  }
}

#define CLK_T 20  // 40ns
void reset_affjtag(volatile axiBlaster_reg *ptr, int32_t N_ns) {
  // 25Mhz 40ns,
  uint32_t clk = N_ns / CLK_T;
  if (verbose) {
    fprintf(stderr, "affjtag clk=%d\n", clk);
  }
  // ptr->jtag_clk_offset = 1000;
  ptr->jtag_clk_offset = clk;
  ptr->reset_offset = 0x0000aaaa;
  usleep(10);
  ptr->reset_offset = 0x0000;
  usleep(10);
}

void reset_only_affjtag(volatile axiBlaster_reg *ptr) {
  ptr->reset_offset = 0x0000aaa5;
  usleep(10);
  ptr->reset_offset = 0x0000;
  usleep(10);
}

void oper_affJtag_mainLoop(volatile axiBlaster_reg *ptr, int bytesLeft,
                           int bitsLeft,
                           int nr_bytes,  // should be send byte
                           unsigned char *buffer, unsigned char *result) {
  int Bleft = bytesLeft;
  int bLeft = bitsLeft;

  unsigned char *pbuf = buffer;
  unsigned char tmpResult[1024];

  // union uint32_4char tmp_tdo;
  uint32_t tmp_tdo;
  int alread_RBn = 0, i = 0;
  int alread_sbit = 0;
  int alread_sBn = 0;
  uint32_t sff_status = 0;
  uint32_t rff_status = 0;
  uint32_t stms, stdi, slen, scb;
  int32_t once_send_num = 0;
  int32_t sB_num = 0, rB_num = 0;
  memset(tmpResult, 0, sizeof(tmpResult));
  while (1) {  // main loop
    // 0:get fifo status
    // 1:if rec fifo is not empty, rec data
    // 2:if send fifo is not full, send data
    sff_status = ptr->full_offset;
    rff_status = ptr->empty_offset;
    if (verbose) {
      fprintf( stderr,"in while(1)\n");
      fprintf( stderr,"sff_status(0x%x),rff_status(0x%x)\n", sff_status,
      rff_status); fprintf( stderr,"alread_RBn(%d),bytesLeft(%d)\n",
      alread_RBn, bytesLeft); fprintf(
      stderr,"alread_sbit(%d),bitsLeft(%d)\n\n", alread_sbit, bitsLeft);
    }

    //// read queue

    if ((ptr->empty_offset & 0x01) != 1) {  // rec fifo is not empty
      while (1) {                           // rec all data
        if ((ptr->empty_offset & 0x01) == 1) {
          break;
        } else {
          if (0) {
            fprintf(stderr, "recieve queue\n");
          }
          tmp_tdo = ptr->tdo_offset;
          // if(verbose){fprintf( stderr,"[0x%8x],<0x%x><0x%x><0x%x><0x%x>\n",
          //             tmp_tdo.u32, tmp_tdo.ch[0], tmp_tdo.ch[1],
          //             tmp_tdo.ch[2], tmp_tdo.ch[3]);}
          memcpy(tmpResult + alread_RBn, &(tmp_tdo), 4);
          if (0) {
            uint32_t *tt = (uint32_t *)(tmpResult + alread_RBn);

            fprintf(stderr, "[0x%08x -> 0x%08x]\n", *tt, tmp_tdo);
          }
          alread_RBn = alread_RBn + 4;
          rB_num++;
        }
      }
      if (alread_RBn >= bytesLeft) {  // if rec num > bytesLeft ,return;
        if ((ptr->empty_offset) != 1) {
          if (verbose) {
            fprintf(stderr, "**************not match reg:0x%08x\n",
                    ptr->empty_offset);
          }
        }
        break;  // break; main while(1)
      }

      //// send queue
      // sff_status
    }

    else if (((ptr->full_offset & 0x0f) == 0x00) && (alread_sbit <= bitsLeft)) {
      if (0) {
        fprintf(stderr, "send queue\n");
      }
      once_send_num = 0;
      while (1) {  // until ptr->full_offset & 0x0f == 0xf
        // once_send_num++;
        if (((ptr->full_offset & 0x0f) != 0x00) || (alread_sbit >= bitsLeft)) {
          break;
        } else {  // can be send
          if (0) {
            fprintf(stderr, "send queue real send alread_sbit(%d)\n",
                    alread_sbit);
          }
          if ((bitsLeft - alread_sbit) >= 32) {
            slen = 32;
            memcpy(&stms, buffer + (alread_sbit / 8), 4);
            memcpy(&stdi, buffer + (alread_sbit / 8) + nr_bytes, 4);
          } else {
            slen = bitsLeft - alread_sbit;
            // memcpy(&stms, buffer + (alread_sbit / 8) + 1, (slen / 8) + 1);
            // memcpy(&stdi, buffer + (alread_sbit / 8) + 1 + nr_bytes,
            //        (slen / 8) + 1);
            memcpy(&stms, buffer + (alread_sbit / 8), 4);
            memcpy(&stdi, buffer + (alread_sbit / 8) + nr_bytes, 4);
          }

          alread_sbit = alread_sbit + slen;
          scb = 0x55aa0000;
          ptr->tms_offset = stms;
          ptr->tdi_offset = stdi;
          ptr->lenght_offset = slen;
          ptr->wrback_offset = scb;

          sB_num++;
        }
      }
    } else {
      if (0) {
        fprintf(stderr, "while(1) else ...\n");
        sff_status = ptr->full_offset;
        rff_status = ptr->empty_offset;
        fprintf(stderr, "sff_status(0x%x),rff_status(0x%x)\n", sff_status,
                rff_status);
        fprintf(stderr, "alread_RBn(%d),bytesLeft(%d)\n", alread_RBn,
                bytesLeft);
        fprintf(stderr, "alread_sbit(%d),bitsLeft(%d)\n\n", alread_sbit,
                bitsLeft);
        usleep(100);
        // reset_jtag(ptr);
      }
    }
    // usleep(100);
  }

  memcpy(result, tmpResult, nr_bytes);
}

int axiBlaster_init(axiBlaster *mod) {
  int fd_uio;
  uint64_t target = AXIBLASTER_BASE_ADDR;
  fd_uio = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd_uio < 1) {
    fprintf(stderr, "FAILED TO open UIO Device");
  }

  unsigned int pagesize = (unsigned)sysconf(_SC_PAGESIZE);
  mod->map_base =
      (unsigned int *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                           fd_uio, target & ~((typeof(target))pagesize - 1));
  unsigned int offset = (unsigned int)(target & (pagesize - 1));
  mod->offset = (axiBlaster_reg *)(mod->map_base + offset);

  if (mod->offset == MAP_FAILED) {
    fprintf(stderr, "Error: MMAP Failed\n");
    close(fd_uio);
    return -1;
  } else {
    fprintf(stderr, "\nInfo :map 0x%llx->0x%p \n", target, (mod->map_base));
  }
  fprintf(stderr, "init axiBlaster ... ok\n");
  sleep(1);
  //munmap((void *)mod->map_base, MAP_SIZE);
  return 0;
}
