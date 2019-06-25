/**********************************************************************
* Definitions for input pin functions
**********************************************************************/

/************************ Revision History ****************************
YYYY-MM-DD  Comments
-------------------------------------------------------------------------------------------
2019-06-25  File created

************************************************************************/

#include "input_pins.h"
#include "io430f2122.h"
#include "typedef_MSP430.h"

bool IsInputPinOnVoltageLow(InputPinInformation ipiInputPinInfo)
{
  return ! (((*(ipiInputPinInfo.u16pPortAddress)) & ipiInputPinInfo.u8InputPinIdentifier) == ipiInputPinInfo.u8InputPinIdentifier);
}