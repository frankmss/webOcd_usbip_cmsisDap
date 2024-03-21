/* ########################################################################

   USBIP hardware emulation

   ########################################################################

   Copyright (c) : 2016  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#include <string.h>
#include <sys/param.h>


#define TAG "cmsis-dap-usbip"

#include "vstub.h"

//from tinyusb define cahill add
/// USB Descriptor Types
typedef enum {
    TUSB_DESC_DEVICE = 0x01,
    TUSB_DESC_CONFIGURATION = 0x02,
    TUSB_DESC_STRING = 0x03,
    TUSB_DESC_INTERFACE = 0x04,
    TUSB_DESC_ENDPOINT = 0x05,
    TUSB_DESC_DEVICE_QUALIFIER = 0x06,
    TUSB_DESC_OTHER_SPEED_CONFIG = 0x07,
    TUSB_DESC_INTERFACE_POWER = 0x08,
    TUSB_DESC_OTG = 0x09,
    TUSB_DESC_DEBUG = 0x0A,
    TUSB_DESC_INTERFACE_ASSOCIATION = 0x0B,

    TUSB_DESC_BOS = 0x0F,
    TUSB_DESC_DEVICE_CAPABILITY = 0x10,

    TUSB_DESC_FUNCTIONAL = 0x21,

    // Class Specific Descriptor
    TUSB_DESC_CS_DEVICE = 0x21,
    TUSB_DESC_CS_CONFIGURATION = 0x22,
    TUSB_DESC_CS_STRING = 0x23,
    TUSB_DESC_CS_INTERFACE = 0x24,
    TUSB_DESC_CS_ENDPOINT = 0x25,

    TUSB_DESC_SUPERSPEED_ENDPOINT_COMPANION = 0x30,
    TUSB_DESC_SUPERSPEED_ISO_ENDPOINT_COMPANION = 0x31
} tusb_desc_type_t;
#define TU_ARRAY_SIZE(_arr) (sizeof(_arr) / sizeof(_arr[0]))
#define TU_MIN(_x, _y) (((_x) < (_y)) ? (_x) : (_y))
#define TU_MAX(_x, _y) (((_x) > (_y)) ? (_x) : (_y))

#define TU_U16_HIGH(_u16) ((uint8_t)(((_u16) >> 8) & 0x00ff))
#define TU_U16_LOW(_u16) ((uint8_t)((_u16)&0x00ff))
#define U16_TO_U8S_BE(_u16) TU_U16_HIGH(_u16), TU_U16_LOW(_u16)
#define U16_TO_U8S_LE(_u16) TU_U16_LOW(_u16), TU_U16_HIGH(_u16)

#define TU_U32_BYTE3(_u32) ((uint8_t)((((uint32_t)_u32) >> 24) & 0x000000ff)) // MSB
#define TU_U32_BYTE2(_u32) ((uint8_t)((((uint32_t)_u32) >> 16) & 0x000000ff))
#define TU_U32_BYTE1(_u32) ((uint8_t)((((uint32_t)_u32) >> 8) & 0x000000ff))
#define TU_U32_BYTE0(_u32) ((uint8_t)(((uint32_t)_u32) & 0x000000ff)) // LSB

#define U32_TO_U8S_BE(_u32) \
    TU_U32_BYTE3(_u32), TU_U32_BYTE2(_u32), TU_U32_BYTE1(_u32), TU_U32_BYTE0(_u32)
#define U32_TO_U8S_LE(_u32) \
    TU_U32_BYTE0(_u32), TU_U32_BYTE1(_u32), TU_U32_BYTE2(_u32), TU_U32_BYTE3(_u32)

#define TU_BIT(n) (1UL << (n))
#define TU_GENMASK(h, l) ((UINT32_MAX << (l)) & (UINT32_MAX >> (31 - (h))))

// total length, number of device caps
#define TUD_BOS_DESCRIPTOR(_total_len, _caps_num) \
    5, TUSB_DESC_BOS, U16_TO_U8S_LE(_total_len), _caps_num

#define _GET_NTH_ARG( \
    _1,               \
    _2,               \
    _3,               \
    _4,               \
    _5,               \
    _6,               \
    _7,               \
    _8,               \
    _9,               \
    _10,              \
    _11,              \
    _12,              \
    _13,              \
    _14,              \
    _15,              \
    _16,              \
    _17,              \
    _18,              \
    _19,              \
    _20,              \
    _21,              \
    _22,              \
    _23,              \
    _24,              \
    _25,              \
    _26,              \
    _27,              \
    _28,              \
    _29,              \
    _30,              \
    _31,              \
    _32,              \
    _33,              \
    _34,              \
    _35,              \
    _36,              \
    _37,              \
    _38,              \
    _39,              \
    _40,              \
    _41,              \
    _42,              \
    _43,              \
    _44,              \
    _45,              \
    _46,              \
    _47,              \
    _48,              \
    _49,              \
    _50,              \
    _51,              \
    _52,              \
    _53,              \
    _54,              \
    _55,              \
    _56,              \
    _57,              \
    _58,              \
    _59,              \
    _60,              \
    _61,              \
    _62,              \
    _63,              \
    N,                \
    ...)              \
    N
#define _RSEQ_N()                                                                               \
    62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
        39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, \
        17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define _TU_NARG(...) _GET_NTH_ARG(__VA_ARGS__)
#define TU_ARGS_NUM(...) _TU_NARG(_0, ##__VA_ARGS__, _RSEQ_N())

#define TUSB_DESC_DEVICE_CAPABILITY 0x10
#define DEVICE_CAPABILITY_PLATFORM 0x05
// Device Capability Platform 128-bit UUID + Data
#define TUD_BOS_PLATFORM_DESCRIPTOR(...)                                                         \
    4 + TU_ARGS_NUM(__VA_ARGS__), TUSB_DESC_DEVICE_CAPABILITY, DEVICE_CAPABILITY_PLATFORM, 0x00, \
        __VA_ARGS__

#define TUD_BOS_WEBUSB_UUID \
    0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47, 0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x66
// Vendor Code, iLandingPage
#define TUD_BOS_WEBUSB_DESCRIPTOR(_vendor_code, _ipage) \
    TUD_BOS_PLATFORM_DESCRIPTOR(TUD_BOS_WEBUSB_UUID, U16_TO_U8S_LE(0x0100), _vendor_code, _ipage)

#define TUD_BOS_MS_OS_20_UUID \
    0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C, 0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9f

// Total Length of descriptor set, vendor code
#define TUD_BOS_MS_OS_20_DESCRIPTOR(_desc_set_len, _vendor_code) \
    TUD_BOS_PLATFORM_DESCRIPTOR(                                 \
        TUD_BOS_MS_OS_20_UUID,                                   \
        U32_TO_U8S_LE(0x06030000),                               \
        U16_TO_U8S_LE(_desc_set_len),                            \
        _vendor_code,                                            \
        0)

#define TUD_BOS_DESC_LEN 5
#define TUD_BOS_WEBUSB_DESC_LEN 24
#define TUD_BOS_MICROSOFT_OS_DESC_LEN 28
#define BOS_TOTAL_LEN (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)
#define MS_OS_20_DESC_LEN 0xB2

enum { VENDOR_REQUEST_WEBUSB = 1, VENDOR_REQUEST_MICROSOFT = 2 };

// static const char usbip_dap_link_desc_bos[] = {
//     // total length, number of device caps
//     TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

//     // Vendor Code, iLandingPage
//     TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),

//     // Microsoft OS 2.0 descriptor
//     TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT),
// };

#define USB_DESCRIPTOR_TYPE_BOS 15
#define kLengthOfBos 0x21
#define USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY 16
#define USB_DEVICE_CAPABILITY_TYPE_PLATFORM 5
// Platform capability UUID,  Table 3.
// {D8DD60DF-4589-4CC7-9CD2-659D9E648A9F}
#define USB_DEVICE_CAPABILITY_UUID \
    0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C, 0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F
#define kLengthOfMsOS20 0xA2
#define kValueOfbMS_VendorCode 0x01 // Just set to 0x01
static const char usbip_dap_link_desc_bos[] = {
    //len=33
    // USB 3.0 Specification, Table 9-9.
    0x05, // bLength of this descriptor
    USB_DESCRIPTOR_TYPE_BOS, // BOS Descriptor type(Constant)
    //USBShort(kLengthOfBos),     // wLength
    kLengthOfBos,
    0x00,

    0x01, // bNumDeviceCaps -> only 0x01 for OS2.0 descriptor

    // Microsoft OS 2.0 platform capability descriptor header (Table 4)
    // See also:
    // USB 3.0 Specification : Format of a Device Capability Descriptor, Table 9-10.

    0x1C, // bLength of this first device capability descriptor
    // bLength -> The total length of the remaining arrays containing this field
    USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY, // bDescriptorType
    USB_DEVICE_CAPABILITY_TYPE_PLATFORM, // bDevCapabilityType

    // Capability-Dependent (See USB 3.0 Specification Table 9-10.)
    0x00, // bReserved
    USB_DEVICE_CAPABILITY_UUID, // MS_OS_20_Platform_Capability_ID

    0x00,
    0x00,
    0x03,
    0x06, // dwWindowsVersion: Windows 8.1 (NTDDI_WINBLUE)
    //USBShort(kLengthOfMsOS20),  // wMSOSDescriptorSetTotalLength(length of descriptor set header)
    kLengthOfMsOS20,
    00,
    kValueOfbMS_VendorCode, // bMS_VendorCode (0x01 will be ok)
    ////TODO:change this
    0, // bAltEnumCode
};

//from tinyusb define cahill add over

/* Device Descriptor */

