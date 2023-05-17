/*******************************************************************************
  System Initialization File

  File Name:
    initialization.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources,
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "definitions.h"
#include "device.h"



// ****************************************************************************
// ****************************************************************************
// Section: Configuration Bits
// ****************************************************************************
// ****************************************************************************

/*** DEVCFG0 ***/
#pragma config DEBUG =      OFF
#pragma config JTAGEN =     OFF
#pragma config ICESEL =     ICS_PGx2
#pragma config TRCEN =      OFF
#pragma config BOOTISA =    MIPS32
#pragma config FECCCON =    OFF_UNLOCKED
#pragma config FSLEEP =     OFF
#pragma config DBGPER =     PG_ALL
#pragma config SMCLR =      MCLR_NORM
#pragma config SOSCGAIN =   GAIN_2X
#pragma config SOSCBOOST =  ON
#pragma config POSCGAIN =   GAIN_2X
#pragma config POSCBOOST =  ON
#pragma config POSCFGAIN =  GAIN_G3
#pragma config POSCAGCDLY = POSCAGCDLY1
#pragma config POSCAGC =      OFF
#pragma config EJTAGBEN =   NORMAL

/*** DEVCFG1 ***/
#pragma config FNOSC =      SPLL
#pragma config DMTINTV =    WIN_127_128
#pragma config FSOSCEN =    OFF
#pragma config IESO =       ON
#pragma config POSCMOD =    EC
#pragma config OSCIOFNC =   OFF
#pragma config FCKSM =      CSECME
#pragma config WDTPS =      PS1048576
#pragma config WDTSPGM =    STOP
#pragma config WINDIS =     NORMAL
#pragma config FWDTEN =     OFF
#pragma config FWDTWINSZ =  WINSZ_25
#pragma config DMTCNT =     DMT31
#pragma config FDMTEN =     OFF

/*** DEVCFG2 ***/
#pragma config FPLLIDIV =   DIV_1
#pragma config FPLLRNG =    RANGE_5_10_MHZ
#pragma config FPLLICLK =   PLL_FRC
#pragma config FPLLMULT =   MUL_50
#pragma config FPLLODIV =   DIV_2
#pragma config VBATBOREN =  ON
#pragma config DSBOREN =    ON
#pragma config DSWDTPS =    DSPS32
#pragma config DSWDTOSC =   LPRC
#pragma config DSWDTEN =    OFF
#pragma config FDSEN =      ON
#pragma config UPLLFSEL =   FREQ_24MHZ

/*** DEVCFG3 ***/
#pragma config USERID =     0xffff
#pragma config FMIIEN =     ON
#pragma config FETHIO =     ON
#pragma config PGL1WAY =    ON
#pragma config PMDL1WAY =   ON
#pragma config IOL1WAY =    ON

/*** DEVCFG4 ***/
#pragma config SWDTPS = SPS1048576

/*** BF1SEQ0 ***/
#pragma config TSEQ =       0x0
#pragma config CSEQ =       0xffff





// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="DRV_SPI Instance 0 Initialization Data">

/* SPI Client Objects Pool */
static DRV_SPI_CLIENT_OBJ drvSPI0ClientObjPool[DRV_SPI_CLIENTS_NUMBER_IDX0];

/* SPI Transfer Objects Pool */
static DRV_SPI_TRANSFER_OBJ drvSPI0TransferObjPool[DRV_SPI_QUEUE_SIZE_IDX0];

/* SPI PLIB Interface Initialization */
const DRV_SPI_PLIB_INTERFACE drvSPI0PlibAPI = {

    /* SPI PLIB Setup */
    .setup = (DRV_SPI_PLIB_SETUP)SPI1_TransferSetup,

    /* SPI PLIB WriteRead function */
    .writeRead = (DRV_SPI_PLIB_WRITE_READ)SPI1_WriteRead,

    /* SPI PLIB Transfer Status function */
    .isTransmitterBusy = (DRV_SPI_PLIB_TRANSMITTER_IS_BUSY)SPI1_IsTransmitterBusy,

    /* SPI PLIB Callback Register */
    .callbackRegister = (DRV_SPI_PLIB_CALLBACK_REGISTER)SPI1_CallbackRegister,
};

const uint32_t drvSPI0remapDataBits[]= { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000400, 0x00000800 };
const uint32_t drvSPI0remapClockPolarity[] = { 0x00000000, 0x00000040 };
const uint32_t drvSPI0remapClockPhase[] = { 0x00000000, 0x00000100 };

