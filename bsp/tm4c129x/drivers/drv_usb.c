#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
//#include <components.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdbulk.h"

#include "usb_comp_bulk_structs.h"

static struct rt_device_usb _hw_usb;
unsigned char *buf;
uint32_t len;
uint8_t g_pucDescriptorData[DESCRIPTOR_DATA_SIZE];

rt_size_t _usb_init(rt_device_t dev)
{

	int i=0;
	PinoutSet(false, true); 
	for(i=0;i<NUM_BULK_DEVICES;i++)
	{
	   USBBufferInit(&g_sTxBuffer[i]);
	   USBBufferInit(&g_sRxBuffer[i]);
	
	   g_sCompDevice.psDevices[i].pvInstance = USBDBulkCompositeInit(0, &g_psBULKDevice[i], &g_psCompEntries[i]);
	}
   USBDCompositeInit(0, &g_sCompDevice, DESCRIPTOR_DATA_SIZE,g_pucDescriptorData);

    return 0;
}
int which_usb_device(tUSBDBulkDevice *psDevice)
{
	int i=0;
	for(i=0;i<NUM_BULK_DEVICES;i++)
		if(psDevice==&(g_psBULKDevice[i]))
			break;

	return i;
}
uint32_t
TxHandlerBulk(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // We are not required to do anything in response to any transmit event
    // in this example. All we do is update our transmit counter.
    //
    if(ui32Event == USB_EVENT_TX_COMPLETE)
    {
        //g_ui32TxCount += ui32MsgValue;
		rt_kprintf("packet sent\n");
    }
    return(0);
}
uint32_t
RxHandlerBulk(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // Which event are we being sent?
    //
     tUSBDBulkDevice *psDevice;
     psDevice = (tUSBDBulkDevice *)pvCBData;
	 int index=which_usb_device(psDevice);
    switch(ui32Event)
    {
        //
        // We are connected to a host and communication is now possible.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Flush our buffers.
            //
            USBBufferFlush(&g_sTxBuffer[index]);
            USBBufferFlush(&g_sRxBuffer[index]);
			rt_kprintf("usb connect %d\n",index);
            break;
        }

        //
        // The host has disconnected.
        //
        case USB_EVENT_DISCONNECTED:
        {
			rt_kprintf("usb disconnect %d\n",index);
            break;
        }

        //
        // A new packet has been received.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
			/*
			uint32_t ui32ReadIndex = (uint32_t)(pvMsgData - g_ppui8USBRxBuffer[index]);
			buf=rt_malloc(ui32MsgValue*sizeof(unsigned char));
			len=ui32MsgValue;
			uint32_t count=ui32MsgValue;
			int i=0;
			while(count)
			{
				buf[i++]=g_ppui8USBRxBuffer[index][ui32ReadIndex];
				ui32ReadIndex++;
				
				ui32ReadIndex = ((ui32ReadIndex == BULK_BUFFER_SIZE) ? 0 : ui32ReadIndex);
				count--;
			}*/
            //
            // Read the new packet and echo it back to the host.
            //
            return 0;
        }

        //
        // Ignore SUSPEND and RESUME for now.
        //
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            break;

        //
        // Ignore all other events and return 0.
        //
        default:
            break;
    }

    return(0);
}

static rt_size_t _usb_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_memcpy(buffer,buf,len);
    return len;
}

static rt_size_t _usb_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
   

    return size;
}

int rt_device_usb_register(const char *name)
{
   /* _hw_usb.parent.type         = RT_Device_Class_Miscellaneous;
    _hw_usb.parent.rx_indicate  = RT_NULL;
    _hw_usb.parent.tx_complete  = RT_NULL;

    _hw_usb.parent.init         = _usb_init;
    _hw_usb.parent.open         = RT_NULL;
    _hw_usb.parent.close        = RT_NULL;
    _hw_usb.parent.read         = _usb_read;
    _hw_usb.parent.write        = _usb_write;
    _hw_usb.parent.control      = RT_NULL;

    /* register a character device 
    rt_device_register(&_hw_usb.parent, "usb", RT_DEVICE_FLAG_RDWR);
	*/

    return 0;
}