static const USB_DEVICE_DESCRIPTOR dev_dsc = {
    .bLength = 0x12, // Size of this descriptor in bytes
    .bDescriptorType = 0x01, // DEVICE descriptor type
    .bcdUSB = 0x0210, // USB Spec Release Number in BCD format
    .bDeviceClass = 0x00, // Class Code
    .bDeviceSubClass = 0x00, // Subclass code
    .bDeviceProtocol = 0x00, // Protocol code
    .bMaxPacketSize0 = 64, // Max packet size for EP0, see usb_config.h
    // .idVendor = 0xc251,                 // Vendor ID
    // .idProduct = 0xf00a,                 // Product ID
    // .bcdDevice = 0x0100,                 // Device release number in BCD format

    .idVendor = 0x0D28, // Vendor ID
    .idProduct = 0x0204, // Product ID
    .bcdDevice = 0x1000, // Device release number in BCD format

    .iManufacturer = 0x01, // Manufacturer string index
    .iProduct = 0x02, // Product string index
    .iSerialNumber = 0x03, // Device serial number string index
    .bNumConfigurations = 0x01 // Number of possible configurations
};

static const USB_DEVICE_QUALIFIER_DESCRIPTOR dev_qua = {
    .bLength = 0x0A, // bLength
    .bType = 0x00, // bDescriptorType
    .bcdUSB = 0x0000, // bcdUSB
    .bDeviceClass = 0x00, // bDeviceClass
    .bDeviceSubClass = 0x00, // bDeviceSubClass
    .bDeviceProtocol = 0x00, // bDeviceProtocol
    .bMaxPacketSize0 = 0x00, // bMaxPacketSize
    .bNumConfigurations = 0x00, // iSerialNumber
    .bReserved = 0x00 //bNumConfigurations*/
};

