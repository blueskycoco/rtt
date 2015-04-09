//*****************************************************************************
//
// usb_serial_structs.h - Data structures defining this USB CDC device.
//
// Copyright (c) 2009-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#ifndef __USB_SERIAL_STRUCTS_H__
#define __USB_SERIAL_STRUCTS_H__

//*****************************************************************************
//
// The size of the transmit and receive buffers used for the redirected UART.
// This number should be a power of 2 for best performance.  256 is chosen
// pretty much at random though the buffer should be at least twice the size of
// a maximum-sized USB packet.
//
//*****************************************************************************
#define UART_BUFFER_SIZE        4096
#define UART_BUFFER_SIZE_TX     1024

#define NUM_BULK_DEVICES      5
#define DESCRIPTOR_DATA_SIZE    (COMPOSITE_DBULK_SIZE*NUM_BULK_DEVICES)

extern uint32_t RxHandlerBulk(void *pvCBData, uint32_t ui32Event,
                              uint32_t ui32MsgValue, void *pvMsgData);
extern uint32_t RxHandlerCmd(void *pvCBData, uint32_t ui32Event,
                             uint32_t ui32MsgValue, void *pvMsgData);
extern uint32_t TxHandlerBulk(void *pvlCBData, uint32_t ui32Event,
                              uint32_t ui32MsgValue, void *pvMsgData);
extern uint32_t TxHandlerCmd(void *pvlCBData, uint32_t ui32Event,
                             uint32_t ui32MsgValue, void *pvMsgData);

extern uint32_t ControlHandler(void *pvCBData, uint32_t ui32Event,
                               uint32_t ui32MsgValue, void *pvMsgData);
extern uint32_t EventHandler(void *pvCBData, uint32_t ui32Event,
                             uint32_t ui32MsgData, void *pvMsgData);
extern const tUSBBuffer g_sTxBuffer[NUM_BULK_DEVICES];
extern const tUSBBuffer g_sRxBuffer[NUM_BULK_DEVICES];
extern tUSBDBulkDevice g_psBULKDevice[NUM_BULK_DEVICES];
extern uint8_t g_pui8USBTxBuffer[];
extern uint8_t g_pui8USBRxBuffer[];
extern tCompositeEntry g_psCompEntries[NUM_BULK_DEVICES];
extern tUSBDCompositeDevice g_sCompDevice;
extern uint8_t g_pui8DescriptorData[DESCRIPTOR_DATA_SIZE];

#endif // __USB_SERIAL_STRUCTS_H__
