//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// drv_ws2182b_definitions.h
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Author: MJ Neri - A63063
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Module Name: drv_ws2182b_definitions.h
// Description: This file contains data structures and type defines that will be
//              needed by initialization.c. Defined data types and structures can
//              also be used by the application if needed.
//
// Revisions:
// Revision 0.01 - File Created (2023-04-10)
// Additional Comments:
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Header Guards
#ifndef _DRV_WS2812B_DEFINITIONS_H_
#define _DRV_WS2812B_DEFINITIONS_H_

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "stddef.h"
#include "system/system_module.h"
#include "driver/driver.h"
#include "driver/driver_common.h"
#include "system/int/sys_int.h"
#include "driver/spi/drv_spi.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/* NOTE: The struct below is from a different implementation of a WS2812B driver
 * which makes use of a NOR-based logic ckt to convert SPI to WS2812B signals. 
 * It's commented out below because for this implementation, I decided to use a 
 * single uint8_t array to store the data instead. */
//typedef struct
//{
//    uint8_t blue;
//    uint8_t red;
//    uint8_t green;
//
//} DRV_WS2812B_PIXEL_DATA;
typedef uint8_t DRV_WS2812B_PIXEL_DATA;

typedef enum
{
    DRV_WS2812B_WRITE_DONE = 0,
    DRV_WS2812B_ERROR,
} DRV_WS2812B_EVENT;

typedef struct 
{
    /* Taken from DRV_SPI_TRANSFER_SETUP definition in drv_spi_definitions.h */
    uint32_t baudRateInHz;

    DRV_SPI_CLOCK_PHASE clockPhase;

    DRV_SPI_CLOCK_POLARITY clockPolarity;

    DRV_SPI_DATA_BITS dataBits;

    SYS_PORT_PIN chipSelect;

    DRV_SPI_CS_POLARITY csPolarity;
} DRV_WS2812B_TRANSFER_SETUP;

typedef struct
{
    DRV_WS2812B_TRANSFER_SETUP transferParams;
    
} DRV_WS2812B_CONFIG_PARAMS;

typedef void (*DRV_WS2812B_EVENT_HANDLER)(DRV_WS2812B_EVENT event, uintptr_t context);

typedef struct
{
    const uintptr_t clientObj;
    SYS_MODULE_INDEX spiDrvIndex;
    uint8_t maxClients;
    DRV_WS2812B_PIXEL_DATA *pixelBuffer;
    uint32_t pixelBufferSize;
    uint16_t ledArraySize;
} DRV_WS2812B_INIT;

#endif /*_DRV_WS2812B_DEFINITIONS_H_*/