enum {
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_VENDOR,
    ITF_NUM_TOTAL,
};
enum {
    TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP = TU_BIT(5),
    TUSB_DESC_CONFIG_ATT_SELF_POWERED = TU_BIT(6),
};

#define kLengthOfMsOS20 0xA2
// Microsoft OS 2.0 descriptor types enum for wDescriptorType values, Table 9.
#define MS_OS_20_SET_HEADER_DESCRIPTOR 0x00
#define MS_OS_20_SUBSET_HEADER_CONFIGURATION 0x01
#define MS_OS_20_SUBSET_HEADER_FUNCTION 0x02
#define MS_OS_20_FEATURE_COMPATIBLE_ID 0x03
#define MS_OS_20_FEATURE_REG_PROPERTY 0x04
#define MS_OS_20_FEATURE_MIN_RESUME_TIME 0x05
#define MS_OS_20_FEATURE_MODEL_ID 0x06
#define MS_OS_20_FEATURE_CCGP_DEVICE 0x07
// Microsoft OS 2.0 descriptor set header
const char msOs20DescriptorSetHeader[kLengthOfMsOS20] = {
    // Microsoft OS 2.0 Descriptor Set header (Table 10)
    0x0A,
    0x00, // wLength (Shall be set to 0x0A)
    MS_OS_20_SET_HEADER_DESCRIPTOR,
    0x00,
    0x00,
    0x00,
    0x03,
    0x06, // dwWindowsVersion: Windows 8.1 (NTDDI_WINBLUE)
    // USBShort(kLengthOfMsOS20),  // wTotalLength
    kLengthOfMsOS20,
    0x00, //
    // Support WinUSB
    // See https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/automatic-installation-of-winusb

    // Microsoft OS 2.0 compatible ID descriptor (Table 13)
    0x14,
    0x00, // wLength
    //USBShort(MS_OS_20_FEATURE_COMPATIBLE_ID),        // wDescriptorType
    MS_OS_20_FEATURE_COMPATIBLE_ID,
    0x00,
    'W',
    'I',
    'N',
    'U',
    'S',
    'B',
    0x00,
    0x00, // compatibleID
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // subCompatibleID

    // Microsoft OS 2.0 registry property descriptor (Table 14)
    0x84,
    0x00, // wLength
    // USBShort(MS_OS_20_FEATURE_REG_PROPERTY),
    MS_OS_20_FEATURE_REG_PROPERTY,
    0x00,
    0x07,
    0x00, // wPropertyDataType: REG_MULTI_SZ (Unicode Strings)
    0x2A,
    0x00, // wPropertyNameLength
    'D',
    0,
    'e',
    0,
    'v',
    0,
    'i',
    0,
    'c',
    0,
    'e',
    0,
    'I',
    0,
    'n',
    0,
    't',
    0,
    'e',
    0,
    'r',
    0,
    'f',
    0,
    'a',
    0,
    'c',
    0,
    'e',
    0,
    'G',
    0,
    'U',
    0,
    'I',
    0,
    'D',
    0,
    's',
    0,
    0,
    0,
    // Set to "DeviceInterfaceGUID" to support WinUSB
    0x50,
    0x00, // wPropertyDataLength
    // WinUSB GUID
    '{',
    0,
    'C',
    0,
    'D',
    0,
    'B',
    0,
    '3',
    0,
    'B',
    0,
    '5',
    0,
    'A',
    0,
    'D',
    0,
    '-',
    0,
    '2',
    0,
    '9',
    0,
    '3',
    0,
    'B',
    0,
    '-',
    0,
    '4',
    0,
    '6',
    0,
    '6',
    0,
    '3',
    0,
    '-',
    0,
    'A',
    0,
    'A',
    0,
    '3',
    0,
    '6',
    0,
    '-',
    0,
    '1',
    0,
    'A',
    0,
    'A',
    0,
    'E',
    0,
    '4',
    0,
    '6',
    0,
    '4',
    0,
    '6',
    0,
    '3',
    0,
    '7',
    0,
    '7',
    0,
    '6',
    0,
    '}',
    0,
    0,
    0,
    0,
    0,
    // identify a CMSIS-DAP V2 configuration,
    // must set to "{CDB3B5AD-293B-4663-AA36-1AAE46463776}"

};

