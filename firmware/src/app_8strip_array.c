/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_8strip_array.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_8strip_array.h"
#include "driver/ws2812b/drv_ws2812b.h"
#include "peripheral/coretimer/plib_coretimer.h"
#include "system/time/sys_time.h"
#include "bsp/bsp.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define GREEN(x)    ((x >> 16) & 0xff)
#define RED(x)      ((x >> 8) & 0xff)
#define BLUE(x)     ((x & 0xff))

#define RGB_TO_UINT32(r,g,b) ( ((g<<16)) | ((r<<8)) | (b) )

#define LED_FRAMERATE_MS 75

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_8STRIP_ARRAY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_8STRIP_ARRAY_DATA app_8strip_arrayData;

/* Global Variables */
volatile bool drv1c1TransferStatus;
DRV_HANDLE drv1c1DrvHandle;
SYS_MODULE_INDEX drv1c1DrvIndex;
uint16_t drv1c1LEDStripSize;
SYS_TIME_HANDLE drv1c1TimerHandle;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/
void APP_8STRIP_EventHandler(DRV_WS2812B_EVENT event, uintptr_t context)
{
    if(event == DRV_WS2812B_WRITE_DONE)
    {
        drv1c1TransferStatus = true;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

// Return color, dimmed by 75% (used by scanner)
// Source: https://learn.adafruit.com/multi-tasking-the-arduino-part-3/utility-functions
uint32_t DimColor(uint32_t color)
{
    uint8_t g, r, b;
    g = GREEN(color);
    r = RED(color);
    b = BLUE(color);
    
    g = g >> 1;
    r = r >> 1;
    b = b >> 1;
    
    uint32_t dimColor = RGB_TO_UINT32(r, g, b);
    return dimColor;
}

uint32_t ColorShift(void)
{
    static uint8_t state = 0;
    uint32_t ret = 0;
    
    switch(state)
    {
        case 0: ret = RGB_TO_UINT32(0x1F, 0, 0); state = 1; break;
        case 1: ret = RGB_TO_UINT32(0x1F, 0x1F, 0); state = 2; break;
        case 2: ret = RGB_TO_UINT32(0, 0x1F, 0); state = 3; break;
        case 3: ret = RGB_TO_UINT32(0, 0x1F, 0x1F); state = 4; break;
        case 4: ret = RGB_TO_UINT32(0x1F, 0x1F, 0x1F); state = 5; break;
        case 5: ret = RGB_TO_UINT32(0x1F, 0, 0x1F); state = 0; break;
    }
    
    return ret;
}

void APP_8STRIP_Scanner(void)
{
    static uint16_t activeLED = 0;
    uint16_t totalSteps = (drv1c1LEDStripSize - 1) * 2;
    uint32_t color, dim;
    static uint8_t direction = 0;
    
    static uint32_t scanColor = RGB_TO_UINT32(0, 0x1F, 0);
    
    for(int i = 0; i < drv1c1LEDStripSize; i++)
    {
        if(i == activeLED)
        {
            DRV_WS2812B_SetColor(drv1c1DrvHandle, i, RED(scanColor), GREEN(scanColor), BLUE(scanColor));
        }
        else if(i == totalSteps - activeLED)
        {
            DRV_WS2812B_SetColor(drv1c1DrvHandle, i, RED(scanColor), GREEN(scanColor), BLUE(scanColor));
        }
        else
        {
            DRV_WS2812B_GetColor(drv1c1DrvHandle, i, &color);
            dim = DimColor(color);
            DRV_WS2812B_SetColor(drv1c1DrvHandle, i, RED(dim), GREEN(dim), BLUE(dim));
        }
    }

    if(!DRV_WS2812B_Refresh(drv1c1DrvHandle))
    {
        return;
    }

    if(direction == 0)
    {
        activeLED++;
    }
    else
    {
        activeLED--;
    }
    
    if(activeLED >= drv1c1LEDStripSize)
    {
        direction++;
    }
    else if(activeLED == 0)
    {
        direction = 0;
        scanColor = ColorShift();
    }
    
    return;
}

void APP_8STRIP_ColorWipe(void)
{
    static uint16_t index = 0;
    static uint32_t scanColor = RGB_TO_UINT32(0x1F, 0x07, 0);
    
    DRV_WS2812B_SetColor(drv1c1DrvHandle, index++, RED(scanColor), GREEN(scanColor), BLUE(scanColor));
    if(!DRV_WS2812B_Refresh(drv1c1DrvHandle))
    {
        return;
    }
    
    if(index == drv1c1LEDStripSize)
    {
        scanColor = ColorShift();
        index = 0;
    }
    
    return;
}

void APP_8STRIP_DemoTask(void)
{
    if(SWITCH2_Get())
    {
        APP_8STRIP_Scanner();
    }
    else
    {
        APP_8STRIP_ColorWipe();
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_8STRIP_ARRAY_Initialize ( void )

  Remarks:
    See prototype in app_8strip_array.h.
 */

void APP_8STRIP_ARRAY_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_8strip_arrayData.state = APP_8STRIP_ARRAY_STATE_INIT;
    drv1c1TransferStatus = false;
    drv1c1DrvHandle = DRV_HANDLE_INVALID;
    drv1c1DrvIndex = DRV_WS2812B_INDEX_1;
    drv1c1TimerHandle = SYS_TIME_HANDLE_INVALID;
    
    return;
}


/******************************************************************************
  Function:
    void APP_8STRIP_ARRAY_Tasks ( void )

  Remarks:
    See prototype in app_8strip_array.h.
 */

void APP_8STRIP_ARRAY_Tasks ( void )
{    
    /* Check the application's current state. */
    switch ( app_8strip_arrayData.state )
    {
        /* Application's initial state. */
        case APP_8STRIP_ARRAY_STATE_INIT:
        {
            /* Open a WS2812B Driver Instance */
            drv1c1DrvHandle = DRV_WS2812B_Open(drv1c1DrvIndex, DRV_IO_INTENT_WRITE);
            if(drv1c1DrvHandle == DRV_HANDLE_INVALID)
            {
                app_8strip_arrayData.state = APP_8STRIP_ARRAY_STATE_ERROR;
                break;
            }
            
            /* Register an event handler */
            DRV_WS2812B_EventHandlerSet(drv1c1DrvHandle, APP_8STRIP_EventHandler, 0);
            
            /* Get LED array size as reported by the driver */
            drv1c1LEDStripSize = DRV_WS2812B_GetArraySize(drv1c1DrvHandle);
            
            /* Turn off LEDs */
            if(!DRV_WS2812B_TurnOff(drv1c1DrvHandle))
            {
                app_8strip_arrayData.state = APP_8STRIP_ARRAY_STATE_ERROR;
                break;
            }
            
            /* Wait for the LEDs to turn off*/
            app_8strip_arrayData.state = APP_8STRIP_ARRAY_STATE_WAIT;
            break;
        }
        
        case APP_8STRIP_ARRAY_STATE_WAIT:
        {
            if(drv1c1TransferStatus)
            {
                /* Transfer is done. Start a 100ms timer delay so user knows the LEDs turned off */
                drv1c1TransferStatus = false;
                SYS_TIME_DelayMS(100, &drv1c1TimerHandle);
            }
            
            if(SYS_TIME_DelayIsComplete(drv1c1TimerHandle))
            {
                /* Timer has been destroyed. We can create a new timer. */
                drv1c1TimerHandle = SYS_TIME_TimerCreate(0, SYS_TIME_MSToCount(LED_FRAMERATE_MS), NULL, 0, SYS_TIME_PERIODIC);
                SYS_TIME_TimerStart(drv1c1TimerHandle);
                app_8strip_arrayData.state = APP_8STRIP_ARRAY_STATE_ANIMATION;
            }
            break;
        }

        case APP_8STRIP_ARRAY_STATE_ANIMATION:
        {
            if(SYS_TIME_TimerPeriodHasExpired(drv1c1TimerHandle))
            {
                APP_8STRIP_DemoTask();
            }
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
