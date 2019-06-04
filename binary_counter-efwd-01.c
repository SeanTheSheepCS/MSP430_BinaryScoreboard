/**********************************************************************
* Supporting functions for LED Binary Counter
**********************************************************************/

/************************ Revision History ****************************
YYYY-MM-DD  Comments
-------------------------------------------------------------------------------------------
2013-09-14  First release.

************************************************************************/

#include "io430.h"
#include "typedef_MSP430.h"
#include "intrinsics.h"
#include "binary_counter-efwd-01.h"
#include "main.h"
#include "leds.h"

/******************** External Globals ************************/
/* Globally available variables from other files as indicated */


/******************** Program Globals ************************/
/* Global variable definitions intended for scope across multiple files */
fnCode_type CounterStateMachine = CounterSM_Initialize;   /* The application state machine */
fnCode_type G_fCurrentStateMachine = CounterSM_Idle;  

volatile u16 u16GlobalRuntimeFlags = 0;               /* Flag register for communicating various runtime events. */
volatile u16 u16GlobalErrorFlags = 0;                 /* Flag register for communicating errors. */

volatile u16 u16GlobalCurrentSleepInterval;           /* Duration that the device will sleep */

/******************** Local Globals ************************/
/* Global variable definitions intended only for the scope of this file */

u8 LG_u8ScoreLedIdentifiers[LEDS_FOR_SCORE]   = {P2_4_LED1,    P1_0_LED2,    P1_1_LED3,    P1_2_LED4,    P1_3_LED5,    P3_2_LED6};
u16*  LG_pu16ScoreLedPorts[LEDS_FOR_SCORE]    = {(u16*)0x0029, (u16*)0x0021, (u16*)0x0021, (u16*)0x0021, (u16*)0x0021, (u16*)0x0019};
LedInformation LG_u8ScoreLeds[LEDS_FOR_SCORE] = {{(u16*)0x0029, P2_4_LED1},
                                                 {(u16*)0x0021, P1_0_LED2},
                                                 {(u16*)0x0021, P1_1_LED3},
                                                 {(u16*)0x0021, P1_2_LED4},
                                                 {(u16*)0x0021, P1_3_LED5},
                                                 {(u16*)0x0019, P3_2_LED6}};

u8 LG_u8LifeLedIdentifiers[LEDS_FOR_LIVES]   = {P3_1_LED7,    P3_0_LED8,    P2_2_LED9};
u16*  LG_pu16LifeLedPorts[LEDS_FOR_LIVES]    = {(u16*)0x0019, (u16*)0x0019, (u16*)0x0029};
LedInformation LG_u8LifeLeds[LEDS_FOR_LIVES] = {{(u16*)0x0019, P3_1_LED7},
                                                {(u16*)0x0019, P3_0_LED8},
                                                {(u16*)0x0029, P2_2_LED9}};

u8  LG_u8ActiveIndex  = 0;


/******************** Function Definitions ************************/
/*------------------------------------------------------------------------------
Function: TestBlink

Description:
Non-returning brute force function to blink LED
Speed depends on clock and delay cycles in the loop.
 
Requires:
  - Active-high LED on Port 1 with the label defined as LED2 

Promises:
  - Port 1 LED pin toggled at x Hz, thus an LED will blink at x/2 Hz.
*/
void TestBlink()
{
/* A quick test function to blink the LEDs with brute force at x Hz */
	while(1)
  {
	  for(u16 i = 5000; i != 0; i--); 	/* 6 cycle loop */
    P1OUT ^= P1_0_LED2; 	/* Takes 5 instruction cycles */	
	} 
} /* end TestBlink */


/*------------------------------------------------------------------------------
Function: SetTimer

Description:
Sets the TACCRO register, clears timer and clear the interrupt flag.
 
Requires:
  - usTaccr0_ is the value to which TACCRO will be set, where the time before interrupt in seconds
    is usTaccr0_ * (8/32768).  e.g. TACCRO = 12288 is a 3000ms delay.

Promises:
  - Timer is reset to 0
  - The timer interrupt flag is cleared
  - TACCRO is loaded with usTaccr0_
*/
void SetTimer(u16 usTaccr0_)
/* Sets the TACCRO register, clears timer and the interrupt flag */
{
  TAR = 0;
	TACCR0 = usTaccr0_;	
	TACTL &= ~TAIFG; 
  
} /* end SetTimer */
  

/****************************************************************************************
State Machine Functions
****************************************************************************************/
void CounterSM_Initialize()
{
  /* Reset key variables */
  u16GlobalCurrentSleepInterval = TIME_MAX;
    
  /* Allow a button interrupt and timer to wake up sleep */
  P2IFG &= ~P2_6_BUTTON_1;
  P2IE |= P2_6_BUTTON_1;	
  P2IFG &= ~P2_7_BUTTON_0;
  P2IE |= P2_7_BUTTON_0;	
  TACTL = TIMERA_INT_ENABLE;
       
  CounterStateMachine = CounterSM_Sleep;
  
} /* end CounterSM_Initialize */

