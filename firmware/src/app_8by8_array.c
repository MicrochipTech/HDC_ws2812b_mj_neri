/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_8by8_array.c

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

#include "app_8by8_array.h"
#include "system/time/sys_time.h"
#include "driver/driver_common.h"
#include "driver/ws2812b/drv_ws2812b.h"
#include "bsp/bsp.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define LED_FRAMERATE_MS 100

#define GREEN(x)    ((x >> 16) & 0xff)
#define RED(x)      ((x >> 8) & 0xff)
#define BLUE(x)     ((x & 0xff))

#define RGB_TO_UINT32(r,g,b) ( ((g<<16)) | ((r<<8)) | (b) )

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_8BY8_ARRAY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_8BY8_ARRAY_DATA app_8by8_arrayData;

/* Global Variables */
volatile bool       drv2c1TransferStatus;
DRV_HANDLE          drv2c1DrvHandle;
SYS_MODULE_INDEX    drv2c1DrvIndex;
uint16_t            drv2c1LedStripSize;
SYS_TIME_HANDLE     drv2c1TimerHandle;

// Source: https://www.min.at/prinz/o/software/pixelfont/
const uint8_t font8x8[] = 
{
	0x7E, 0x7F, 0x09, 0x09, 0x09, 0x7F, 0x7E, 0x00,	// Char 065 (A)
	0x41, 0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00,	// Char 066 (B)
	0x3E, 0x7F, 0x41, 0x41, 0x41, 0x63, 0x22, 0x00,	// Char 067 (C)
	0x41, 0x7F, 0x7F, 0x41, 0x41, 0x7F, 0x3E, 0x00,	// Char 068 (D)
	0x41, 0x7F, 0x7F, 0x49, 0x5D, 0x41, 0x63, 0x00,	// Char 069 (E)
	0x41, 0x7F, 0x7F, 0x49, 0x1D, 0x01, 0x03, 0x00,	// Char 070 (F)
	0x3E, 0x7F, 0x41, 0x51, 0x51, 0x77, 0x36, 0x00,	// Char 071 (G)
	0x7F, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x7F, 0x00,	// Char 072 (H)
	0x00, 0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00,	// Char 073 (I)
	0x20, 0x60, 0x40, 0x41, 0x7F, 0x3F, 0x01, 0x00,	// Char 074 (J)
	0x7F, 0x7F, 0x08, 0x1C, 0x36, 0x63, 0x41, 0x00,	// Char 075 (K)
	0x41, 0x7F, 0x7F, 0x41, 0x40, 0x40, 0x60, 0x00,	// Char 076 (L)
	0x7F, 0x7F, 0x06, 0x0C, 0x06, 0x7F, 0x7F, 0x00,	// Char 077 (M)
	0x7F, 0x7F, 0x06, 0x0C, 0x18, 0x7F, 0x7F, 0x00,	// Char 078 (N)
	0x3E, 0x7F, 0x41, 0x41, 0x41, 0x7F, 0x3E, 0x00,	// Char 079 (O)
	0x41, 0x7F, 0x7F, 0x49, 0x09, 0x0F, 0x06, 0x00,	// Char 080 (P)
	0x3E, 0x7F, 0x41, 0x41, 0xC1, 0xFF, 0x3E, 0x00,	// Char 081 (Q)
	0x41, 0x7F, 0x7F, 0x09, 0x09, 0x7F, 0x76, 0x00,	// Char 082 (R)
	0x26, 0x6F, 0x49, 0x49, 0x49, 0x7B, 0x32, 0x00,	// Char 083 (S)
	0x00, 0x03, 0x41, 0x7F, 0x7F, 0x41, 0x03, 0x00,	// Char 084 (T)
	0x3F, 0x7F, 0x40, 0x40, 0x40, 0x7F, 0x3F, 0x00,	// Char 085 (U)
	0x1F, 0x3F, 0x60, 0x40, 0x60, 0x3F, 0x1F, 0x00,	// Char 086 (V)
	0x7F, 0x7F, 0x20, 0x10, 0x20, 0x7F, 0x7F, 0x00,	// Char 087 (W)
	0x41, 0x63, 0x3E, 0x1C, 0x3E, 0x63, 0x41, 0x00,	// Char 088 (X)
	0x00, 0x07, 0x4F, 0x78, 0x78, 0x4F, 0x07, 0x00,	// Char 089 (Y)
	0x43, 0x63, 0x71, 0x59, 0x4D, 0x67, 0x63, 0x00,	// Char 090 (Z)
};

