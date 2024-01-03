//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// drv_ws2182b_local.h
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Author: MJ Neri
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Module Name: drv_ws2182b_local.h
// Description: This file contains data structures and type defines that are local
//              to the driver.
//
// Revisions:
// Revision 0.01 - File Created (2023-04-10)
// Additional Comments:
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Header Guards
#ifndef _DRV_WS2812B_LOCAL_H_
#define _DRV_WS2812B_LOCAL_H_

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "../drv_ws2812b_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/* WS2812B Driver Client Object */
typedef struct
{
    /* Indicates if the client object is in use */
    bool inUse;

    /* Event handler registered by the client */
    DRV_WS2812B_EVENT_HANDLER eventHandler;

    /* Driver event passed to the client-registered event handler */
    DRV_WS2812B_EVENT event;

    /* Client-specific context */
    uintptr_t context;

    /* Driver instance associated with the client */
    SYS_MODULE_INDEX drvIndex;

    /* Client handle assigned to the client upon opening the driver */
    DRV_HANDLE clientHandle;

    /* Contains driver handle from the SPI Driver */
    DRV_HANDLE spiDrvHandle;

    /* Contains SPI Driver transfer handle */
    DRV_SPI_TRANSFER_HANDLE spiDrvTransferHandle;

    /* Contains SPI Driver transfer parameters */
    DRV_WS2812B_CONFIG_PARAMS spiTransferParams;

} DRV_WS2812B_CLIENT_OBJ;

/* WS2812B Driver Instance Object */
typedef struct
{
    /* Indicates whether the instance has been initialized */
    bool isInitialized;

    /* Indicates max no. of clients supported by the instance */
    uint8_t maxClients;

    /* Indicates current no. of active clients */
    uint8_t nClients;

    /* Pointer to client object pool for the driver instance */
    DRV_WS2812B_CLIENT_OBJ *clientObjPool;

    /* Indicates whether SPI transfer is ongoing or not */
    bool isTransferInProgress;

    /* Contains driver index for the SPI Driver */
    SYS_MODULE_INDEX spiDrvIndex;

    /* Pointer to array containing LED Data, provided by the client */
    DRV_WS2812B_PIXEL_DATA *pixelBuffer;
    
    /* Contains size of the pixelBuffer array */
    uint32_t pixelBufferSize;

    /* Indicates the size of the LED array */
    uint16_t ledArraySize;

} DRV_WS2812B_OBJ;

#endif /*_DRV_WS2812B_LOCAL_H_*/
