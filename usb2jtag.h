#ifndef __USB2JTAG_H__
#define __USB2JTG_H__

#include "windows.h"

/* system includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_E(str, ...) printf(str "\n", ##__VA_ARGS__)
#ifdef _DEBUG
#define LOG_D(str, ...) printf(str "\n", ##__VA_ARGS__)
#else
#define LOG_D(str,...)
#endif

#define JTAGIO_STA_OUT_TDI (0x10)
#define JTAGIO_STA_OUT_TMS (0x02)
#define JTAGIO_STA_OUT_TCK (0x01)

#define TDI_H JTAGIO_STA_OUT_TDI
#define TDI_L 0
#define TMS_H JTAGIO_STA_OUT_TMS
#define TMS_L 0
#define TCK_H JTAGIO_STA_OUT_TCK
#define TCK_L 0

#define HW_TDO_BUF_SIZE 4096
#define SF_PACKET_BUF_SIZE 51200         // 命令包长度
#define CMDPKT_DATA_MAX_BYTES_USBFS 59   // USB全速时每个命令包内包含数据长度
#define USB_PACKET_USBFS 64              // USB全速时单包最大数据长度
#define CMDPKT_DATA_MAX_BYTES_USBHS 507  // USB高速时每个命令包内包含数据长度
#define USB_PACKET_USBHS 512             // USB高速时单包最大数据长度

#define USB20Jtag_CMD_HEADER 3  // 协议包头长度

#define USB20Jtag_CMD_INFO_RD \
  0xCA  // 参数获取,用于获取固件版本、JTAG接口相关参数等
#define USB20Jtag_CMD_JTAG_INIT 0xD0           // JTAG接口初始化命令
#define USB20Jtag_CMD_JTAG_BIT_OP 0xD1         // JTAG接口引脚位控制命令
#define USB20Jtag_CMD_JTAG_BIT_OP_RD 0xD2      // JTAG接口引脚位控制并读取命令
#define USB20Jtag_CMD_JTAG_DATA_SHIFT 0xD3     // JTAG接口数据移位命令
#define USB20Jtag_CMD_JTAG_DATA_SHIFT_RD 0xD4  // JTAG接口数据移位并读取命令

#define USB_DEVICE_DESCRIPTOR_TYPE 0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE 0x02
#define USB_STRING_DESCRIPTOR_TYPE 0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE 0x05

#define USB_ENDPOINT_TYPE_MASK 0x03
#define USB_ENDPOINT_TYPE_CONTROL 0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS 0x01
#define USB_ENDPOINT_TYPE_BULK 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT 0x03
#define USB_ENDPOINT_DIRECTION_MASK 0x80

#pragma pack(1)
typedef struct _USB_COMMON_DESCRIPTOR {
  unsigned char bLength;
  unsigned char bDescriptorType;
} USB_COMMON_DESCRIPTOR, *PUSB_COMMON_DESCRIPTOR;

typedef struct _USB_DEVICE_DESCRIPTOR {
  unsigned char bLength;
  unsigned char bDescriptorType;
  unsigned short bcdUSB;
  unsigned char bDeviceClass;
  unsigned char bDeviceSubClass;
  unsigned char bDeviceProtocol;
  unsigned char bMaxPacketSize0;
  unsigned short idVendor;
  unsigned short idProduct;
  unsigned short bcdDevice;
  unsigned char iManufacturer;
  unsigned char iProduct;
  unsigned char iSerialNumber;
  unsigned char bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

typedef struct _USB_ENDPOINT_DESCRIPTOR {
  unsigned char bLength;
  unsigned char bDescriptorType;
  unsigned char bEndpointAddress;
  unsigned char bmAttributes;
  unsigned short wMaxPacketSize;
  unsigned char bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

typedef struct _USB_CONFIGURATION_DESCRIPTOR {
  unsigned char bLength;
  unsigned char bDescriptorType;
  unsigned short wTotalLength;
  unsigned char bNumInterfaces;
  unsigned char bConfigurationValue;
  unsigned char iConfiguration;
  unsigned char bmAttributes;
  unsigned char MaxPower;
} USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

typedef struct _USB_INTERFACE_DESCRIPTOR {
  unsigned char bLength;
  unsigned char bDescriptorType;
  unsigned char bInterfaceNumber;
  unsigned char bAlternateSetting;
  unsigned char bNumEndpoints;
  unsigned char bInterfaceClass;
  unsigned char bInterfaceSubClass;
  unsigned char bInterfaceProtocol;
  unsigned char iInterface;
} USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

typedef struct _USBCMD_PKT {  // 定义与硬件通讯的命令接口结构
  unsigned char mFunction;    // 输入时指定功能代码
  unsigned short mLength;     // 后续数据长度
  union {
    unsigned char mStatus;       // 输出时返回操作状态
    unsigned char mBuffer[512];  // 数据缓冲区,默认长度为0至512B
  };
} mUSB_HW_COMMAND, *mPUSB_HW_COMMAND;

typedef struct _PIN_STATUS {  // 记录USB20JTAG引脚状态
  int TMS;
  int TDI;
  int TCK;
} JtagPinStatus;
#pragma pack()

typedef int(__stdcall* pCH375OpenDevice)(unsigned long iIndex);
typedef void(__stdcall* pCH375CloseDevice)(unsigned long iIndex);
typedef unsigned long(__stdcall* pCH375SetTimeoutEx)(
    unsigned long iIndex,  // 指定设备序号
    unsigned long
        iWriteTimeout,  // 指定USB写出数据块的超时时间,以毫秒mS为单位,0xFFFFFFFF指定不超时(默认值)
    unsigned long
        iReadTimeout,  // 指定USB读取数据块的超时时间,以毫秒mS为单位,0xFFFFFFFF指定不超时(默认值)
    unsigned long
        iAuxTimeout,  // 指定USB辅助下传数据的超时时间,以毫秒mS为单位,0xFFFFFFFF指定不超时(默认值)
    unsigned long
        iInterTimeout);  // 指定USB中断上传数据的超时时间,以毫秒mS为单位,0xFFFFFFFF指定不超时(默认值)
typedef unsigned long(__stdcall* pCH375SetBufUploadEx)(  // 设定内部缓冲上传模式
    unsigned long iIndex,  // 指定设备序号,0对应第一个设备
    unsigned long
        iEnableOrClear,  // 为0则禁止内部缓冲上传模式,使用直接上传,非0则启用内部缓冲上传模式并清除缓冲区中的已有数据
    unsigned long iPipeNum,  // 端点号，有效值为1到8
    unsigned long BufSize);  // 每包缓冲区大小，最大4MB
typedef unsigned long(
    __stdcall*
        pCH375ClearBufUpload)(  // 清空内部缓冲区，无需暂停内部缓冲上传模式
    unsigned long iIndex,       // 指定设备序号,0对应第一个设备
    unsigned long iPipeNum);    // 端点号，有效值为1到8
typedef unsigned long(
    __stdcall*
        pCH375QueryBufUploadEx)(  // 查询内部上传缓冲区中的已有数据包个数和总字节数,成功返回TRUE,失败FALSE
    unsigned long iIndex,       // 指定设备序号,0对应第一个设备
    unsigned long iPipeNum,     // 端点号，有效值为1到8
    unsigned long* oPacketNum,  // 返回内部缓冲中的已有数据包个数
    unsigned long* oTotalLen);  // 返回内部缓冲中的已有数据包总字节数
typedef unsigned long(__stdcall* pCH375GetConfigDescr)(
    unsigned long iIndex,  // 指定CH375设备序号
    void* oBuffer,         // 指向一个足够大的缓冲区,用于保存描述符
    unsigned long*
        ioLength);  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度
typedef unsigned long(__stdcall* pCH375WriteData)(
    unsigned long iIndex,  // 指定设备序号
    void* oBuffer,         // 指向一个足够大的缓冲区,用于保存描述符
    unsigned long*
        ioLength);  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度
typedef unsigned long(__stdcall* pCH375ReadData)(
    unsigned long iIndex,  // 指定设备序号
    void* oBuffer,         // 指向一个足够大的缓冲区,用于保存描述符
    unsigned long*
        ioLength);  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度
typedef unsigned long(__stdcall* pCH375ReadEndP)(
    unsigned long iIndex,    // 指定设备序号
    unsigned long iPipeNum,  // 端点号，有效值为1到8。
    void* iBuffer,           // 指向一个足够大的缓冲区,用于保存读取的数据
    unsigned long*
        ioLength);  // 指向长度单元,输入时为准备读取的长度,返回后为实际读取的长度
typedef unsigned long(__stdcall* pCH375WriteEndP)(
    unsigned long iIndex,    // 指定设备序号
    unsigned long iPipeNum,  // 端点号，有效值为1到8。
    void* iBuffer,           // 指向一个缓冲区,放置准备写出的数据
    unsigned long*
        ioLength);  // 指向长度单元,输入时为准备写出的长度,返回后为实际写出的长度
HMODULE hModule;
BOOL gOpen;
unsigned long gIndex;
pCH375OpenDevice pOpenDev;
pCH375CloseDevice pCloseDev;
pCH375SetTimeoutEx pSetTimeout;
pCH375SetBufUploadEx pSetBufUpload;
pCH375ClearBufUpload pClearBufUpload;
pCH375QueryBufUploadEx pQueryBufUploadEx;
pCH375ReadData pReadData;
pCH375WriteData pWriteData;
pCH375ReadEndP pReadDataEndP;
pCH375WriteEndP pWriteDataEndP;
pCH375GetConfigDescr pGetConfigDescr;

bool AfxDevIsOpened;  // 设备是否打开
bool AfxUsbHighDev = true;
unsigned long USB_PACKET;
unsigned long MaxBitsPerBulk = 0;
unsigned long MaxBytesPerBulk = 0;
unsigned long CMDPKT_DATA_MAX_BYTES =
    CMDPKT_DATA_MAX_BYTES_USBHS;  // USB PKT由命令码(1B)+长度(2B)+数据
unsigned long CMDPKT_DATA_MAX_BITS =
    (CMDPKT_DATA_MAX_BYTES_USBHS /
     2);  // USB PKT每包最大数据位数，每个数据位需两个TCK由低到高的位带组成。
unsigned char DataUpEndp = 0, DataDnEndp = 0;
unsigned short BulkInEndpMaxSize = 512, BulkOutEndpMaxSize = 512;

static char* HexToString(uint8_t* buf, unsigned int size) {
  unsigned int i;
  char* str = calloc(size * 2 + 1, 1);

  for (i = 0; i < size; i++) sprintf(str + 2 * i, "%02x ", buf[i]);
  return str;
}

int USB20Jtag_Write(void* oBuffer, unsigned long* ioLength) {
  unsigned long wlength = *ioLength;
  int ret = pWriteData(gIndex, oBuffer, ioLength);
  LOG_D("(size=%d, DataDnEndp=%d, buf=[%s]) -> %d", wlength, DataDnEndp,
        HexToString((uint8_t*)oBuffer, *ioLength), *ioLength);
  return ret;
}

int USB20Jtag_Read(void* oBuffer, unsigned long* ioLength) {
  unsigned long rlength = *ioLength, packetNum, bufferNum, RI, RLen, WaitT = 0,
                timeout = 20;
  int ret = false;

  // 单次读取最大允许读取4096B数据，超过则按4096B进行计算
  if (rlength > HW_TDO_BUF_SIZE) rlength = HW_TDO_BUF_SIZE;

  RI = 0;
  while (1) {
    RLen = 8192;
    if (!pQueryBufUploadEx(gIndex, DataUpEndp, &packetNum, &bufferNum)) break;

    if (!pReadDataEndP(gIndex, DataUpEndp, (uint8_t*)oBuffer + RI, &RLen)) {
      LOG_E("USB20Jtag_Read read data failure.");
      goto Exit;
    }
    RI += RLen;
    if (RI >= *ioLength) break;
    if (WaitT++ >= timeout) break;
    Sleep(1);
  }
  LOG_D("(size=%d, DataDnEndp=%d, buf=[%s]) -> %d", rlength, DataUpEndp,
        HexToString((uint8_t*)oBuffer, *ioLength), *ioLength);
  ret = true;
Exit:
  *ioLength = RI;
  return ret;
}

int USB20Jtag_ClockRateInit(unsigned long Index,unsigned char iClockRate)
{
    unsigned char mBuffer[256] = "";
	unsigned long mLength ,i,DescBufSize;
	bool  RetVal = false;
	unsigned char DescBuf[256] = "";
	unsigned char clearBuffer[8192] = "";
	unsigned long TxLen = 8192;
	PUSB_ENDPOINT_DESCRIPTOR       EndpDesc;
	PUSB_COMMON_DESCRIPTOR         UsbCommDesc;

    if( (iClockRate > 5) )
		goto Exit;

	// 获取的USB速度,默认为480MHz USB2.0高速，如果连接至全速HUB下则为12MHz USB全速。
	DescBufSize = sizeof(DescBuf);
	if( !pGetConfigDescr(gIndex,DescBuf,&DescBufSize))
		goto Exit;

	// 根据USB BULK端点大小来判断。如端点大小为512B，则为480MHz USB2.0高速
	AfxUsbHighDev = false;
	i = 0;
	while(i < DescBufSize)
	{
		UsbCommDesc = (PUSB_COMMON_DESCRIPTOR)&DescBuf[i];
		if( UsbCommDesc->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE )
		{
			EndpDesc = (PUSB_ENDPOINT_DESCRIPTOR)&DescBuf[i];
			if( (EndpDesc->bmAttributes&0x03)==USB_ENDPOINT_TYPE_BULK )
			{
				if((EndpDesc->bEndpointAddress&USB_ENDPOINT_DIRECTION_MASK))
				{
					DataUpEndp = EndpDesc->bEndpointAddress&(~USB_ENDPOINT_DIRECTION_MASK);
					BulkInEndpMaxSize = EndpDesc->wMaxPacketSize;			// 端点大小
					AfxUsbHighDev = (EndpDesc->wMaxPacketSize == 512);		// USB速度类型
					
				}
				else
				{
					BulkOutEndpMaxSize = EndpDesc->wMaxPacketSize;
					DataDnEndp = EndpDesc->bEndpointAddress;				
				}
			}
		}
		i += UsbCommDesc->bLength;
	}
	// 根据USB速度,设置每个命令包最大数据长度		
	if(AfxUsbHighDev)
	{
		USB_PACKET 			   =  USB_PACKET_USBHS;
		CMDPKT_DATA_MAX_BYTES  =  CMDPKT_DATA_MAX_BYTES_USBHS; 				//507B
	}
	else				
	{
		USB_PACKET			   =  USB_PACKET_USBFS;
		CMDPKT_DATA_MAX_BYTES  =  CMDPKT_DATA_MAX_BYTES_USBFS; 				//59B
	}
	CMDPKT_DATA_MAX_BITS = CMDPKT_DATA_MAX_BYTES/16*16/2;  					//每个命令所传输的最大位数，每位需由两个字节表示，取2字节的整数倍

	// 根据硬件缓冲区大小计算每批量传输传输的位数,多命令拼包
	MaxBitsPerBulk = HW_TDO_BUF_SIZE/CMDPKT_DATA_MAX_BYTES*CMDPKT_DATA_MAX_BITS;  
	// 根据硬件缓冲区大小计算每批量传输传输的字数,多命令拼包
	MaxBytesPerBulk = HW_TDO_BUF_SIZE - (HW_TDO_BUF_SIZE+CMDPKT_DATA_MAX_BYTES-1)/CMDPKT_DATA_MAX_BYTES*3;;

	// USB BULKIN上传数据采用驱动缓冲上传方式,较直接上传效率更高
	pSetBufUpload(gIndex, true, DataUpEndp, 4096);							//指上传端点为缓冲上传模式，缓冲区大小为8192
	pClearBufUpload(gIndex, DataUpEndp);									//清空驱动内缓冲区数据

	if (!USB20Jtag_Read(clearBuffer, &TxLen))								//读取硬件缓冲区数据
	{	
		LOG_E("USB20Jtag_WriteRead read usb data failure.");
		return 0;
	}

	//构建USB JTAG初始化命令包，并执行
	i = 0;
	mBuffer[i++] = USB20Jtag_CMD_JTAG_INIT;
	mBuffer[i++] = 6;
	mBuffer[i++] = 0;
	mBuffer[i++] = 0;														//保留字节
	mBuffer[i++] = iClockRate;												//JTAG时钟速度
	i += 4; 																//保留字节
	mLength = i;
	if( !USB20Jtag_Write(mBuffer,&mLength) || (mLength!=i) )
		goto Exit;

	// 读取返回值并判断初始化是否成功
	mLength = 4;
	if( !USB20Jtag_Read(mBuffer,&mLength) || (mLength!=4) ) 
	{
		LOG_E("USB20Jtag clock initialization failed.");
		goto Exit;
	}
	RetVal = ( (mBuffer[0] == USB20Jtag_CMD_JTAG_INIT) && (mBuffer[USB20Jtag_CMD_HEADER]==0) );
Exit:
	return (RetVal);
}

int usb20jtag_init(void)
{ 
    if(hModule == 0)
	{
		hModule = LoadLibrary("CH375DLL64.DLL");
        if (hModule)
        {
            pOpenDev    		= (pCH375OpenDevice)  GetProcAddress(hModule, "CH375OpenDevice"); 
			pCloseDev   		= (pCH375CloseDevice) GetProcAddress(hModule, "CH375CloseDevice"); 
			pReadData   		= (pCH375ReadData)  GetProcAddress(hModule, "CH375ReadData"); 
			pWriteData  		= (pCH375WriteData) GetProcAddress(hModule, "CH375WriteData"); 
			pReadDataEndP   	= (pCH375ReadEndP)  GetProcAddress(hModule, "CH375ReadEndP"); 
			pWriteDataEndP  	= (pCH375WriteEndP) GetProcAddress(hModule, "CH375WriteEndP"); 
			pSetTimeout 		= (pCH375SetTimeoutEx) GetProcAddress(hModule, "CH375SetTimeout"); 
			pSetBufUpload 		= (pCH375SetBufUploadEx) GetProcAddress(hModule, "CH375SetBufUploadEx"); 
			pClearBufUpload 	= (pCH375ClearBufUpload) GetProcAddress(hModule, "CH375ClearBufUpload"); 
			pQueryBufUploadEx 	= (pCH375QueryBufUploadEx) GetProcAddress(hModule, "CH375QueryBufUploadEx");
            pGetConfigDescr  	= (pCH375GetConfigDescr) GetProcAddress(hModule, "CH375GetConfigDescr"); 
			if(pOpenDev == NULL || pCloseDev == NULL || pSetTimeout == NULL || pSetBufUpload == NULL || pClearBufUpload == NULL || pQueryBufUploadEx == NULL || pReadData == NULL || pWriteData == NULL || pReadDataEndP == NULL || pWriteDataEndP == NULL || pGetConfigDescr == NULL)
			{
				LOG_E("GetProcAddress error ");
				return -1;
			}
        }
        AfxDevIsOpened = pOpenDev(gIndex);
        if (AfxDevIsOpened == false)
        {
            gOpen = false;
            LOG_E("USB20JTAG Open Error.");
            return -1;
        }
		pSetTimeout(gIndex,1000,1000,1000,1000);
        USB20Jtag_ClockRateInit(0, 0);

    }
	return 0;
}

int usb20jtag_quit(void)
{ 
	// 退出前将信号线全部设置为低电平
    uint32_t retlen = 4;
    unsigned char byte[4] = {USB20Jtag_CMD_JTAG_BIT_OP, 0x01, 0x00, 0x00};
    USB20Jtag_Write(byte, &retlen);

    if (AfxDevIsOpened) 
    {
        pCloseDev(gIndex);
        LOG_E("Close the USB20JTAG.");
		AfxDevIsOpened = false;
    }
	return 0;
}

#endif