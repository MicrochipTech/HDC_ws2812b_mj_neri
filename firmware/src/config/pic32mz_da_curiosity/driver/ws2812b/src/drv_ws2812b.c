//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// drv_ws2182b.c
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Author: MJ Neri - A63063
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Module Name: drv_ws2182b.c
// Description: This file contains the driver implementation and public APIs
//
// Revisions:
// Revision 0.01 - File Created (2023-04-10)
// Additional Comments:
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "configuration.h"
#include "driver/ws2812b/drv_ws2812b.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

static DRV_WS2812B_OBJ gDrvWS2812BObj[DRV_WS2812B_INSTANCES_NUMBER];

// *****************************************************************************
// *****************************************************************************
// Section: Local Functions
// *****************************************************************************
// *****************************************************************************

// Local functions go here.
static void DRV_WS2812B_EventHandler(DRV_SPI_TRANSFER_EVENT event, DRV_SPI_TRANSFER_HANDLE handle, uintptr_t context)
{
    DRV_WS2812B_CLIENT_OBJ *clientObj = (DRV_WS2812B_CLIENT_OBJ *) context;
    DRV_WS2812B_OBJ *dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    dObj->isTransferInProgress = false;

    if(event == DRV_SPI_TRANSFER_EVENT_COMPLETE)
    {
        clientObj->event = DRV_WS2812B_WRITE_DONE;
        if(clientObj->eventHandler != NULL)
        {
            clientObj->eventHandler(clientObj->event, clientObj->context);
        }
    }
    else
    {
        clientObj->event = DRV_WS2812B_ERROR;
        if (clientObj->eventHandler != NULL)
        {
            clientObj->eventHandler(clientObj->event, clientObj->context);
        }
    }
}

static DRV_WS2812B_CLIENT_OBJ *DRV_WS2812B_ValidateDriverHandle(DRV_HANDLE handle)
{
    // This function returns the pointer to the client object that is associated with 
    // this handle if the handle is valid. Returns NULL otherwise.

    /* Extract the drvIndex value from the handle */
    uint32_t drvIndex = handle >> 8;

    /* Extract the clientIndex value from the handle */
    uint32_t clientIndex = handle & 0xFF;

    if (drvIndex < DRV_WS2812B_INSTANCES_NUMBER)
    {
        if (clientIndex < gDrvWS2812BObj[drvIndex].maxClients)
        {
            if (gDrvWS2812BObj[drvIndex].clientObjPool[clientIndex].inUse == true)
            {
                return &gDrvWS2812BObj[drvIndex].clientObjPool[clientIndex];
            }
        }
    }

    return NULL;
}

static uint32_t DRV_WS2812B_Encode_To_SPI(uint8_t b)
{
    uint32_t temp = 0;
    for (int i = 0; i < 8; i++)
    {
        temp = temp << 3;
        if (b & 0x80)
        {
            temp |= 0b110;
        }
        else
        {
            temp |= 0b100;
        }

        b = b << 1;
    }

    // temp shall be parsed by the calling function as follows:
    // msb = (temp >> 16) & 0xff;
    // midb = (temp >> 8) & 0xff;
    // lsb = temp & 0xff;
    return temp;
}

static uint8_t DRV_WS2812B_Decode_From_SPI(uint32_t w)
{
    uint8_t temp = 0;
    for (int i = 0; i < 8; i++)
    {
        temp = temp << 1;
        if ((w & 0xE00000) == 0xC00000)
        {
            temp |= 0b1;
        }
        else if ((w & 0xE00000) == 0x800000)
        {
            temp |= 0b0;
        }

        w = w << 3;
    }

    return temp;
}

