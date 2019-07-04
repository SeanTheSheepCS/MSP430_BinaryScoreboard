/**********************************************************************
* Supporting functions for LED Binary Counter
**********************************************************************/

/************************ Revision History ****************************
YYYY-MM-DD  Comments
-------------------------------------------------------------------------------------------
2013-09-14  First release.
2019-06-05  Binary counter scoreboard first attempt
2019-06-24  Pin assignments changed
2019-07-03  Documentation done

************************************************************************/

#include "io430.h"
#include "typedef_MSP430.h"
#include "intrinsics.h"
#include "binary_counter-efwd-01.h"
#include "main.h"
#include "leds.h"
#include "rgb_leds.h"
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

LedInformation LG_aLedInfoScoreLeds[LEDS_FOR_SCORE] = {{(u16*)0x0019, P3_2_LED6},
                                                       {(u16*)0x0019, P3_1_LED5},
                                                       {(u16*)0x0019, P3_0_LED4},
                                                       {(u16*)0x0029, P2_2_LED3},
                                                       {(u16*)0x0029, P2_1_LED2},
                                                       {(u16*)0x0029, P2_0_LED1}};
//This is so that the campers will have a simpler name to use
#define scoreLeds LG_aLedInfoScoreLeds

LedInformation LG_aLedInfoLifeLeds[LEDS_FOR_LIVES] = {{(u16*)0x0029, P2_4_LED7},
                                                      {(u16*)0x0029, P2_3_LED8},
                                                      {(u16*)0x0019, P3_7_LED9}};
//This is so that the campers will have a simpler name to use
#define lifeLeds LG_aLedInfoLifeLeds

RgbLedInformation LG_aRgbLedInfoRgbLeds[NUMBER_OF_RGB_LEDS] = {{(u16*)0x0021, P1_2_RGB_RED, RGB_LED_ACTIVE_TYPE_LOW,
                                                                (u16*)0x0021, P1_1_RGB_GRN, RGB_LED_ACTIVE_TYPE_LOW,
                                                                (u16*)0x0021, P1_0_RGB_BLU, RGB_LED_ACTIVE_TYPE_LOW}};
//This is so that the campers will have a simpler name to use
#define DUAL_RGB_LEDS LG_aRgbLedInfoRgbLeds[0]

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

bool  LG_bHasScoredSinceLastLifeLoss = FALSE;

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

/*----------------------------------------------------------------------------------------------------------------------
Function: gameOver

Description: switches the state of the counter to the game over state

Requires:
  - N/A

Promises:
  - switches the state of the counter to the game over state

*/
void gameOver()
{
  G_fCounterStateMachine = CounterSM_GameOver;
}

/*----------------------------------------------------------------------------------------------------------------------
Function: turnAllScoreLedsOn

Description: turns all the score LEDS on

Requires:
  - N/A

Promises:
  - turns every LED in LG_aLedInfoScoreLeds on

*/
void turnAllScoreLedsOn()
{
  for(int i = 0; i < LEDS_FOR_SCORE; i++)
  {
    LedOn(LG_aLedInfoScoreLeds[i]);
  }
}

  

/****************************************************************************************
State Machine Functions
****************************************************************************************/
/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_Initialize

Description: Sets up the pins to be used, turns off all LEDS except the life LEDS which are turned on

Requires:
  - N/A

Promises:
  - All I/O pins are selected as the I/O function
  - All output pins and input pins are set as such

