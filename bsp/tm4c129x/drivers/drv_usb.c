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
extern uint8_t g_ppui8USBRxBuffer[NUM_BULK_DEVICES][UART_BUFFER_SIZE];
extern uint8_t g_ppcUSBTxBuffer[NUM_BULK_DEVICES][UART_BUFFER_SIZE];

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
         int index=which_usb_device((tUSBDBulkDevice *)pvCBData);
		rt_kprintf("packet sent %d,length %d\n",index,ui32MsgValue);
    }
    return(0);
}

//*****************************************************************************
//
// Receive new data and echo it back to the host.
//
// \param psDevice points to the instance data for the device whose data is to
// be processed.
// \param pi8Data points to the newly received data in the USB receive buffer.
// \param ui32NumBytes is the number of bytes of data available to be
// processed.
//
// This function is called whenever we receive a notification that data is
// available from the host. We read the data, byte-by-byte and swap the case
// of any alphabetical characters found then write it back out to be
// transmitted back to the host.
//
// \return Returns the number of bytes of data processed.
//
//*****************************************************************************
static uint32_t
EchoNewDataToHost(int index, uint8_t *pi8Data,
                  uint_fast32_t ui32NumBytes)
{
    uint_fast32_t ui32Loop, ui32Space, ui32Count;
    uint_fast32_t ui32ReadIndex;
    uint_fast32_t ui32WriteIndex;
    tUSBRingBufObject sTxRing;

    //
    // Get the current buffer information to allow us to write directly to
    // the transmit buffer (we already have enough information from the
    // parameters to access the receive buffer directly).
    //
    USBBufferInfoGet(&(g_sTxBuffer[index]), &sTxRing);

    //
    // How much space is there in the transmit buffer?
    //
    ui32Space = USBBufferSpaceAvailable(&(g_sTxBuffer[index]));

    //
    // How many characters can we process this time round?
    //
    ui32Loop = (ui32Space < ui32NumBytes) ? ui32Space : ui32NumBytes;
    ui32Count = ui32Loop;

    //
    // Update our receive counter.
    //
   // g_ui32RxCount += ui32NumBytes;

    //
    // Set up to process the characters by directly accessing the USB buffers.
    //
    ui32ReadIndex = (uint32_t)(pi8Data - g_ppui8USBRxBuffer[index]);
    ui32WriteIndex = sTxRing.ui32WriteIndex;

    while(ui32Loop)
    {
        //
        // Copy from the receive buffer to the transmit buffer converting
        // character case on the way.
        //

        //
        // Is this a lower case character?
        //
        if((g_ppui8USBRxBuffer[index][ui32ReadIndex] >= 'a') &&
           (g_ppui8USBRxBuffer[index][ui32ReadIndex] <= 'z'))
        {
            //
            // Convert to upper case and write to the transmit buffer.
            //
            g_ppcUSBTxBuffer[index][ui32WriteIndex] =
                (g_ppui8USBRxBuffer[index][ui32ReadIndex] - 'a') + 'A';
        }
        else
        {
            //
            // Is this an upper case character?
            //
            if((g_ppui8USBRxBuffer[index][ui32ReadIndex] >= 'A') &&
               (g_ppui8USBRxBuffer[index][ui32ReadIndex] <= 'Z'))
            {
                //
                // Convert to lower case and write to the transmit buffer.
                //
                g_ppcUSBTxBuffer[index][ui32WriteIndex] =
                    (g_ppui8USBRxBuffer[index][ui32ReadIndex] - 'Z') + 'z';
            }
            else
            {
                //
                // Copy the received character to the transmit buffer.
                //
                g_ppcUSBTxBuffer[index][ui32WriteIndex] =
                    g_ppui8USBRxBuffer[index][ui32ReadIndex];
            }
        }

        //
        // Move to the next character taking care to adjust the pointer for
        // the buffer wrap if necessary.
        //
        ui32WriteIndex++;
        ui32WriteIndex =
            (ui32WriteIndex == UART_BUFFER_SIZE) ? 0 : ui32WriteIndex;

        ui32ReadIndex++;

        ui32ReadIndex = ((ui32ReadIndex == UART_BUFFER_SIZE) ?
                         0 : ui32ReadIndex);

        ui32Loop--;
    }

    //
    // We've processed the data in place so now send the processed data
    // back to the host.
    //
    USBBufferDataWritten(&(g_sTxBuffer[index]), ui32Count);

    //
    // We processed as much data as we can directly from the receive buffer so
    // we need to return the number of bytes to allow the lower layer to
    // update its read pointer appropriately.
    //
    return(ui32Count);
}

uint32_t
RxHandlerBulk(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // Which event are we being sent?
    //
    
			unsigned char tmpbuf[1024];
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
			//int i;
			//uint32_t ui32ReadIndex = (uint32_t)(pvMsgData - (void *)(g_ppui8USBRxBuffer[index]));
			//rt_kprintf("usb index %d rxd\n",index);
			//for(i=ui32ReadIndex;i<ui32MsgValue;i++)
			//	rt_kprintf("%d \n",g_ppui8USBRxBuffer[index][i]);
			//USBBufferDataWritten(&(g_sTxBuffer[index]), 10);
			//EchoNewDataToHost(index,pvMsgData,ui32MsgValue);
			//USBBufferFlush(&g_sRxBuffer[index]);
			int bytes=USBBufferRead(&g_sRxBuffer[index],tmpbuf,1024);
			rt_kprintf("read index %d ,bytes %d\n",index,bytes);
			USBBufferWrite(&g_sTxBuffer[index],tmpbuf,bytes);
			/*buf=rt_malloc(ui32MsgValue*sizeof(unsigned char));
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
    return USBBufferRead(&g_sRxBuffer[index],tmpbuf,1024);;
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

