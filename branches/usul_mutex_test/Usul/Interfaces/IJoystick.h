
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting input from a joystick.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_JOYSTICK_INPUT_H_
#define _USUL_INTERFACE_JOYSTICK_INPUT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IJoystick : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IJoystick );

  /// Id for this interface.
  enum { IID = iid };

  // Get the joystick value in the range [-1,1].
  virtual RealType          joystickHorizontal() const = 0;
  virtual RealType          joystickVertical()   const = 0;
};


typedef IJoystick<1076051636,float>  IJoystickFloat;
typedef IJoystick<1076051637,double> IJoystickDouble;


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_JOYSTICK_INPUT_H_