*/
void CounterSM_Initialize()
{
  /* Reset key variables */
  u16GlobalCurrentSleepInterval = TIME_MAX;
  
  P1DIR |= P1_0_RGB_BLU; //Sets the pin as an output
  P1SEL2 &= ~P1_0_RGB_BLU; // This and the next line select the I/O function.
  P1SEL &= ~P1_0_RGB_BLU; // This and the previous line select the I/O function.
  P1DIR |= P1_1_RGB_GRN;
  P1SEL2 &= ~P1_1_RGB_GRN; 
  P1SEL &= ~P1_1_RGB_GRN; 
  P1DIR |= P1_2_RGB_RED;
  P1SEL2 &= ~P1_2_RGB_RED; 
  P1SEL &= ~P1_2_RGB_RED; 
  P1DIR &= ~P1_3_BUTTON_0; //Sets the pin as an input
  P1SEL2 &= ~P1_3_BUTTON_0;
  P1SEL &= ~P1_3_BUTTON_0;
  
  P2DIR |= P2_0_LED1;
  P2SEL2 &= ~P2_0_LED1;
  P2SEL &= ~P2_0_LED1;
  P2DIR |= P2_1_LED2;
  P2SEL2 &= ~P2_1_LED2;
  P2SEL &= ~P2_1_LED2;
  P2DIR |= P2_2_LED3;
  P2SEL2 &= ~P2_2_LED3;
  P2SEL &= ~P2_2_LED3;
  P2DIR |= P2_3_LED8;
  P2SEL2 &= ~P2_3_LED8;
  P2SEL &= ~P2_3_LED8;
  P2DIR |= P2_4_LED7;
  P2SEL2 &= ~P2_4_LED7;
  P2SEL &= ~P2_4_LED7;
  P2DIR &= ~P2_5_SPARE;
  P2SEL2 &= ~P2_5_SPARE;
  P2SEL &= ~P2_5_SPARE;
  P2DIR &= ~P2_6_SCORE;
  P2SEL2 &= ~P2_6_SCORE;
  P2SEL &= ~P2_6_SCORE;
  P2DIR &= ~P2_7_LOSELIFE;
  P2SEL2 &= ~P2_7_LOSELIFE;
  P2SEL &= ~P2_7_LOSELIFE;
  
  //P3SEL2 is not set here since port 3 only has one select register
  P3DIR |= P3_0_LED4;
  P3SEL &= ~P3_0_LED4;
  P3DIR |= P3_1_LED5;
  P3SEL &= ~P3_1_LED5;
  P3DIR |= P3_2_LED6;
  P3SEL &= ~P3_2_LED6;
  P3DIR &= ~P3_3_BUTTON_1;
  P3SEL &= ~P3_3_BUTTON_1;
  P3DIR |= P3_6_BUZZER;
  P3SEL &= ~P3_6_BUZZER;
  P3DIR |= P3_7_LED9;
  P3SEL &= ~P3_7_LED9;
  
  /* Allows interrupts on the pin */
  P2IFG &= ~P2_6_SCORE;
  P2IE |= P2_6_SCORE;
  P2IFG &= ~P2_7_LOSELIFE;
  P2IE |= P2_7_LOSELIFE;
  
  // Set the initial state of the leds
  turnAllLifeLedsOn();
  turnAllScoreLedsOn(); //So that the campers know when their turn all scoreLedsOff() function has worked.
  turnAllScoreLedsOff();
  RgbLedOffBlue(DUAL_RGB_LEDS);
  RgbLedOffRed(DUAL_RGB_LEDS);
  RgbLedOffGreen(DUAL_RGB_LEDS);
  
  G_fCounterStateMachine = CounterSM_Idle;
  
} /* end CounterSM_Initialize */

/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_GameOver

Description: the state that the counter should go to if the player loses a life when they had none left

Requires:
  - N/A

Promises:
  - the DUAL_RGB_LEDS are turned red until the reset button is pressed

*/
void CounterSM_GameOver()
{
  RgbLedOnRed(DUAL_RGB_LEDS);
  if(IsButtonPressed(RESET_BUTTON))
  {
    RgbLedOffRed(DUAL_RGB_LEDS);
    G_fCounterStateMachine = CounterSM_ResetButtonPressed;
    /* Debounce the button for 10 ms */
    /* 120 / 12,000 = 10 ms */
    for(u16 i = 0; i < 120; i++);
    turnAllScoreLedsOff();
    turnAllLifeLedsOn();
  }
    
} /* end CounterSM_GameOver() */


/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_ScorePostTouched

Description: The state the counter should enter once the score post has been touched

Requires:
  - N/A

Promises:
  - DUAL_RGB_LEDS are turned blue until the ball stops touching the post
  - When the ball stops touching the post, the score is incremented by one