const uint8_t stringMCHP[] = 
{
	0x7F, 0x7F, 0x06, 0x0C, 0x06, 0x7F, 0x7F, 0x00,	// Char 077 (M)
    0x00, 0x00,
	0x00, 0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00,	// Char 073 (I)
    0x00, 0x00,
	0x3E, 0x7F, 0x41, 0x41, 0x41, 0x63, 0x22, 0x00,	// Char 067 (C)
    0x00, 0x00,
	0x41, 0x7F, 0x7F, 0x09, 0x09, 0x7F, 0x76, 0x00,	// Char 082 (R)
    0x00, 0x00,
	0x3E, 0x7F, 0x41, 0x41, 0x41, 0x7F, 0x3E, 0x00,	// Char 079 (O)
    0x00, 0x00,
	0x3E, 0x7F, 0x41, 0x41, 0x41, 0x63, 0x22, 0x00,	// Char 067 (C)
    0x00, 0x00,
	0x7F, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x7F, 0x00,	// Char 072 (H)
    0x00, 0x00,
	0x00, 0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00,	// Char 073 (I)
    0x00, 0x00,
	0x41, 0x7F, 0x7F, 0x49, 0x09, 0x0F, 0x06, 0x00,	// Char 080 (P)
    0x00, 0x00,
	0x7F, 0x7F, 0x06, 0x0C, 0x06, 0x7F, 0x7F, 0x00,	// Char 077 (M) - added for continuity while scrolling the last letter
};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/
void APP_8BY8_EventHandler(DRV_WS2812B_EVENT event, uintptr_t context)
{
    if(event == DRV_WS2812B_WRITE_DONE)
    {
        drv2c1TransferStatus = true;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

uint32_t APP_8BY8_ColorCycle(void)
{   
    // Cycle between red, green, and yellow
    
	static unsigned Red = 0;
	static unsigned Green = 255;
	static unsigned Blue = 0;
	static int State = 0;
    
	switch (State)
	{
		case 0:
        {
            Red++;
			if (Red == 255) State = 1;
			break;
		}
		case 1:
        {
			Green--;
			if (Green == 0) State = 2;
			break;
		}
		case 2:
        {
			Green++;
			if (Green == 255) State = 3;			
			break;
		}
        case 3:
        {
			Red--;
			if (Red == 0) State = 0;			
			break;
		}
	}
    
    uint8_t g, r, b;
    
    // Reduce brightness
    g = Green >> 4;
    r = Red >> 4;
    b = Blue >> 4;
    
#ifdef __DEBUG
    // Issue: while the text scrolls, sometimes some LEDs flash with a blue/violet-like color,
    // even though I don't modify the blue channel.
    static int i = 0; // Counter - checks how many times this function's been called.
    static uint32_t storevals[1500];
    
    storevals[i++] = (g << 16) + (r << 8) + b;
    
    if(i > 1500 || (b > 0))
        __builtin_software_breakpoint();
#endif
	return (g << 16) + (r << 8) + b;
}


/* The 8by8 matrix's LEDs are arranged in a zigzag pattern. This function 
 helps map a more matrix-like addressing scheme to the zigzag pattern */
uint16_t APP_8BY8_MapMatrixToArray(uint16_t matrixAdd)
{
    uint16_t zigzagAdd;
    
    if((matrixAdd & 0x8))
    {
        zigzagAdd = matrixAdd ^ 0x7;
    }
    else
    {
        zigzagAdd = matrixAdd;
    }
    
    return zigzagAdd;
}

void APP_8BY8_DemoTask(void)
{
    // Hack-y way to display "MICROCHIP" as scrolling text
    
    uint8_t temp;
    uint16_t zigzagAdd;
    
    static uint8_t scrollOffset = 0;
    
    uint32_t color = APP_8BY8_ColorCycle();
    
    // Column loop:
    for(int i = 0; i < 8; i++)
    {
        temp = stringMCHP[i+scrollOffset];
        
        // Row loop:
        for(int j = 0; j < 8; j++)
        {
            zigzagAdd = APP_8BY8_MapMatrixToArray(8*i + j);
            
            if(temp & 0x01)
            {
                DRV_WS2812B_SetColor(drv2c1DrvHandle, zigzagAdd, RED(color), GREEN(color), BLUE(color));
            }
            else
            {
                DRV_WS2812B_SetColor(drv2c1DrvHandle, zigzagAdd, 0,0,0);
            }
            
            temp = temp >> 1;
        }
    }
    
    DRV_WS2812B_Refresh(drv2c1DrvHandle);
    scrollOffset = (scrollOffset + 1) % (sizeof(stringMCHP)-8);
    
    return;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_8BY8_ARRAY_Initialize ( void )

  Remarks:
    See prototype in app_8by8_array.h.
 */

void APP_8BY8_ARRAY_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_8by8_arrayData.state = APP_8BY8_ARRAY_STATE_INIT;

    drv2c1TransferStatus = false;
    drv2c1DrvHandle = DRV_HANDLE_INVALID;
    drv2c1DrvIndex = DRV_WS2812B_INDEX_2;
    drv2c1TimerHandle = SYS_TIME_HANDLE_INVALID;
}


/******************************************************************************
  Function:
    void APP_8BY8_ARRAY_Tasks ( void )

  Remarks:
    See prototype in app_8by8_array.h.
 */

void APP_8BY8_ARRAY_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( app_8by8_arrayData.state )
    {
        /* Application's initial state. */
        case APP_8BY8_ARRAY_STATE_INIT:
        {
            drv2c1DrvHandle = DRV_WS2812B_Open(drv2c1DrvIndex, DRV_IO_INTENT_WRITE);
            if(drv2c1DrvHandle == DRV_HANDLE_INVALID)
            {
                app_8by8_arrayData.state = APP_8BY8_ARRAY_STATE_ERROR;
                break;
            }
            
            DRV_WS2812B_EventHandlerSet(drv2c1DrvHandle, APP_8BY8_EventHandler, 0);
            
            drv2c1LedStripSize = DRV_WS2812B_GetArraySize(drv2c1DrvHandle);
            
            /* Turn off LEDs */
            if(!DRV_WS2812B_TurnOff(drv2c1DrvHandle))
            {
                app_8by8_arrayData.state = APP_8BY8_ARRAY_STATE_ERROR;
                break;
            }
            
            app_8by8_arrayData.state = APP_8BY8_ARRAY_STATE_CLEAR_WAIT;
            break;
        }
        
        case APP_8BY8_ARRAY_STATE_CLEAR_WAIT:
        {
            if(drv2c1TransferStatus)
            {
                drv2c1TransferStatus = false;
                SYS_TIME_DelayMS(10, &drv2c1TimerHandle);
            }
            
            if(SYS_TIME_DelayIsComplete(drv2c1TimerHandle))
            {
                // Create & start a new timer
                drv2c1TimerHandle = SYS_TIME_TimerCreate(0, SYS_TIME_MSToCount(LED_FRAMERATE_MS), NULL, 0, SYS_TIME_PERIODIC);
                SYS_TIME_TimerStart(drv2c1TimerHandle);
                app_8by8_arrayData.state = APP_8BY8_ARRAY_STATE_SERVICE_TASKS;
            }
            
            break;
        }

        case APP_8BY8_ARRAY_STATE_SERVICE_TASKS:
        {
            /* Freeze the animation if SW3 is held. */
            if(SWITCH3_Get() == SWITCH4_STATE_RELEASED)
            {
                if(SYS_TIME_TimerPeriodHasExpired(drv2c1TimerHandle))
                {
                    APP_8BY8_DemoTask();
                }
            }
            break;
        }
        
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
