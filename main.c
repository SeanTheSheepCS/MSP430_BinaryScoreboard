/**********************************************************************
File name:	main.c

Main program file for binary counter 

**********************************************************************/




/************************ Revision History ****************************
YYYY-MM-DD  Checksum  Comments
-------------------------------------------------------------------------------------------
2009-04-08            First release. 
2019-06-27            Updated for use with the binary counter

************************************************************************/

#include "io430.h"
#include "typedef_MSP430.h"
#include "intrinsics.h"
#include "main.h"
#include "binary_counter-efwd-01.h"


/************************ External Program Globals ****************************/
/* Globally available variables from other files as indicated */
extern fnCode_type G_fCounterStateMachine;            /* From binary_counter-efwd-01.c */

/************************ Program Globals ****************************/
/* Global variable definitions intended for scope of multiple files */


/************************ Main Program ****************************/
/* From cstartup.s43, the processor is running from the ACLK, TimerA is running, and I/O lines have been configured. */

int main(void)
{

  /* Enter the state machine where the program will remain unless power cycled */
  __bis_SR_register(GIE);

  while(1)
  {
	  G_fCounterStateMachine();
  } 
  
} /* end main */


/************************ Interrupt Service Routines ****************************/
#pragma vector = PORT2_VECTOR
__interrupt void Port2ISR(void)
/* Handles waking up from low power mode via a button press and returns with processor awake */
{
  
  /* If pin is still grounded, consider it valid */
  if( !(P2IN & P2_7_LOSELIFE) )
  {
    /* Debounce the input for 1 ms, this is done to make sure the signal is stable before we start checking if it is low in the next state. */
    /* 12 / 12,000 = 1 ms */
    for(u16 i = 0; i < 12; i++);
    G_fCounterStateMachine = CounterSM_LoseLifePostTouched;
  }
  if( (!(P2IN & P2_6_SCORE) && (G_fCounterStateMachine != CounterSM_GameOver)) )
  {
    /* Debounce the input for 1 ms, this is done to make sure the signal is stable before we start checking if it is low in the next state. */
    /* 12 / 12,000 = 1 ms */
    for(u16 i = 0; i < 12; i++);
    G_fCounterStateMachine = CounterSM_ScorePostTouched;
  }
 
  /* Clear the flag, but keep the interrupt active */
  P2IFG &= ~P2_7_LOSELIFE;
  P2IFG &= ~P2_6_SCORE;
  
  asm("BIC #0x0010,0(SP)"); 
} /* end Port2ISR */

