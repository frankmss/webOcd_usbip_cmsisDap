#ifndef __CMSIS_DAP_USBIP_H__
#define __CMSIS_DAP_USBIP_H__
#include <stdint.h>
#include "vstub/vstub.h"

#define DAPBUFFSIZE 512

struct usbip2dapPkg {
  int32_t len;
  uint8_t buf[DAPBUFFSIZE];
  USBIP_CMD_SUBMIT submit;
};

#define DAP_HANDLE_SIZE (sizeof(struct usbip2dapPkg) + sizeof(USBIP_CMD_SUBMIT))

#endif