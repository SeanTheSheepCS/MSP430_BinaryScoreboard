/**********************************************************************
* Definitions for RGB LED functions
**********************************************************************/

/************************ Revision History ****************************
YYYY-MM-DD  Comments
-------------------------------------------------------------------------------------------
2019-06-28  File created

************************************************************************/

#include "rgb_leds.h"
#include "io430f2122.h"
#include "typedef_MSP430.h"

void RgbLedOnRed(RgbLedInformation RgbLedInfo)
{
  *(RgbLedInfo.u16pRedPinPortAddress) |= RgbLedInfo.u8RedPinLedIdentifier;
}

void RgbLedOffRed(RgbLedInformation RgbLedInfo)
{
  *(RgbLedInfo.u16pRedPinPortAddress) &= ~RgbLedInfo.u8RedPinLedIdentifier;
}

void RgbLedOnBlue(RgbLedInformation RgbLedInfo)
{
  *(RgbLedInfo.u16pBluePinPortAddress) |= RgbLedInfo.u8BluePinLedIdentifier;
}

void RgbLedOffBlue(RgbLedInformation RgbLedInfo)
{
  *(RgbLedInfo.u16pBluePinPortAddress) &= ~RgbLedInfo.u8BluePinLedIdentifier;
}

void RgbLedOnGreen(RgbLedInformation RgbLedInfo)
{
  *(RgbLedInfo.u16pGreenPinPortAddress) |= RgbLedInfo.u8GreenPinLedIdentifier;
}

void RgbLedOffGreen(RgbLedInformation RgbLedInfo)
{
  *(RgbLedInfo.u16pGreenPinPortAddress) &= ~RgbLedInfo.u8GreenPinLedIdentifier;
}