/**********************************************************************
* Supporting functions for LED Binary Counter
**********************************************************************/

/************************ Revision History ****************************
YYYY-MM-DD  Comments
-------------------------------------------------------------------------------------------
2013-09-14  First release.
2019-06-05  Binary counter scoreboard first attempt
2019-06-24  Pin assignments changed

************************************************************************/

#include "io430.h"
#include "typedef_MSP430.h"
#include "intrinsics.h"
#include "binary_counter-efwd-01.h"
#include "main.h"
#include "leds.h"
#include "buttons.h"
#include "input_pins.h"

/******************** External Globals ************************/
/* Globally available variables from other files as indicated */


/******************** Program Globals ************************/
/* Global variable definitions intended for scope across multiple files */
fnCode_type G_fCounterStateMachine = CounterSM_Initialize;   /* The application state machine */  

volatile u16 u16GlobalRuntimeFlags = 0;               /* Flag register for communicating various runtime events. */
volatile u16 u16GlobalErrorFlags = 0;                 /* Flag register for communicating errors. */

volatile u16 u16GlobalCurrentSleepInterval;           /* Duration that the device will sleep */

/******************** Local Globals ************************/
/* Global variable definitions intended only for the scope of this file */

LedInformation LG_aLedInfoScoreLeds[LEDS_FOR_SCORE] = {{(u16*)0x0029, P2_0_LED1},
                                                       {(u16*)0x0029, P2_1_LED2},
                                                       {(u16*)0x0029, P2_2_LED3},
                                                       {(u16*)0x0019, P3_0_LED4},
                                                       {(u16*)0x0019, P3_1_LED5},
                                                       {(u16*)0x0019, P3_2_LED6}};
//This is so that the campers will have a simpler name to use
#define scoreLeds LG_aLedInfoScoreLeds

LedInformation LG_aLedInfoLifeLeds[LEDS_FOR_LIVES] = {{(u16*)0x0029, P2_4_LED7},
                                                      {(u16*)0x0029, P2_3_LED8},
                                                      {(u16*)0x0019, P3_7_LED9}};
//This is so that the campers will have a simpler name to use
#define lifeLeds LG_aLedInfoLifeLeds

ButtonInformation LG_aButtonInfoButtons[NUMBER_OF_BUTTONS] = {{(u16*)0x0020, P1_3_BUTTON_0},
                                                              {(u16*)0x0018, P3_3_BUTTON_1}};

//This is so that the campers will have a simpler name to use
#define RESET_BUTTON LG_aButtonInfoButtons[0]
#define SPARE_BUTTON LG_aButtonInfoButtons[1]

InputPinInformation LG_aInputPinInfoInputPins[NUMBER_OF_INPUT_PINS] = {{(u16*)0x0028, P2_5_SPARE},
                                                                       {(u16*)0x0028, P2_6_SCORE},
                                                                       {(u16*)0x0028, P2_7_LOSELIFE}};

//This is so that the campers will have a simpler name to use
#define SPARE_PIN LG_aInputPinInfoInputPins[0]
#define SCORE_PIN LG_aInputPinInfoInputPins[1]
#define LOSE_LIFE_PIN LG_aInputPinInfoInputPins[2]

u8  LG_u8ActiveIndex  = 0;

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

void gameOver()
{
  G_fCounterStateMachine = CounterSM_GameOver;
}
  

/****************************************************************************************
State Machine Functions
****************************************************************************************/
void CounterSM_Initialize()
{
  /* Reset key variables */
  u16GlobalCurrentSleepInterval = TIME_MAX;
  
  P1DIR |= P1_0_RGB_BLU;
  P1DIR |= P1_1_RGB_GRN;
  P1DIR |= P1_2_RGB_RED;
  P1DIR &= ~P1_3_BUTTON_0;
  
  P2DIR |= P2_0_LED1;
  P2DIR |= P2_1_LED2;
  P2DIR |= P2_2_LED3;
  P2DIR |= P2_3_LED8;
  P2DIR |= P2_4_LED7;
  P2DIR &= ~P2_5_SPARE;
  P2DIR &= ~P2_6_SCORE;
  P2DIR &= ~P2_7_LOSELIFE;
  
  P3DIR |= P3_0_LED4;
  P3DIR |= P3_1_LED5;
  P3DIR |= P3_2_LED6;
  P3DIR &= ~P3_3_BUTTON_1;
  P3DIR |= P3_6_BUZZER;
  P3DIR |= P3_7_LED9;
       
  G_fCounterStateMachine = CounterSM_Idle;
  
} /* end CounterSM_Initialize */

/*----------------------------------------------------------------------------*/
void CounterSM_GameOver()
{
  turnAllScoreLedsOff();
  G_fCounterStateMachine = CounterSM_Idle;
    
} /* end CounterSM_GameOver() */


/*----------------------------------------------------------------------------*/
void CounterSM_ScorePostTouched()
{
  if(!IsInputPinOnVoltageLow(SCORE_PIN))
  {
    G_fCounterStateMachine = CounterSM_Idle;
  }
} /* end CounterSM_ScorePostTouched() */