static void _DRV_WS2812B_SetColor(DRV_WS2812B_OBJ *dObj, uint16_t ledN, uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t temp, index;

    /* Compute the array index for the first byte to be accessed */
    index = DRV_WS2812B_PAD_ZERO_AMT + (ledN * DRV_WS2812B_BYTES_PER_LED * DRV_WS2812B_SPI_BYTES_PER_LED_BYTE);

    /* Encode green, then red, then blue. */
    temp = DRV_WS2812B_Encode_To_SPI(green);
    dObj->pixelBuffer[index] = (temp >> 16) & 0xff;
    dObj->pixelBuffer[index + 1] = (temp >> 8) & 0xff;
    dObj->pixelBuffer[index + 2] = temp & 0xff;

    temp = DRV_WS2812B_Encode_To_SPI(red);
    dObj->pixelBuffer[index + 3] = (temp >> 16) & 0xff;
    dObj->pixelBuffer[index + 4] = (temp >> 8) & 0xff;
    dObj->pixelBuffer[index + 5] = temp & 0xff;

    temp = DRV_WS2812B_Encode_To_SPI(blue);
    dObj->pixelBuffer[index + 6] = (temp >> 16) & 0xff;
    dObj->pixelBuffer[index + 7] = (temp >> 8) & 0xff;
    dObj->pixelBuffer[index + 8] = temp & 0xff;

    return;
}

static uint32_t _DRV_WS2812B_GetColor(DRV_WS2812B_OBJ *dObj, uint16_t ledN)
{
    uint32_t temp, index, color;

    /* Compute the array index for the first byte to be accessed */
    index = DRV_WS2812B_PAD_ZERO_AMT + (ledN * DRV_WS2812B_BYTES_PER_LED * DRV_WS2812B_SPI_BYTES_PER_LED_BYTE);

    /* Decode green. */
    temp = (uint32_t)(dObj->pixelBuffer[index] << 16) | \
           (uint32_t)(dObj->pixelBuffer[index + 1] << 8) | \
           (uint32_t)(dObj->pixelBuffer[index + 2]);
    color = DRV_WS2812B_Decode_From_SPI(temp);
    color = color << 8;
    
    /* Decode red. */
    temp = (uint32_t)(dObj->pixelBuffer[index + 3] << 16) | \
           (uint32_t)(dObj->pixelBuffer[index + 4] << 8) | \
           (uint32_t)(dObj->pixelBuffer[index + 5]);
    color |= DRV_WS2812B_Decode_From_SPI(temp);
    color = color << 8;
    
    /* Decode blue. */
    temp = (uint32_t)(dObj->pixelBuffer[index + 6] << 16) | \
           (uint32_t)(dObj->pixelBuffer[index + 7] << 8) | \
           (uint32_t)(dObj->pixelBuffer[index + 8]);
    color |= DRV_WS2812B_Decode_From_SPI(temp);

    return color;
}

