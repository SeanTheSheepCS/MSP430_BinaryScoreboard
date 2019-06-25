/**********************************************************************
* Definitions for button functions
**********************************************************************/

/************************ Revision History ****************************
YYYY-MM-DD  Comments
-------------------------------------------------------------------------------------------
2019-06-25  File created

************************************************************************/

#include "buttons.h"
#include "io430f2122.h"
#include "typedef_MSP430.h"

bool IsButtonPressed(ButtonInformation biButtonInfo)
{
  return !(((*(biButtonInfo.u16pPortAddress)) & biButtonInfo.u8ButtonIdentifier) == biButtonInfo.u8ButtonIdentifier);
}