const DRV_SPI_INTERRUPT_SOURCES drvSPI0InterruptSources =
{
    /* Peripheral has more than one interrupt vectors */
    .isSingleIntSrc                        = false,

    /* Peripheral interrupt lines */
    .intSources.multi.spiTxReadyInt      = -1,
    .intSources.multi.spiTxCompleteInt   = _SPI1_TX_VECTOR,
    .intSources.multi.spiRxInt           = _SPI1_RX_VECTOR,
};

/* SPI Driver Initialization Data */
const DRV_SPI_INIT drvSPI0InitData =
{
    /* SPI PLIB API */
    .spiPlib = &drvSPI0PlibAPI,

    .remapDataBits = drvSPI0remapDataBits,

    .remapClockPolarity = drvSPI0remapClockPolarity,

    .remapClockPhase = drvSPI0remapClockPhase,

    /* SPI Number of clients */
    .numClients = DRV_SPI_CLIENTS_NUMBER_IDX0,

    /* SPI Client Objects Pool */
    .clientObjPool = (uintptr_t)&drvSPI0ClientObjPool[0],


    /* SPI Queue Size */
    .transferObjPoolSize = DRV_SPI_QUEUE_SIZE_IDX0,

    /* SPI Transfer Objects Pool */
    .transferObjPool = (uintptr_t)&drvSPI0TransferObjPool[0],

    /* SPI interrupt sources (SPI peripheral and DMA) */
    .interruptSources = &drvSPI0InterruptSources,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_SPI Instance 1 Initialization Data">

/* SPI Client Objects Pool */
static DRV_SPI_CLIENT_OBJ drvSPI1ClientObjPool[DRV_SPI_CLIENTS_NUMBER_IDX1];

/* SPI Transfer Objects Pool */
static DRV_SPI_TRANSFER_OBJ drvSPI1TransferObjPool[DRV_SPI_QUEUE_SIZE_IDX1];

/* SPI PLIB Interface Initialization */
const DRV_SPI_PLIB_INTERFACE drvSPI1PlibAPI = {

    /* SPI PLIB Setup */
    .setup = (DRV_SPI_PLIB_SETUP)SPI3_TransferSetup,

    /* SPI PLIB WriteRead function */
    .writeRead = (DRV_SPI_PLIB_WRITE_READ)SPI3_WriteRead,

    /* SPI PLIB Transfer Status function */
    .isTransmitterBusy = (DRV_SPI_PLIB_TRANSMITTER_IS_BUSY)SPI3_IsTransmitterBusy,

    /* SPI PLIB Callback Register */
    .callbackRegister = (DRV_SPI_PLIB_CALLBACK_REGISTER)SPI3_CallbackRegister,
};

const uint32_t drvSPI1remapDataBits[]= { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000400, 0x00000800 };
const uint32_t drvSPI1remapClockPolarity[] = { 0x00000000, 0x00000040 };
const uint32_t drvSPI1remapClockPhase[] = { 0x00000000, 0x00000100 };

const DRV_SPI_INTERRUPT_SOURCES drvSPI1InterruptSources =
{
    /* Peripheral has more than one interrupt vectors */
    .isSingleIntSrc                        = false,

    /* Peripheral interrupt lines */
    .intSources.multi.spiTxReadyInt      = -1,
    .intSources.multi.spiTxCompleteInt   = _SPI3_TX_VECTOR,
    .intSources.multi.spiRxInt           = _SPI3_RX_VECTOR,
};

/* SPI Driver Initialization Data */
const DRV_SPI_INIT drvSPI1InitData =
{
    /* SPI PLIB API */
    .spiPlib = &drvSPI1PlibAPI,

    .remapDataBits = drvSPI1remapDataBits,

    .remapClockPolarity = drvSPI1remapClockPolarity,

    .remapClockPhase = drvSPI1remapClockPhase,

    /* SPI Number of clients */
    .numClients = DRV_SPI_CLIENTS_NUMBER_IDX1,

    /* SPI Client Objects Pool */
    .clientObjPool = (uintptr_t)&drvSPI1ClientObjPool[0],


    /* SPI Queue Size */
    .transferObjPoolSize = DRV_SPI_QUEUE_SIZE_IDX1,

    /* SPI Transfer Objects Pool */
    .transferObjPool = (uintptr_t)&drvSPI1TransferObjPool[0],

    /* SPI interrupt sources (SPI peripheral and DMA) */
    .interruptSources = &drvSPI1InterruptSources,
};

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="DRV_SPI Instance 2 Initialization Data">

/* SPI Client Objects Pool */
static DRV_SPI_CLIENT_OBJ drvSPI2ClientObjPool[DRV_SPI_CLIENTS_NUMBER_IDX2];

/* SPI Transfer Objects Pool */
static DRV_SPI_TRANSFER_OBJ drvSPI2TransferObjPool[DRV_SPI_QUEUE_SIZE_IDX2];

/* SPI PLIB Interface Initialization */
const DRV_SPI_PLIB_INTERFACE drvSPI2PlibAPI = {

    /* SPI PLIB Setup */
    .setup = (DRV_SPI_PLIB_SETUP)SPI2_TransferSetup,

    /* SPI PLIB WriteRead function */
    .writeRead = (DRV_SPI_PLIB_WRITE_READ)SPI2_WriteRead,

    /* SPI PLIB Transfer Status function */
    .isTransmitterBusy = (DRV_SPI_PLIB_TRANSMITTER_IS_BUSY)SPI2_IsTransmitterBusy,

    /* SPI PLIB Callback Register */
    .callbackRegister = (DRV_SPI_PLIB_CALLBACK_REGISTER)SPI2_CallbackRegister,
};

const uint32_t drvSPI2remapDataBits[]= { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000400, 0x00000800 };
const uint32_t drvSPI2remapClockPolarity[] = { 0x00000000, 0x00000040 };
const uint32_t drvSPI2remapClockPhase[] = { 0x00000000, 0x00000100 };

const DRV_SPI_INTERRUPT_SOURCES drvSPI2InterruptSources =
{
    /* Peripheral has more than one interrupt vectors */
    .isSingleIntSrc                        = false,

    /* Peripheral interrupt lines */
    .intSources.multi.spiTxReadyInt      = -1,
    .intSources.multi.spiTxCompleteInt   = _SPI2_TX_VECTOR,
    .intSources.multi.spiRxInt           = _SPI2_RX_VECTOR,
};

/* SPI Driver Initialization Data */
const DRV_SPI_INIT drvSPI2InitData =
{
    /* SPI PLIB API */
    .spiPlib = &drvSPI2PlibAPI,

    .remapDataBits = drvSPI2remapDataBits,

    .remapClockPolarity = drvSPI2remapClockPolarity,

    .remapClockPhase = drvSPI2remapClockPhase,

    /* SPI Number of clients */
    .numClients = DRV_SPI_CLIENTS_NUMBER_IDX2,

    /* SPI Client Objects Pool */
    .clientObjPool = (uintptr_t)&drvSPI2ClientObjPool[0],


    /* SPI Queue Size */
    .transferObjPoolSize = DRV_SPI_QUEUE_SIZE_IDX2,

    /* SPI Transfer Objects Pool */
    .transferObjPool = (uintptr_t)&drvSPI2TransferObjPool[0],

    /* SPI interrupt sources (SPI peripheral and DMA) */
    .interruptSources = &drvSPI2InterruptSources,
};

// </editor-fold>

/* DRV_WS2812B Initialization Data */
// Declaring pixelBuffer arrays for each driver instance
DRV_WS2812B_PIXEL_DATA gDrvWS2812B0PixelBuffer[DRV_WS2812B_BUFFER_SIZE_IDX0];
DRV_WS2812B_PIXEL_DATA gDrvWS2812B1PixelBuffer[DRV_WS2812B_BUFFER_SIZE_IDX1];
DRV_WS2812B_PIXEL_DATA gDrvWS2812B2PixelBuffer[DRV_WS2812B_BUFFER_SIZE_IDX2];

// Client object pools
DRV_WS2812B_CLIENT_OBJ gDrvWS2812B0ClientObjPool[DRV_WS2812B_CLIENTS_NUMBER_IDX0];
DRV_WS2812B_CLIENT_OBJ gDrvWS2812B1ClientObjPool[DRV_WS2812B_CLIENTS_NUMBER_IDX1];
DRV_WS2812B_CLIENT_OBJ gDrvWS2812B2ClientObjPool[DRV_WS2812B_CLIENTS_NUMBER_IDX2];

// Initialization data
const DRV_WS2812B_INIT gDrvWS2812BInitObj[DRV_WS2812B_INSTANCES_NUMBER] =
{
    {
        .clientObj = (uintptr_t)&gDrvWS2812B0ClientObjPool,
        .ledArraySize = DRV_WS2812B_LEDSTRIPSIZE_IDX0,
        .maxClients = DRV_WS2812B_CLIENTS_NUMBER_IDX0,
        .pixelBuffer = gDrvWS2812B0PixelBuffer,
        .pixelBufferSize = DRV_WS2812B_BUFFER_SIZE_IDX0,
        .spiDrvIndex = DRV_SPI_INDEX_0,
    },
    {
        .clientObj = (uintptr_t)&gDrvWS2812B1ClientObjPool,
        .ledArraySize = DRV_WS2812B_LEDSTRIPSIZE_IDX1,
        .maxClients = DRV_WS2812B_CLIENTS_NUMBER_IDX1,
        .pixelBuffer = gDrvWS2812B1PixelBuffer,
        .pixelBufferSize = DRV_WS2812B_BUFFER_SIZE_IDX1,
        .spiDrvIndex = DRV_SPI_INDEX_1,
    },
    {
        .clientObj = (uintptr_t)&gDrvWS2812B2ClientObjPool,
        .ledArraySize = DRV_WS2812B_LEDSTRIPSIZE_IDX2,
        .maxClients = DRV_WS2812B_CLIENTS_NUMBER_IDX2,
        .pixelBuffer = gDrvWS2812B2PixelBuffer,
        .pixelBufferSize = DRV_WS2812B_BUFFER_SIZE_IDX2,
        .spiDrvIndex = DRV_SPI_INDEX_2,
    },
};

// *****************************************************************************
// *****************************************************************************
// Section: System Data
// *****************************************************************************
// *****************************************************************************
/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;

// *****************************************************************************
// *****************************************************************************
// Section: Library/Stack Initialization Data
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="SYS_TIME Initialization Data">

const SYS_TIME_PLIB_INTERFACE sysTimePlibAPI = {
    .timerCallbackSet = (SYS_TIME_PLIB_CALLBACK_REGISTER)CORETIMER_CallbackSet,
    .timerStart = (SYS_TIME_PLIB_START)CORETIMER_Start,
    .timerStop = (SYS_TIME_PLIB_STOP)CORETIMER_Stop ,
    .timerFrequencyGet = (SYS_TIME_PLIB_FREQUENCY_GET)CORETIMER_FrequencyGet,
    .timerPeriodSet = (SYS_TIME_PLIB_PERIOD_SET)NULL,
    .timerCompareSet = (SYS_TIME_PLIB_COMPARE_SET)CORETIMER_CompareSet,
    .timerCounterGet = (SYS_TIME_PLIB_COUNTER_GET)CORETIMER_CounterGet,
};

const SYS_TIME_INIT sysTimeInitData =
{
    .timePlib = &sysTimePlibAPI,
    .hwTimerIntNum = 0,
};

// </editor-fold>



// *****************************************************************************
// *****************************************************************************
// Section: Local initialization functions
// *****************************************************************************
// *****************************************************************************



/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
 */

void SYS_Initialize ( void* data )
{
    /* MISRAC 2012 deviation block start */
    /* MISRA C-2012 Rule 2.2 deviated in this file.  Deviation record ID -  H3_MISRAC_2012_R_2_2_DR_1 */

    /* Start out with interrupts disabled before configuring any modules */
    __builtin_disable_interrupts();

  
    CLK_Initialize();
    /* Configure Prefetch, Wait States and ECC */
    PRECONbits.PREFEN = 3;
    PRECONbits.PFMWS = 2;
    CFGCONbits.ECCCON = 3;



	GPIO_Initialize();

	SPI3_Initialize();

    CORETIMER_Initialize();
	BSP_Initialize();
	SPI2_Initialize();

	SPI1_Initialize();


    /* Initialize SPI0 Driver Instance */
    sysObj.drvSPI0 = DRV_SPI_Initialize(DRV_SPI_INDEX_0, (SYS_MODULE_INIT *)&drvSPI0InitData);
    /* Initialize SPI1 Driver Instance */
    sysObj.drvSPI1 = DRV_SPI_Initialize(DRV_SPI_INDEX_1, (SYS_MODULE_INIT *)&drvSPI1InitData);
    /* Initialize SPI2 Driver Instance */
    sysObj.drvSPI2 = DRV_SPI_Initialize(DRV_SPI_INDEX_2, (SYS_MODULE_INIT *)&drvSPI2InitData);

    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);
    
    /* Initialize WS2812B Driver Instances */
    DRV_WS2812B_Initialize(DRV_WS2812B_INDEX_0, (SYS_MODULE_INIT *)&gDrvWS2812BInitObj[DRV_WS2812B_INDEX_0]);
    DRV_WS2812B_Initialize(DRV_WS2812B_INDEX_1, (SYS_MODULE_INIT *)&gDrvWS2812BInitObj[DRV_WS2812B_INDEX_1]);
    DRV_WS2812B_Initialize(DRV_WS2812B_INDEX_2, (SYS_MODULE_INIT *)&gDrvWS2812BInitObj[DRV_WS2812B_INDEX_2]);

    APP_4BY4_ARRAY_Initialize();
    APP_8STRIP_ARRAY_Initialize();
    APP_8BY8_ARRAY_Initialize();


    EVIC_Initialize();

	/* Enable global interrupts */
    __builtin_enable_interrupts();


    /* MISRAC 2012 deviation block end */
}


/*******************************************************************************
 End of File
*/