// *****************************************************************************
// *****************************************************************************
// Section: WS2812B Driver Implementation
// *****************************************************************************
// *****************************************************************************
SYS_MODULE_OBJ DRV_WS2812B_Initialize(const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT *const init)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_INIT *ws2812bInit = (DRV_WS2812B_INIT *)init;
    uint32_t temp;

    /* Validate the request */
    if (drvIndex >= DRV_WS2812B_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    else
    {
        dObj = &gDrvWS2812BObj[drvIndex];
    }
    
    /* Double-check if the driver was already initialized */
    if(dObj->isInitialized == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* Initialize the driver instance */
    /* Get max clients supported by this driver instance */
    dObj->maxClients = ws2812bInit->maxClients;

    /* No active clients yet */
    dObj->nClients = 0;

    /* Get client object pool to store client data for this instance */
    dObj->clientObjPool = (DRV_WS2812B_CLIENT_OBJ *)ws2812bInit->clientObj;

    /* Initialize some status variables */
    dObj->isTransferInProgress = false;
    dObj->isInitialized = true;

    /* Get LED array and SPI driver properties */
    dObj->pixelBuffer = ws2812bInit->pixelBuffer;
    dObj->pixelBufferSize = ws2812bInit->pixelBufferSize;
    dObj->ledArraySize = ws2812bInit->ledArraySize;
    dObj->spiDrvIndex = ws2812bInit->spiDrvIndex;

    /* Initialize the pixelBuffer */
    memset(dObj->pixelBuffer, 0, dObj->pixelBufferSize);

    /* Skip the first few bytes because we want these to be zeroes */
    for (int i = DRV_WS2812B_PAD_ZERO_AMT; i < dObj->pixelBufferSize; i += 3)
    {
        temp = DRV_WS2812B_Encode_To_SPI(0x0);

        dObj->pixelBuffer[i] = (temp >> 16) & 0xff;
        dObj->pixelBuffer[i+1] = (temp >> 8) & 0xff;
        dObj->pixelBuffer[i+2] = temp & 0xff;
    }

    return (SYS_MODULE_OBJ)drvIndex;
}

DRV_HANDLE DRV_WS2812B_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;

    /* Validate the request */
    if (drvIndex >= DRV_WS2812B_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }
    
    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[drvIndex];

    /* Make sure the driver is initialized before the client can open it. Also, ensure there are still available client slots */
    if (dObj->isInitialized == false || dObj->nClients >= dObj->maxClients)
    {
        return DRV_HANDLE_INVALID;
    }
    else
    {
        /* Get the next free client object */
        for(int32_t i = 0; i < dObj->maxClients; i++)
        {
            if(dObj->clientObjPool[i].inUse == false)
            {
                /* We have a free client object */
                clientObj = &dObj->clientObjPool[i];

                /* Get the client object */
                clientObj->inUse = true;

                /* Initialize the client object */
                clientObj->eventHandler = NULL;
                clientObj->context = (uintptr_t) NULL;
                clientObj->event = 0;
                clientObj->drvIndex = drvIndex;

                /* Format the client handle */
                clientObj->clientHandle = ((clientObj->drvIndex) << 8) | (i);

                /* Open an instance of the SPI Driver and register an event handler */
                clientObj->spiDrvHandle = DRV_SPI_Open(dObj->spiDrvIndex, DRV_IO_INTENT_WRITE);
                DRV_SPI_TransferEventHandlerSet(clientObj->spiDrvHandle, DRV_WS2812B_EventHandler, (uintptr_t)clientObj);

                /* Call DRV_SPI_TransferSetup to force a baudrate of 2.4MHz. Other settings don't matter as much. */
                clientObj->spiTransferParams.transferParams.baudRateInHz = DRV_WS2812B_SPI_BAUD_HZ;
                clientObj->spiTransferParams.transferParams.clockPhase = DRV_SPI_CLOCK_PHASE_VALID_TRAILING_EDGE;
                clientObj->spiTransferParams.transferParams.clockPolarity = DRV_SPI_CLOCK_POLARITY_IDLE_LOW;
                clientObj->spiTransferParams.transferParams.dataBits = DRV_SPI_DATA_BITS_8;
                clientObj->spiTransferParams.transferParams.chipSelect = SYS_PORT_PIN_NONE;
                clientObj->spiTransferParams.transferParams.csPolarity = DRV_SPI_CS_POLARITY_ACTIVE_LOW;
                DRV_SPI_TransferSetup(clientObj->spiDrvHandle, 
                        (DRV_SPI_TRANSFER_SETUP *)&clientObj->spiTransferParams.transferParams);
                
                /* Increment the # of active clients for this WS2812B driver instance */
                dObj->nClients += 1;

                return clientObj->clientHandle;
            }
        }
    }
    
    return DRV_HANDLE_INVALID;
}

void DRV_WS2812B_Close(const DRV_HANDLE handle)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;

    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if(clientObj == NULL)
    {
        return;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    /* Check if there is a pending transfer before proceeding */
    if(dObj->isTransferInProgress)
    {
        return;
    }

    /* Reduce the number of clients */
    dObj->nClients--;

    /* Invalidate the client handle */
    clientObj->clientHandle = DRV_HANDLE_INVALID;

    /* Remove the registered event handler */
    clientObj->eventHandler = NULL;

    /* Free the client object */
    clientObj->inUse = false;

    return;
}

bool DRV_WS2812B_Refresh(const DRV_HANDLE handle)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;

    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return false;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    /* Return if there is a pending transfer */
    if (dObj->isTransferInProgress)
    {
        return false;
    }

    /* Schedule the SPI Transfer */
    DRV_SPI_WriteTransferAdd(clientObj->spiDrvHandle, (void *)dObj->pixelBuffer,
                                dObj->pixelBufferSize, &clientObj->spiDrvTransferHandle);
    if (clientObj->spiDrvTransferHandle == DRV_SPI_TRANSFER_HANDLE_INVALID)
    {
        return false;
    }

    /* Mark the transfer status as in progress */
    dObj->isTransferInProgress = true;
    return true;
}