/*----------------------------------------------------------------------------*/
void CounterSM_LoseLifePostTouched()
{
  if(!IsInputPinOnVoltageLow(LOSE_LIFE_PIN))
  {
    G_fCounterStateMachine = CounterSM_Idle;
  }
}
 
/*----------------------------------------------------------------------------*/
void CounterSM_Idle()
{
  if(IsButtonPressed(RESET_BUTTON))
  {
    /* Debounce the button for 10 ms */
    /* 120 / 12,000 = 10 ms */
    for(u16 i = 0; i < 120; i++);
    turnAllScoreLedsOff();
    turnAllLifeLedsOn();
    G_fCounterStateMachine = CounterSM_ResetButtonPressed;
  }
  
  if(IsButtonPressed(SPARE_BUTTON))
  {
    /* Debounce the button for 10 ms */
    /* 120 / 12,000 = 10 ms */
    for(u16 i = 0; i < 120; i++);
    manageSpareButtonPress();
    G_fCounterStateMachine = CounterSM_SpareButtonPressed;
  }
  
  if(IsInputPinOnVoltageLow(SCORE_PIN))
  {
    /* Debounce the button for 10 ms */
    /* 120 / 12,000 = 10 ms */
    for(u16 i = 0; i < 120; i++);
    incrementScoreByOne();
    G_fCounterStateMachine = CounterSM_ScorePostTouched;
  }
  
  if(IsInputPinOnVoltageLow(LOSE_LIFE_PIN))
  {
    /* Debounce the button for 10 ms */
    /* 120 / 12,000 = 10 ms */
    for(u16 i = 0; i < 120; i++);
    decrementLivesByOne();
    G_fCounterStateMachine = CounterSM_LoseLifePostTouched;
  }
} /* end CounterSM_Idle() */

/*----------------------------------------------------------------------------*/
void CounterSM_Sleep()
{
  /* Update to the current sleep interval and re-enable the timer interrupt */
  SetTimer(u16GlobalCurrentSleepInterval);
  TACTL = TIMERA_INT_ENABLE;
    
  /* Enter low power mode */
  __bis_SR_register(CPUOFF);
     
  /* Wake up (timer interrupt is off now from ISR) and go to next state */
  if(G_fCounterStateMachine == CounterSM_Sleep)
  {
    G_fCounterStateMachine = CounterSM_Idle;
  }
} /* end CounterSM_Sleep */

void CounterSM_ResetButtonPressed()
{
  if(!IsButtonPressed(RESET_BUTTON))
  {
    G_fCounterStateMachine = CounterSM_Idle;
  }
}

void CounterSM_SpareButtonPressed()
{
  manageSpareButtonPress();
  if(!IsButtonPressed(SPARE_BUTTON))
  {
    G_fCounterStateMachine = CounterSM_Idle;
  }
}














/*-----------------------------------------------------------------------------

HERE BEGINS THE CODE THAT THE CAMPERS WILL WRITE.
The campers will implement each of these functions and then bring it to the leader to be inspected and run on the board.
Solutions are provided here. The functions they will need to know about are as follows:

-void LedOff(LedInformation LedInfo)                     This function turns off the LED you pass in as an argument
-void LedOn(LedInformation LedInfo)                      This function turns on the LED you pass in as an argument
-bool isLedOff(LedInformation LedInfo)                   This function returns true (1) if the LED you pass in as an argument is off
-bool isLedOn(LedInformation LedInfo)                    This function returns false (0) if the LED you pass in as an argument is on
-void gameOver()                                         This function should be called if the game has ended

They will also need to know about these two arrays:

-LedInformation LG_u8ScoreLeds[LEDS_FOR_SCORE]           This array contains the six LEDS used to keep track of score. The LED at index zero is the rightmost, the LED at index five is the leftmost.
-LedInformation LG_u8LifeLeds[LEDS_FOR_LIVES]             This array contains the three LEDS used to keep track of lives. The LED at index zero is the leftmost, the LED at index two is the rightmost.

-----------------------------------------------------------------------------*/




void turnAllScoreLedsOff()
{
  for(int i = 0; i < LEDS_FOR_SCORE; i++)
  {
    LedOff(LG_aLedInfoScoreLeds[i]);
  }
}

void turnAllLifeLedsOn()
{
  for(int i = 0; i < LEDS_FOR_LIVES; i++)
  {
    LedOn(LG_aLedInfoLifeLeds[i]);
  }
}

void decrementLivesByOne()
{
  for(int i = 0; i < LEDS_FOR_LIVES; i++)
  {
    if(isLedOn(LG_aLedInfoLifeLeds[i]))
    {
      LedOff(LG_aLedInfoLifeLeds[i]);
      return;
    }
  }
  gameOver();
}

void incrementScoreByOne()
{
  for(int i = 0; i < LEDS_FOR_SCORE; i++)
  {
    if(isLedOff(LG_aLedInfoScoreLeds[i]))
    {
      LedOn(LG_aLedInfoScoreLeds[i]);
      return;
    }
    else
    {
      LedOff(LG_aLedInfoScoreLeds[i]);
    }
  }
}

void manageSpareButtonPress()
{
  //The campers can put anything they want here!
  turnAllLifeLedsOn();
}