#define EPNUM_CDC_IN 2
#define EPNUM_CDC_OUT 2
#define EPNUM_VENDOR_IN 3
#define EPNUM_VENDOR_OUT 3

#define USB_CLASS_HID 3
#define USB_DT_HID 0x21
#define USB_DT_ENDPOINT 5
#define USB_ENDPOINT_ATTR_INTERRUPT 0x03
#define USB_HID_MAX_PACKET_SIZE 64
#define USB_DT_INTERFACE 4
#define USB_DT_CONFIGURATION 2
#define USB_ENDPOINT_ATTR_BULK 0x02

static const CONFIG_CMSIS_DAP configuration_cmsis_dap = {
    .dev_conf0 =
        {
            /* Configuration Descriptor */
            .bLength = 0x09, //sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
            .bDescriptorType = USB_DT_CONFIGURATION, // CONFIGURATION descriptor type
            .wTotalLength = sizeof(CONFIG_CMSIS_DAP), // Total length of data for this cfg
            .bNumInterfaces = 1, // Number of interfaces in this cfg
            .bConfigurationValue = 1, // Index value of this configuration
            .iConfiguration = 0, // Configuration string index
            .bmAttributes = 0x80, //0xC0 ?
            .bMaxPower = 250, // Max power consumption (2X mA)
        },
    .dev_inface =
        {
            .bLength = 0x09, //sizeof(USB_INTF_DSC),
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 0,
            .bAlternateSetting = 0,
            .bNumEndpoints = 3,
            .bInterfaceClass = 0xff, //custom if0 class
            .bInterfaceSubClass = 0,
            .bInterfaceProtocol = 0,
            .iInterface = 0,
        },
    // Standard Endpoint Descriptor

    // Endpoint 1: Bulk Out – used for commands received from host PC.
    // Endpoint 2: Bulk In – used for responses send to host PC.
    // Endpoint 3: Bulk In (optional) – used for streaming SWO trace

    // ATTENTION:
    // physical endpoint 1 indeed included two "endpoints": Bulk OUT and Bulk IN
    // physical endpoint 1 -> Endpoint 1 & Endpoint 2
    // physical endpoint 2 -> Endpoint 3

    // See also :
    // http://www.keil.com/pack/doc/CMSIS/DAP/html/group__DAP__ConfigUSB__gr.html

    /*                 Pysical endpoint 1                 */
    .dev_ep0 =
        {
            .bLength = 0x07, //sizeof(USB_ENDPOINT_DESCRIPTOR),//0x07,/*sizeof(USB_EP_DSC)*/
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = 0x1,
            .bmAttributes = USB_ENDPOINT_ATTR_BULK,
            .wMaxPacketSize = 512, //USB_HID_MAX_PACKET_SIZE,  //may be 512 cahill
            .bInterval = 0x0,
        },
    .dev_ep1 =
        {
            .bLength = 0x07, //sizeof(USB_ENDPOINT_DESCRIPTOR),//0x07,/*sizeof(USB_EP_DSC)*/
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = 0x81,
            .bmAttributes = USB_ENDPOINT_ATTR_BULK,
            .wMaxPacketSize = 512, //USB_HID_MAX_PACKET_SIZE,
            .bInterval = 0x0,
        },
    .dev_ep2 =
        {
            .bLength = 0x07, //sizeof(USB_ENDPOINT_DESCRIPTOR),//0x07,/*sizeof(USB_EP_DSC)*/
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = 0x82,
            .bmAttributes = USB_ENDPOINT_ATTR_BULK,
            .wMaxPacketSize = 512, //USB_HID_MAX_PACKET_SIZE,
            .bInterval = 0x0,
        },
};

