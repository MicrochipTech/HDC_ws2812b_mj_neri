/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_4by4_array.c

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

#include "app_4by4_array.h"
#include "driver/ws2812b/drv_ws2812b_definitions.h"
#include "bsp/bsp.h"
#include "peripheral/coretimer/plib_coretimer.h"
#include "driver/ws2812b/drv_ws2812b.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define UINT32_TO_GRN(x)    ((x >> 16) & 0xff)
#define UINT32_TO_RED(x)    ((x >> 8) & 0xff)
#define UINT32_TO_BLU(x)    ((x & 0xff))

#define RGB_TO_UINT32(r,g,b) ( ((g<<16)) | ((r<<8)) | (b) )

#define LED_UPDATE_RATE_MS  64
#define ONE_SHOT_TIMER_MS   500

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
    This structure should be initialized by the APP_4BY4_ARRAY_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

static APP_4BY4_ARRAY_DATA app4by4Data;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

void APP_4BY4_EventHandler(DRV_WS2812B_EVENT event, uintptr_t context)
{
    if(event == DRV_WS2812B_WRITE_DONE)
    {
        app4by4Data.transferStatus = true;
        
        // Toggle to indicate there is activity
        RGB_LED_R_Toggle();
    }
}

void APP_4BY4_OneShot(uintptr_t context)
{
    app4by4Data.oneShotExpired = true;
}

