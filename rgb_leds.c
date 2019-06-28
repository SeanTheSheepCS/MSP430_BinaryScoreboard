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
  if(RgbLedInfo.acttypeRedPinActiveType == RGB_LED_ACTIVE_TYPE_HIGH)
  {
    *(RgbLedInfo.u16pRedPinPortAddress) |= RgbLedInfo.u8RedPinLedIdentifier; 
  }
  else if(RgbLedInfo.acttypeRedPinActiveType == RGB_LED_ACTIVE_TYPE_LOW)
  {
    *(RgbLedInfo.u16pRedPinPortAddress) &= ~RgbLedInfo.u8RedPinLedIdentifier;
  }
}

void RgbLedOffRed(RgbLedInformation RgbLedInfo)
{
  if(RgbLedInfo.acttypeRedPinActiveType == RGB_LED_ACTIVE_TYPE_HIGH)
  {
    *(RgbLedInfo.u16pRedPinPortAddress) &= ~RgbLedInfo.u8RedPinLedIdentifier;
  }
  else if(RgbLedInfo.acttypeRedPinActiveType == RGB_LED_ACTIVE_TYPE_LOW)
  {
    *(RgbLedInfo.u16pRedPinPortAddress) |= RgbLedInfo.u8RedPinLedIdentifier; 
  }
}

void RgbLedOnBlue(RgbLedInformation RgbLedInfo)
{
  if(RgbLedInfo.acttypeBluePinActiveType == RGB_LED_ACTIVE_TYPE_HIGH)
  {
    *(RgbLedInfo.u16pBluePinPortAddress) |= RgbLedInfo.u8BluePinLedIdentifier; 
  }
  else if(RgbLedInfo.acttypeBluePinActiveType == RGB_LED_ACTIVE_TYPE_LOW)
  {
    *(RgbLedInfo.u16pBluePinPortAddress) &= ~RgbLedInfo.u8BluePinLedIdentifier;
  }
}

void RgbLedOffBlue(RgbLedInformation RgbLedInfo)
{
  if(RgbLedInfo.acttypeBluePinActiveType == RGB_LED_ACTIVE_TYPE_HIGH)
  {
    *(RgbLedInfo.u16pBluePinPortAddress) &= ~RgbLedInfo.u8BluePinLedIdentifier;
  }
  else if(RgbLedInfo.acttypeBluePinActiveType == RGB_LED_ACTIVE_TYPE_LOW)
  {
    *(RgbLedInfo.u16pBluePinPortAddress) |= RgbLedInfo.u8BluePinLedIdentifier; 
  }
}

void RgbLedOnGreen(RgbLedInformation RgbLedInfo)
{
  if(RgbLedInfo.acttypeGreenPinActiveType == RGB_LED_ACTIVE_TYPE_HIGH)
  {
    *(RgbLedInfo.u16pGreenPinPortAddress) |= RgbLedInfo.u8GreenPinLedIdentifier; 
  }
  else if(RgbLedInfo.acttypeGreenPinActiveType == RGB_LED_ACTIVE_TYPE_LOW)
  {
    *(RgbLedInfo.u16pGreenPinPortAddress) &= ~RgbLedInfo.u8GreenPinLedIdentifier;
  }
}

void RgbLedOffGreen(RgbLedInformation RgbLedInfo)
{
  if(RgbLedInfo.acttypeGreenPinActiveType == RGB_LED_ACTIVE_TYPE_HIGH)
  {
    *(RgbLedInfo.u16pGreenPinPortAddress) &= ~RgbLedInfo.u8GreenPinLedIdentifier;
  }
  else if(RgbLedInfo.acttypeGreenPinActiveType == RGB_LED_ACTIVE_TYPE_LOW)
  {
    *(RgbLedInfo.u16pGreenPinPortAddress) |= RgbLedInfo.u8GreenPinLedIdentifier; 
  }
}