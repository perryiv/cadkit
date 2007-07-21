
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for horizontal joystick input.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_JOYSTICK_HORIZONTAL_INPUT_FUNCTOR_H_
#define _VRV_JOYSTICK_HORIZONTAL_INPUT_FUNCTOR_H_

#include "VRV/Export.h"
#include "VRV/Functors/Input/AnalogInput.h"
#include "VRV/Interfaces/IJoystick.h"


namespace VRV {
namespace Functors {


class VRV_EXPORT JoystickHorizontal : public AnalogInput
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( JoystickHorizontal );
  typedef AnalogInput BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef VRV::Interfaces::IJoystickFloat Joystick;

  explicit JoystickHorizontal ( Unknown *unknown );
  JoystickHorizontal ( const JoystickHorizontal &a );

  // Update the analog input.
  virtual void          operator()();

protected:

  virtual ~JoystickHorizontal();

private:

  Joystick::ValidQueryPtr _j;
};


} // namespace Functors
} // namespace VRV


#endif // _VRV_JOYSTICK_HORIZONTAL_INPUT_FUNCTOR_H_
