/**********************************************************************
* Header file for RGB LED functions
**********************************************************************/

/************************ Revision History ****************************
YYYY-MM-DD  Comments
-------------------------------------------------------------------------------------------
2019-06-28  File created

************************************************************************/

#ifndef __RGB_LED_HEADER
#define __RGB_LED_HEADER

#include "typedef_MSP430.h"

/******************************************************************************
Type Definitions
******************************************************************************/

typedef enum {RGB_LED_ACTIVE_TYPE_HIGH = 0, RGB_LED_ACTIVE_TYPE_LOW = 1} RgbLedActiveType;

typedef struct
{
  u16* u16pRedPinPortAddress;
  u8 u8RedPinLedIdentifier;
  RgbLedActiveType acttypeRedPinActiveType;
  u16* u16pGreenPinPortAddress;
  u8 u8GreenPinLedIdentifier;
  RgbLedActiveType acttypeGreenPinActiveType;
  u16* u16pBluePinPortAddress;
  u8 u8BluePinLedIdentifier;
  RgbLedActiveType acttypeBluePinActiveType;
}RgbLedInformation;

/****************************************************************************************
Constants
****************************************************************************************/

/************************ Function Declarations ****************************/

void RgbLedOnRed(RgbLedInformation RgbLedInfo);
void RgbLedOffRed(RgbLedInformation RgbLedInfo);
void RgbLedOnBlue(RgbLedInformation RgbLedInfo);
void RgbLedOffBlue(RgbLedInformation RgbLedInfo);
void RgbLedOnGreen(RgbLedInformation RgbLedInfo);
void RgbLedOffGreen(RgbLedInformation RgbLedInfo);

#endif /* __RGB_LED_HEADER */