// static const char *strings[] = { "Test", "USB CDC", "Virtual USB" };
static const char* strings[] = {"Test", "USBIP-CMSIS-DAP", "Virtual USB"};

// #define BSIZE 64

// static char	buffer[BSIZE+1];
static int bsize = 0;

//for call dap_process_request include "dap.h"
// #include "dap.h"
// #include "freertos/ringbuf.h"


#define DAPBUFFSIZE 512
uint8_t dap_in[DAPBUFFSIZE + 1];
uint8_t dap_out[DAPBUFFSIZE + 1];
size_t dap_respone = 0;
static int dapResponePkgs = 0;
// DAP handle
static RingbufHandle_t dap_dataIN_handle = NULL; //for usb -> dap,
static RingbufHandle_t dap_dataOUT_handle = NULL; //for dap -> usb,
static SemaphoreHandle_t data_response_mux = NULL;
#define DAP_BUFFER_NUM 10
TaskHandle_t kDAPTaskHandle = NULL;

struct usbip2dapPkg {
    int32_t len;
    uint8_t buf[DAPBUFFSIZE];
    USBIP_CMD_SUBMIT submit;
};

#define DAP_HANDLE_SIZE (sizeof(struct usbip2dapPkg) + sizeof(USBIP_CMD_SUBMIT))



