
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for vertical joystick input.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_JOYSTICK_VERTICAL_INPUT_FUNCTOR_H_
#define _VRV_JOYSTICK_VERTICAL_INPUT_FUNCTOR_H_

#include "VRV/Export.h"
#include "VRV/Functors/Input/AnalogInput.h"

#include "Usul/Interfaces/IJoystick.h"


namespace VRV {
namespace Functors {


class VRV_EXPORT JoystickVertical : public AnalogInput
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( JoystickVertical );
  typedef AnalogInput BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Interfaces::IJoystickFloat Joystick;

  explicit JoystickVertical ( Unknown *unknown );
  JoystickVertical ( const JoystickVertical &a );

  // Update the analog input.
  virtual void          operator()();

protected:

  virtual ~JoystickVertical();

private:

  Joystick::ValidQueryPtr _j;
};


}; // namespace Functors
}; // namespace CV


#endif // _VRV_JOYSTICK_VERTICAL_INPUT_FUNCTOR_H_