void APP_4BY4_Periodic(uintptr_t context)
{
    EXT1_PIN7_Toggle();     // For troubleshooting purposes, to check sequence of function calls
    app4by4Data.periodicExpired = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/
// Source: https://github.com/fduignan/NucleoF042_SingleWS2812B
uint32_t getRainbow(void)
{   
    // Cycle through the colours of the rainbow (non-uniform brightness however)
	// Inspired by : http://academe.co.uk/2012/04/arduino-cycling-through-colours-of-the-rainbow/
	static unsigned Red = 255;
	static unsigned Green = 0;
	static unsigned Blue = 0;
	static int State = 0;
	switch (State)
	{
		case 0:{
			Green++;
			if (Green == 255)
				State = 1;
			break;
		}
		case 1:{
			Red--;
			if (Red == 0)
				State = 2;
			break;
		}
		case 2:{
			Blue++;
			if (Blue == 255)
				State = 3;			
			break;
		}
		case 3:{
			Green--;
			if (Green == 0)
				State = 4;
			break;
		}
		case 4:{
			Red++;
			if (Red == 255)
				State = 5;
			break;
		}
		case 5:{
			Blue --;
			if (Blue == 0)
				State = 0;
			break;
		}		
	}
    
    uint8_t g, r, b;
    
    // Reduce brightness
    g = Green >> 4;
    r = Red >> 4;
    b = Blue >> 4;
	return (g << 16) + (r << 8) + b;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_4BY4_ARRAY_Initialize ( void )

  Remarks:
    See prototype in app_4by4_array.h.
 */

void APP_4BY4_ARRAY_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app4by4Data.state = APP_4BY4_STATE_INIT;
    app4by4Data.transferStatus = false;
    app4by4Data.ws2812bDrvHandle = DRV_HANDLE_INVALID;
    app4by4Data.ws2812bDrvIndex = DRV_WS2812B_INDEX_0;
    app4by4Data.oneShotHandle = SYS_TIME_HANDLE_INVALID;
    app4by4Data.periodicHandle = SYS_TIME_HANDLE_INVALID;
    app4by4Data.oneShotExpired = false;
    app4by4Data.periodicExpired = false;
    
    // Create a client-controlled one-shot software timer
    app4by4Data.oneShotHandle = SYS_TIME_TimerCreate(0, SYS_TIME_MSToCount(ONE_SHOT_TIMER_MS), APP_4BY4_OneShot, 0, SYS_TIME_SINGLE);
    
    // Create a client-controlled periodic software timer. I do this instead of calling
    // SYS_TIME_CallbackRegisterMS so I could control when the timer starts.
    app4by4Data.periodicHandle = SYS_TIME_TimerCreate(0, SYS_TIME_MSToCount(LED_UPDATE_RATE_MS), APP_4BY4_Periodic, 0, SYS_TIME_PERIODIC);
    
    RGB_LED_B_On();
}


/******************************************************************************
  Function:
    void APP_4BY4_ARRAY_Tasks ( void )

  Remarks:
    See prototype in app_4by4_array.h.
 */

void APP_4BY4_ARRAY_Tasks ( void )
{
    uint32_t temp;
    uint8_t g, r, b;
    static uint8_t clearCounter = 0;
    
    /* Check the application's current state. */
    switch ( app4by4Data.state )
    {
        /* Application's initial state. */
        case APP_4BY4_STATE_INIT:
        {
            /* Open an instance of the WS2812B Driver */
            app4by4Data.ws2812bDrvHandle = DRV_WS2812B_Open(app4by4Data.ws2812bDrvIndex, DRV_IO_INTENT_WRITE);
            if(app4by4Data.ws2812bDrvHandle == DRV_HANDLE_INVALID)
            {
                app4by4Data.state = APP_4BY4_STATE_ERROR;
                break;
            }
            
            /* Register a callback */
            DRV_WS2812B_EventHandlerSet(app4by4Data.ws2812bDrvHandle, APP_4BY4_EventHandler, 0);
            
            /* Get strip size from the driver */
            app4by4Data.ledStripSize = DRV_WS2812B_GetArraySize(app4by4Data.ws2812bDrvHandle);
            
            app4by4Data.state = APP_4BY4_STATE_CLEAR_LEDS;
            break;
        }

        case APP_4BY4_STATE_CLEAR_LEDS:
        {
            /* Turn off LEDs */
            if(!DRV_WS2812B_TurnOff(app4by4Data.ws2812bDrvHandle))
            {
                app4by4Data.state = APP_4BY4_STATE_ERROR;
                break;
            }
            
            // Start the one-shot timer
            SYS_TIME_TimerStart(app4by4Data.oneShotHandle);
            app4by4Data.state = APP_4BY4_STATE_CLEAR_LEDS_WAIT;
            break;
        }
        
        case APP_4BY4_STATE_CLEAR_LEDS_WAIT:
        {
            /* Here, we call DRV_WS2812B_TurnOff twice as a workaround. 
            In some of the LED arrays I tested, the First LED doesn't 
            always turn off. Calling this function twice at least ensures 
            the LEDs turn off. */            
            if(clearCounter < 1)
            {
                if(app4by4Data.transferStatus)
                {
                    app4by4Data.transferStatus = false;
                    app4by4Data.state = APP_4BY4_STATE_CLEAR_LEDS;
                    clearCounter++;
                    
                    // Stop the one-shot timer
                    SYS_TIME_TimerStop(app4by4Data.oneShotHandle);
                }
            }
            else
            {
                if(app4by4Data.transferStatus)
                {
                    app4by4Data.transferStatus = false;
                }
                
                if(app4by4Data.oneShotExpired)
                {
                    app4by4Data.oneShotExpired = false;
                    
                    // Destroy the timer
                    SYS_TIME_TimerDestroy(app4by4Data.oneShotHandle);
                    
                    // Start the periodic timer before moving to the next state
                    SYS_TIME_TimerStart(app4by4Data.periodicHandle);
                    app4by4Data.state = APP_4BY4_STATE_RAINBOW;
                }
            }
            
            break;
        }
        
        case APP_4BY4_STATE_RAINBOW:
        {
            for(int i = 0; i < app4by4Data.ledStripSize; i++)
            {
                temp = getRainbow();
                g = (temp >> 16) & 0xff;
                r = (temp >> 8) & 0xff;
                b = temp & 0xff;
                
                if(!DRV_WS2812B_SetColor(app4by4Data.ws2812bDrvHandle, i, r, g, b))
                {
                    app4by4Data.state = APP_4BY4_STATE_ERROR;
                    break;
                }
            }
            
            EXT1_PIN5_Toggle();     // For troubleshooting purposes, to check sequence of function calls
            if(!DRV_WS2812B_Refresh(app4by4Data.ws2812bDrvHandle))
            {
                app4by4Data.state = APP_4BY4_STATE_ERROR;
                break;
            }
            
            app4by4Data.state = APP_4BY4_STATE_RAINBOW_WAIT;
            break;
        }
        
        case APP_4BY4_STATE_RAINBOW_WAIT:
        {
            if(app4by4Data.transferStatus)
            {
                app4by4Data.transferStatus = false;
            }
            
            if(app4by4Data.periodicExpired)
            {
                EXT1_PIN3_Toggle();     // For troubleshooting purposes, to check sequence of function calls
                app4by4Data.periodicExpired = false;
                app4by4Data.state = APP_4BY4_STATE_RAINBOW;
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
