
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Button IDs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_BUTTON_IDS_H_
#define _VIRTUAL_REALITY_VIEWER_BUTTON_IDS_H_


namespace VRV
{
  const unsigned long BUTTON0 ( 0x00000001 );
  const unsigned long BUTTON1 ( 0x00000002 );
  const unsigned long BUTTON2 ( 0x00000004 );
  const unsigned long BUTTON3 ( 0x00000008 );
  const unsigned long BUTTON4 ( 0x00000010 );
  const unsigned long BUTTON5 ( 0x00000020 );
  const unsigned long BUTTON6 ( 0x00000040 );
  const unsigned long BUTTON7 ( 0x00000080 );
  const unsigned long BUTTON8 ( 0x00000100 );
  const unsigned long BUTTON9 ( 0x00000200 );

  // For convenience.
  const unsigned long BUTTON_RED      = VRV::BUTTON1;
  const unsigned long BUTTON_YELLOW   = VRV::BUTTON0;
  const unsigned long BUTTON_GREEN    = VRV::BUTTON2;
  const unsigned long BUTTON_BLUE     = VRV::BUTTON3;
  const unsigned long BUTTON_JOYSTICK = VRV::BUTTON4;
  const unsigned long BUTTON_TRIGGER  = VRV::BUTTON5;
};


#endif // _VIRTUAL_REALITY_VIEWER_BUTTON_IDS_H_