void show_cmd_submit(USBIP_CMD_SUBMIT* cmd_submit) {
    unsigned short ep;
    int i;
    ESP_LOGI(TAG, "command:%d", cmd_submit->command);
    ESP_LOGI(TAG, "seqnum:%d", cmd_submit->seqnum);
    ESP_LOGI(TAG, "devid:%d", cmd_submit->devid);
    ESP_LOGI(TAG, "direction:%d", cmd_submit->direction);
    ESP_LOGI(TAG, "ep:%d", cmd_submit->ep);
    ESP_LOGI(TAG, "transfer_flags:%d", cmd_submit->transfer_flags);
    ESP_LOGI(TAG, "transfer_buffer_length:%d", cmd_submit->transfer_buffer_length);
    ESP_LOGI(TAG, "start_frame:%d", cmd_submit->start_frame);
    ESP_LOGI(TAG, "number_of_packets:%d", cmd_submit->number_of_packets);
    ESP_LOGI(TAG, "interval:%d", cmd_submit->interval);
    ESP_LOGI(TAG, "----------");
}

struct usbip2dapPkg u2dPkg, d2uPkg;
bool shuld_dap_exec = false;
int32_t cnt = 0;
#if 0
static bool handle_non_control_transfer(vstub_t* vstub, USBIP_CMD_SUBMIT* cmd_submit) {
    //show_cmd_submit(cmd_submit);
    if(cmd_submit->ep == 0x01) {
        
        if(cmd_submit->direction == 0) {
            ESP_LOGI(TAG, "direction=input");
            //input
            // memset(dap_in, 0, DAPBUFFSIZE);
            // memset(dap_out, 0, DAPBUFFSIZE);
            memset(u2dPkg.buf, 0, DAPBUFFSIZE);
            // memcpy((uint8_t*)&(u2dPkg.submit), (uint8_t*)cmd_submit, sizeof(USBIP_CMD_SUBMIT));
            if(!recv_data(vstub, (char*)u2dPkg.buf, cmd_submit->transfer_buffer_length)) {
                ESP_LOGE(TAG, "recv_data err");
                return false;
            }

            bsize = cmd_submit->transfer_buffer_length;
            u2dPkg.len = cmd_submit->transfer_buffer_length;
            //esp_log_buffer_hex(TAG, dap_in, bsize);
            // dap_process_request(uint8_t *req, int req_size, uint8_t *resp, int resp_size);
            if(shuld_dap_exec) {
                ESP_LOGE(TAG, "usbip dap message override");
            }

            xRingbufferSend(dap_dataIN_handle, &u2dPkg, DAP_HANDLE_SIZE, portMAX_DELAY);
            xTaskNotifyGive(kDAPTaskHandle);
            reply_cmd_submit(vstub, cmd_submit, NULL, 0);
            if(xSemaphoreTake(data_response_mux, portMAX_DELAY) == pdTRUE) {
                ++dapResponePkgs;
                xSemaphoreGive(data_response_mux);
            }
            // do {
            //     dap_respone = dap_process_request(dap_in, DAPBUFFSIZE, dap_out, DAPBUFFSIZE);
            //     shuld_dap_exec = true;
            //     reply_cmd_submit(vstub, cmd_submit, NULL, 0);
            //     ESP_LOGI(TAG, "direction=input size(%d) num(%d)", dap_respone, cmd_submit->seqnum);
            //     if(cnt != (cmd_submit->seqnum)) {
            //         ESP_LOGE(
            //             TAG, "in cnt(%d) != (cmd_submit->seqnum (%d))", cnt, cmd_submit->seqnum);
            //     }
            //     cnt = cmd_submit->seqnum + 1;
            // } while(0);

        } else {  //cmd_submit->direction = 1
            ESP_LOGI(TAG, "direction=output " );
            struct usbip2dapPkg* pdap_out;
            size_t packetSize;
            // if(dapResponePkgs <= 0) {
            //     reply_cmd_submit(vstub, cmd_submit, 0, 0);
            //     return true;
            // }
            pdap_out = xRingbufferReceiveUpTo(
                dap_dataOUT_handle, &packetSize, pdMS_TO_TICKS(1500), DAP_HANDLE_SIZE);
            if(packetSize != DAP_HANDLE_SIZE) {
                cmd_submit->transfer_buffer_length = 0;
                reply_cmd_submit(vstub, cmd_submit, 0, 0);
                reply_cmd_submit_noop(vstub, cmd_submit);
                ESP_LOGE(TAG, "No Message can be response !!!");
            } else {
                // memcpy(cmd_submit, (uint8_t*)(&pdap_out->submit), sizeof(USBIP_CMD_SUBMIT));
                // cmd_submit->seqnum+=1;
                ESP_LOGW(TAG, "REAL RESPONSE");
                // pdap_out->submit.seqnum += 1;
                
                

                //reply_cmd_submit(vstub, (USBIP_CMD_SUBMIT*)(&(pdap_out->submit)), (char*)pdap_out->buf, pdap_out->len);
                reply_cmd_submit(vstub, cmd_submit, (char*)pdap_out->buf, pdap_out->len);
                //show_cmd_submit((USBIP_CMD_SUBMIT*)(&(pdap_out->submit)));
                
                vRingbufferReturnItem(dap_dataOUT_handle, (void*)pdap_out); // process done.
            }

            // do {
            //     if(shuld_dap_exec) {
            //         //dap_respone = dap_process_request(dap_in, DAPBUFFSIZE, dap_out, DAPBUFFSIZE);
            //         shuld_dap_exec = false;
            //         cmd_submit->transfer_buffer_length = dap_respone;
            //         reply_cmd_submit(vstub, cmd_submit, (char*)dap_out, dap_respone);
            //         dap_respone = 0;
            //     } else {
            //         cmd_submit->transfer_buffer_length = 0;
            //         reply_cmd_submit(vstub, cmd_submit, 0, 0);
            //         ESP_LOGE(TAG, "No Message can be response !!!");
            //     }
            //     if(cnt != (cmd_submit->seqnum)) {
            //         ESP_LOGE(
            //             TAG, "out cnt(%d) != (cmd_submit->seqnum (%d))", cnt, cmd_submit->seqnum);
            //     }
            //     cnt = cmd_submit->seqnum + 1;
            // } while(0);
        }
        //not supported

        //usleep(500);
        return true;
    } else {
        ESP_LOGE(TAG, "handle_non_control_transfer, reques ep>1");
    }

    return false;
}
#endif //0