*/void CounterSM_ScorePostTouched()
{
  RgbLedOnBlue(DUAL_RGB_LEDS);
  if(!IsInputPinOnVoltageLow(SCORE_PIN))
  {
    RgbLedOffBlue(DUAL_RGB_LEDS);
    G_fCounterStateMachine = CounterSM_Idle;
    LG_bHasScoredSinceLastLifeLoss = TRUE;
    incrementScoreByOne();
  }
} /* end CounterSM_ScorePostTouched() */

/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_LoseLifePostTouched

Description: The state the counter should enter once the lose life post has been touched

Requires:
  - N/A

Promises:
  - If you have lost a life since you last touched the lose life post, once the ball stops touching the post, you lose a life.

*/
void CounterSM_LoseLifePostTouched()
{
  if(!LG_bHasScoredSinceLastLifeLoss)
  {
    G_fCounterStateMachine = CounterSM_Idle;
  }
  else if(!IsInputPinOnVoltageLow(LOSE_LIFE_PIN))
  {
    G_fCounterStateMachine = CounterSM_Idle;
    LG_bHasScoredSinceLastLifeLoss = FALSE;
    decrementLivesByOne();
  }
} /* end CounterSM_LoseLifePostTouched() */

 
/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_Idle

Description: watches to see if the reset or spare button has been pressed and swaps the state accordingly

Requires:
  - N/A

Promises:
  - watches to see if the reset or spare button has been pressed and swaps the state accordingly

*/void CounterSM_Idle()
{
  if(IsButtonPressed(RESET_BUTTON))
  {
    G_fCounterStateMachine = CounterSM_ResetButtonPressed;
  }
  else if(IsButtonPressed(SPARE_BUTTON))
  {
    G_fCounterStateMachine = CounterSM_SpareButtonPressed;
  }
} /* end CounterSM_Idle() */

/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_Sleep

Description: UNUSED

Requires:
  - UNUSED

Promises:
  - UNUSED

*/void CounterSM_Sleep()
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

/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_ResetButtonPressed

Description: the state that the counter should enter once the reset button has been pressed

Requires:
  - N/A

Promises:
  - turns the score LEDs off once the button is no longer pressed down
  - turns the life LEDs on once the button is no longer pressed down

*/
void CounterSM_ResetButtonPressed()
{
  if(!IsButtonPressed(RESET_BUTTON))
  {
    G_fCounterStateMachine = CounterSM_Idle;
    turnAllScoreLedsOff();
    turnAllLifeLedsOn();
  }
}

/*----------------------------------------------------------------------------------------------------------------------
State: CounterSM_SpareButtonPressed

Description: the state that the counter should enter once the spare button has been pressed

Requires:
  - N/A

Promises:
  - calls manageSpareButtonPress once the button is no longer pressed down

*/
void CounterSM_SpareButtonPressed()
{
  if(!IsButtonPressed(SPARE_BUTTON))
  {
    G_fCounterStateMachine = CounterSM_Idle;
    manageSpareButtonPress();
  }
}














/*-----------------------------------------------------------------------------

HELLO CAMPERS! You will need these functions.

-void LedOff(LedInformation LedInfo)                     This function turns off the LED you pass in as an argument
-void LedOn(LedInformation LedInfo)                      This function turns on the LED you pass in as an argument
-bool isLedOff(LedInformation LedInfo)                   This function returns true (1) if the LED you pass in as an argument is off
-bool isLedOn(LedInformation LedInfo)                    This function returns false (0) if the LED you pass in as an argument is on
-void gameOver()                                         This function should be called if the game has ended

They will also need to know about these two arrays:

-LedInformation scoreLeds[6]           This array contains the six LEDS used to keep track of score. The LED at index zero is the rightmost, the LED at index five is the leftmost.
-LedInformation sifeLeds[3]             This array contains the three LEDS used to keep track of lives. The LED at index zero is the leftmost, the LED at index two is the rightmost.

-----------------------------------------------------------------------------*/




void turnAllScoreLedsOff()
{
  
}

void turnAllLifeLedsOn()
{
  
}

void decrementLivesByOne()
{
  
}

void incrementScoreByOne()
{
  
}

void manageSpareButtonPress()
{
  //The campers can put anything they want here!
}