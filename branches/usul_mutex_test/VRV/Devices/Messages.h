
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Enumerations that represent messages.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_DEVICES_MESSAGES_
#define _VRV_DEVICES_MESSAGES_


namespace VRV
{
  namespace Devices 
  {
    enum Message
    {
      JOYSTICK_AT_CENTER, 
      JOYSTICK_AT_RIGHT, 
      JOYSTICK_AT_LEFT, 
      JOYSTICK_AT_UP, 
      JOYSTICK_AT_DOWN,
      JOYSTICK_ENTERING_CENTER,
      JOYSTICK_ENTERING_RIGHT,
      JOYSTICK_ENTERING_LEFT,
      JOYSTICK_ENTERING_UP,
      JOYSTICK_ENTERING_DOWN,
      JOYSTICK_LEAVING_CENTER,
      JOYSTICK_LEAVING_RIGHT,
      JOYSTICK_LEAVING_LEFT,
      JOYSTICK_LEAVING_UP,
      JOYSTICK_LEAVING_DOWN,
    };
  }
}


#endif // _VRV_DEVICES_MESSAGES_
