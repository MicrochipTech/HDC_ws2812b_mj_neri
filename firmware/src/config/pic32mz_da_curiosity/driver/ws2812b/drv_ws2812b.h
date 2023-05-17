//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// drv_ws2182b.h
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Author: MJ Neri - A63063
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Module Name: drv_ws2182b.h
// Description: This file contains function prototypes for Public APIs used by the
//              application.
//
// Revisions:
// Revision 0.01 - File Created (2023-04-10)
// Additional Comments:
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Header Guards
#ifndef _DRV_WS2812B_H_
#define _DRV_WS2812B_H_

// Header files
#include "driver/ws2812b/drv_ws2812b_definitions.h"
#include "driver/ws2812b/src/drv_ws2812b_local.h"

// WS2812B Driver Public APIs
/* Initializes a specific instance of the Driver */
SYS_MODULE_OBJ DRV_WS2812B_Initialize(const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT *const init);

/* Called by a client to open an instance of the driver for application use. Calling this function will result in 
the driver setting up the SPI Driver to a baud rate of 2.4MHz, which is required to communicate properly with the WS2812B
LEDs. */
DRV_HANDLE DRV_WS2812B_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent);

/* Closes the instance of the driver, allowing it to be used by other potential clients. */
void DRV_WS2812B_Close(const DRV_HANDLE handle);

/* Used by the application to register an event handler. */
void DRV_WS2812B_EventHandlerSet(const DRV_HANDLE handle, const DRV_WS2812B_EVENT_HANDLER eventHandler, const uintptr_t context);

/* Calling this function triggers the driver to request a write with the SPI Driver, which eventually sends the framebuffer
contents to the LEDs. Returns false if there is an ongoing transfer, and true if the transfer is scheduled successfully. */
bool DRV_WS2812B_Refresh(const DRV_HANDLE handle);

/* Calling this function triggers the driver to clear the framebuffer contents. Implementation-wise, the framebuffer isn't 
actually 'cleared' (i.e., set to zeros). Rather, encoded data equivalent to WS2812B 0's are stored in the framebuffer instead. */
bool DRV_WS2812B_Clear(const DRV_HANDLE handle);

/* This function is for convenience of the application, and is equivalent to calling DRV_WS2812B_Clear then DRV_WS2812B_Refresh. */
bool DRV_WS2812B_TurnOff(const DRV_HANDLE handle);

/* This function sets the color of the specified LED and updates the framebuffer. ledN starts from 0. */
bool DRV_WS2812B_SetColor(const DRV_HANDLE handle, uint16_t ledN, uint8_t red, uint8_t green, uint8_t blue);

/* This function gets the encoded data from the framebuffer, then decodes it to get the color values.
The result is stored in *color, and is formatted as <8'b0, Green, Red, Blue>. */
bool DRV_WS2812B_GetColor(const DRV_HANDLE handle, uint16_t ledN, uint32_t *color);

/* This function returns the LED array size that the driver obtained during initialization. */
uint16_t DRV_WS2812B_GetArraySize(const DRV_HANDLE handle);

/* This function sets the color of multiple LEDs and updates the framebuffer. firstLED indicates the first LED to be updated, while 
count indicates how many LEDs will be updated. For example, specifying firstLED = 10, and count = 5, means that LEDs 10 to 14 will
be updated. */
bool DRV_WS2812B_Fill(const DRV_HANDLE handle, uint16_t firstLED, uint16_t count, uint8_t red, uint8_t green, uint8_t blue);

#endif /*_DRV_WS2812B_H_*/