typedef struct _LINE_CODING {
    word dwDTERate; //in bits per second
    byte bCharFormat; //0-1 stop; 1-1.5 stop; 2-2 stop bits
    byte ParityType; //0 none; 1- odd; 2 -even; 3-mark; 4 -space
    byte bDataBits; //5,6,7,8 or 16
} LINE_CODING;

LINE_CODING linec;
unsigned short linecs = 0;


static bool handle_non_control_transfer(vstub_t* vstub, USBIP_CMD_SUBMIT* cmd_submit) {

}

static bool handle_control_transfer(vstub_t* vstub, USBIP_CMD_SUBMIT* cmd_submit) {
    setup_pkt_t* setup_pkt = (setup_pkt_t*)cmd_submit->setup;

    if(setup_pkt->bmRequestType != 0x21) {
        ESP_LOGE(
            TAG, "handle_control_transfer unknow bmRequestType(0x%x)", setup_pkt->bmRequestType);
        return false;
    }

    //Abstract Control Model Requests
    switch(setup_pkt->bRequest) {

    case 0x0a: //set idle
        ESP_LOGI(TAG, "SET IDLE *");
        reply_cmd_submit(vstub, cmd_submit, 0, 0);
        break;
    default:
        ESP_LOGE(TAG, "unknow setup_pkt->bRequest(0x%x)", setup_pkt->bRequest);
        return false;
    }
    return true;
}