bool DRV_WS2812B_Clear(const DRV_HANDLE handle)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;
    uint32_t temp;

    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return false;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    /* Return if there is a pending transfer to prevent modifying pixelBuffer contents */
    if (dObj->isTransferInProgress)
    {
        return false;
    }

    /* Initialize the pixelBuffer */
    memset(dObj->pixelBuffer, 0, dObj->pixelBufferSize);

    /* Skip the first few bytes because we want these to be zeroes */
    for (int i = DRV_WS2812B_PAD_ZERO_AMT; i < dObj->pixelBufferSize; i += 3)
    {
        temp = DRV_WS2812B_Encode_To_SPI(0x0);

        dObj->pixelBuffer[i] = (temp >> 16) & 0xff;
        dObj->pixelBuffer[i + 1] = (temp >> 8) & 0xff;
        dObj->pixelBuffer[i + 2] = temp & 0xff;
    }

    return true;
}

bool DRV_WS2812B_TurnOff(const DRV_HANDLE handle)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;

    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return false;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    /* Return if there is a pending transfer */
    if (dObj->isTransferInProgress)
    {
        return false;
    }

    if(!DRV_WS2812B_Clear(handle))
    {
        return false;
    }

    if (!DRV_WS2812B_Refresh(handle))
    {
        return false;
    }

    return true;
}

bool DRV_WS2812B_SetColor(const DRV_HANDLE handle, uint16_t ledN, uint8_t red, uint8_t green, uint8_t blue)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;
    
    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return false;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    /* Return if there is a pending transfer to prevent modifying pixelBuffer contents */
    if (dObj->isTransferInProgress)
    {
        return false;
    }

    /* Return if ledN exceeds the LED array size */
    if(ledN >= dObj->ledArraySize)
    {
        return false;
    }

    // Call a local function
    _DRV_WS2812B_SetColor(dObj, ledN, red, green, blue);
    
    return true;
}

bool DRV_WS2812B_GetColor(const DRV_HANDLE handle, uint16_t ledN, uint32_t *color)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;
    
    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return false;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    /* Return if ledN exceeds the LED array size */
    if(ledN >= dObj->ledArraySize)
    {
        return false;
    }
    
    // Call a local function
    *color = _DRV_WS2812B_GetColor(dObj, ledN);
    
    return true;
}

uint16_t DRV_WS2812B_GetArraySize(const DRV_HANDLE handle)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;

    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return 0;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    return dObj->ledArraySize;
}

bool DRV_WS2812B_Fill(const DRV_HANDLE handle, uint16_t firstLED, uint16_t count, uint8_t red, uint8_t green, uint8_t blue)
{
    DRV_WS2812B_OBJ *dObj = NULL;
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;
    uint16_t endLED;

    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return false;
    }

    /* Get the corresponding driver instance */
    dObj = &gDrvWS2812BObj[clientObj->drvIndex];

    /* Return if there is a pending transfer to prevent modifying pixelBuffer contents */
    if (dObj->isTransferInProgress)
    {
        return false;
    }

    /* Return if firstLED exceeds the LED array size */
    if (firstLED >= dObj->ledArraySize)
    {
        return false;
    }

    /* Return if count is 0 */
    if(count == 0)
    {
        return false;
    }

    /* Ensure that the fill won't go past the last LED */
    endLED = firstLED + count;
    if(endLED >= dObj->ledArraySize)
    {
        endLED = dObj->ledArraySize;
    }

    /* Fill the buffer with the specified color */
    for(int i = firstLED; i < endLED; i++)
    {
        _DRV_WS2812B_SetColor(dObj, i, red, green, blue);
    }

    return true;
}

void DRV_WS2812B_EventHandlerSet(const DRV_HANDLE handle, const DRV_WS2812B_EVENT_HANDLER eventHandler, const uintptr_t context)
{
    DRV_WS2812B_CLIENT_OBJ *clientObj = NULL;

    /* Check if the driver handle is valid before proceeding */
    clientObj = DRV_WS2812B_ValidateDriverHandle(handle);
    if (clientObj == NULL)
    {
        return;
    }

    /* Make sure the client did not pass an empty address... */
    if((eventHandler != NULL))
    {
        clientObj->eventHandler = eventHandler;
        clientObj->context = context;
    }
    
    return;
}
