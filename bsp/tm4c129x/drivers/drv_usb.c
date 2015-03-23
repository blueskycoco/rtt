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
static struct rt_device_usb _hw_usb;
static rt_size_t _usb_init(rt_device_t dev)
{
    PinoutSet(false, true); 
	USBBufferInit(&g_sTxBuffer);
    USBBufferInit(&g_sRxBuffer);
	USBStackModeSet(0, eUSBModeDevice, 0);
	USBDBulkInit(0, &g_sBulkDevice);
    return 0;
}
uint32_t
TxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // We are not required to do anything in response to any transmit event
    // in this example. All we do is update our transmit counter.
    //
    if(ui32Event == USB_EVENT_TX_COMPLETE)
    {
        g_ui32TxCount += ui32MsgValue;
		rt_kprintf("packet sent\n");
    }
    return(0);
}
uint32_t
RxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // Which event are we being sent?
    //
    switch(ui32Event)
    {
        //
        // We are connected to a host and communication is now possible.
        //
        case USB_EVENT_CONNECTED:
        {
            g_bUSBConfigured = true;
            g_ui32Flags |= COMMAND_STATUS_UPDATE;

            //
            // Flush our buffers.
            //
            USBBufferFlush(&g_sTxBuffer);
            USBBufferFlush(&g_sRxBuffer);

            break;
        }

        //
        // The host has disconnected.
        //
        case USB_EVENT_DISCONNECTED:
        {
            g_bUSBConfigured = false;
            g_ui32Flags |= COMMAND_STATUS_UPDATE;
            break;
        }

        //
        // A new packet has been received.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            tUSBDBulkDevice *psDevice;

            //
            // Get a pointer to our instance data from the callback data
            // parameter.
            //
            psDevice = (tUSBDBulkDevice *)pvCBData;

            //
            // Read the new packet and echo it back to the host.
            //
            return(EchoNewDataToHost(psDevice, pvMsgData, ui32MsgValue));
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
    
    return size;
}

static rt_size_t _usb_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
   

    return size;
}

int rt_device_usb_register(const char *name)
{
    _hw_usb.parent.type         = RT_Device_Class_Miscellaneous;
    _hw_usb.parent.rx_indicate  = RT_NULL;
    _hw_usb.parent.tx_complete  = RT_NULL;

    _hw_usb.parent.init         = _usb_init;
    _hw_usb.parent.open         = RT_NULL;
    _hw_usb.parent.close        = RT_NULL;
    _hw_usb.parent.read         = _usb_read;
    _hw_usb.parent.write        = _usb_write;
    _hw_usb.parent.control      = RT_NULL;

    /* register a character device */
    rt_device_register(&_hw_usb.parent, "usb", RT_DEVICE_FLAG_RDWR);

    return 0;
}