/*----------------------------------------------------------------------------*/
void CounterSM_GameOver()
{
  for(u8 i = 0; i < TOTAL_LEDS; i++)
  {
    *LG_pu16LedPorts[i] |= LG_u8Leds[i];
  }
  
  /* Sleep for max time (or could disable sleep timer interrupt */
  u16GlobalCurrentSleepInterval = TIME_MAX;
  CounterStateMachine = CounterSM_Sleep;

    
} /* end CounterSM_GameOver() */


/*----------------------------------------------------------------------------*/
void CounterSM_Score()
{
  for(u8 i = 0; i < TOTAL_LEDS; i++)
  {
    *LG_pu16LedPorts[i] &= ~LG_u8Leds[i];
  }
  
  /* Sleep for max time (or could disable sleep timer interrupt */
  u16GlobalCurrentSleepInterval = TIME_MAX;
  CounterStateMachine = CounterSM_Sleep;

    
} /* end CounterSM_Score() */

 
/*----------------------------------------------------------------------------*/
void CounterSM_Idle()
{
  static bool bCurrentlyOn = FALSE;
  
  /* LEDs are on, so turn them off and sleep long */
  if(bCurrentlyOn)
  {
    for(u8 i = 0; i < TOTAL_LEDS; i++)
    {
      *LG_pu16LedPorts[i] &= ~LG_u8Leds[i];
    }
    bCurrentlyOn = FALSE;
    u16GlobalCurrentSleepInterval = TIME_3S;
  }
  /* LEDS are off, so turn them on and sleep short */
  else
  {
    for(u8 i = 0; i < TOTAL_LEDS; i++)
    {
      *LG_pu16LedPorts[i] |= LG_u8Leds[i];
    }
    bCurrentlyOn = TRUE;
    u16GlobalCurrentSleepInterval = TIME_125MS;
  }

  CounterStateMachine = CounterSM_Sleep;
  
} /* end CounterSM_Idle() */


#if 0
/*----------------------------------------------------------------------------*/
void BlinkSM_ButtonCheck()
{
#if 0
  /* Check for button press / hold */
  SetTimer(TIME_3S);
  while( !(P1IN & P1_0_BUTTON) && !(TACTL & TAIFG) )
  {
    P2OUT |= P2_2_LED4;
  } 

  if( !(P1IN & P1_0_BUTTON) && (TACTL & TAIFG) )
  {
    BlinkStateMachine = BlinkSM_Clockwise;
  } 
  else
  {
    P2OUT &= ~P2_2_LED4;
    BlinkStateMachine = BlinkSM_Sleep;
  }
#endif
  
} /* end BlinkSM_ButtonCheck */
#endif

/*----------------------------------------------------------------------------*/
void CounterSM_Sleep()
{
  /* Update to the current sleep interval and re-enable the timer interrupt */
  SetTimer(u16GlobalCurrentSleepInterval);
  TACTL = TIMERA_INT_ENABLE;
    
  /* Enter low power mode */
  __bis_SR_register(CPUOFF);
     
  /* Wake up (timer interrupt is off now from ISR) and go to next state */
  CounterStateMachine = G_fCurrentStateMachine;

} /* end CounterSM_Sleep */

void CounterSM_ResetButtonPressed()
{
  turnAllScoreLedsOff();
  turnAllLifeLedsOn();
}














/*-----------------------------------------------------------------------------

HERE BEGINS THE CODE THAT THE CAMPERS WILL WRITE.
The campers will implement each of these functions and then bring it to the leader to be inspected and run on the board.
Solutions are provided here.

-----------------------------------------------------------------------------*/




void turnAllScoreLedsOff()
{
  for(int i = 0; i < LEDS_FOR_SCORE; i++)
  {
    LedOff(LG_u8ScoreLeds[i]);
  }
}

void turnAllLifeLedsOn()
{
  for(int i = 0; i < LEDS_FOR_LIVES; i++)
  {
    LedOn(LG_u8LifeLeds[i]);
  }
}

void decrementLivesByOne()
{
  for(int i = LEDS_FOR_LIVES-1; i >= 0; i--)
  {
    if(isLedOn(LG_u8LifeLeds[i]))
    {
      LedOff(LG_u8LifeLeds[i]);
      break;
    }
    else if(i == 0)
    {
      CounterStateMachine = CounterSM_GameOver;
    }
  }
}

void incrementScoreByOne()
{
  for(int i = 0; i < LEDS_FOR_SCORE; i++)
  {
    if(isLedOff(LG_u8ScoreLeds[i]))
    {
      LedOn(LG_u8ScoreLeds[i]);
      break;
    }
    else
    {
      LedOff(LG_u8ScoreLeds[i]);
    }
  }
}