void usbip_dap_thread(void* argument) {
    size_t packetSize;
    struct usbip2dapPkg* pdap_in;
    int32_t dap_respone = 0;
    int32_t loopN = 0;
    while(1) {
        while(1) {
            ulTaskNotifyTake(pdFALSE, portMAX_DELAY);  // wait event
            // should exec dap_respone = dap_process_request(dap_in, DAPBUFFSIZE, dap_out, DAPBUFFSIZE);
            //vTaskDelay(1000 / portTICK_PERIOD_MS); // delay a moment
            if(dapResponePkgs >= 0) {
                pdap_in = (struct usbip2dapPkg*)xRingbufferReceiveUpTo(
                    dap_dataIN_handle, &packetSize, pdMS_TO_TICKS(200), DAP_HANDLE_SIZE);
                // pdap_in = (struct usbip2dapPkg *)xRingbufferReceive(
                //     dap_dataIN_handle, &packetSize, pdMS_TO_TICKS(1));
                if(packetSize != DAP_HANDLE_SIZE) {
                    break;
                } else { //this is correct (packetSize == DAP_HANDLE_SIZE)

                    ESP_LOGE(
                        TAG, "USBIP_DAP_THREAD ... >>> %d >>packetSize: %d", loopN++, packetSize);
                    //printf("pdap_in addr:0x%x ", (uint32_t)pdap_in);
                    ESP_LOGE(TAG, "recieve:[0]:%d, [511]:%d", pdap_in->buf[0], pdap_in->buf[511]);
                    dap_respone =
                        dap_process_request(pdap_in->buf, DAPBUFFSIZE, d2uPkg.buf, DAPBUFFSIZE);
                    d2uPkg.len = dap_respone;
                    // d2uPkg.submit.seqnum += 1;
                    // d2uPkg.submit.direction
                    // d2uPkg.submit.transfer_buffer_length = dap_respone;
                    //memcpy((uint8_t*)&(u2dPkg.submit), (uint8_t*)cmd_submit, sizeof(USBIP_CMD_SUBMIT));
                    // memcpy((uint8_t*)&(d2uPkg.submit), (uint8_t*)(&pdap_in->submit),sizeof(USBIP_CMD_SUBMIT));
                    vRingbufferReturnItem(dap_dataIN_handle, (void*)pdap_in); // process done.
                    
                    xRingbufferSend(
                        dap_dataOUT_handle, (uint8_t*)(&d2uPkg), DAP_HANDLE_SIZE, portMAX_DELAY);
                    if(xSemaphoreTake(data_response_mux, portMAX_DELAY) == pdTRUE) {
                        --dapResponePkgs;
                        xSemaphoreGive(data_response_mux);
                    }
                }
            } else {
                ESP_LOGI(TAG, "usb_dap_task waitting ...");
                vTaskDelay(100 / portTICK_PERIOD_MS); // delay one second,for other task run
            }
        }
    }
}

void usbip_dap_thread_test(void* argument) {
    size_t packetSize;
    struct usbip2dapPkg* pdap_in;
    int32_t dap_respone = 0;
    int32_t loopN = 0;
    int32_t i = 0;
    while(1) {
        while(1) {
            vTaskDelay(10 / portTICK_PERIOD_MS); // delay a moment
            for(i = 0; i < 512; i++) {
                u2dPkg.buf[i] = i + loopN;
            }
            u2dPkg.len = 512;
            xRingbufferSend(dap_dataIN_handle, &u2dPkg, DAP_HANDLE_SIZE, portMAX_DELAY);

            ESP_LOGI(TAG, "USBIP_DAP_THREAD send ... >>> %d", loopN++);
            
        }
    }
}



vstubmod_t vstubmod_cmsis_dap = {
    .code = "cdc",
    .desc = "CDC ACM",
    .dev_dsc = &dev_dsc,
    .dev_qua = &dev_qua,
    // .conf = (CONFIG_GEN *)&configuration_cdc,
    .conf = (CONFIG_GEN*)&configuration_cmsis_dap,
    .n_strings = 3,
    .strings = strings,
    .handler_get_status = NULL,
    .handler_control_transfer = handle_control_transfer,
    .handler_non_control_transfer = handle_non_control_transfer,
    .desc_bos = usbip_dap_link_desc_bos,
    .ms0s20Des = msOs20DescriptorSetHeader};
