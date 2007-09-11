
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

#ifndef _USUL_FUNCTOR_JOYSTICK_HORIZONTAL_INPUT_FUNCTOR_H_
#define _USUL_FUNCTOR_JOYSTICK_HORIZONTAL_INPUT_FUNCTOR_H_

#include "Usul/Functors/Interaction/Input/AnalogInput.h"
#include "Usul/Interfaces/IJoystick.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Input {


class USUL_EXPORT JoystickHorizontal : public AnalogInput
{
public:

  // Useful typedef(s).
  typedef AnalogInput BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Interfaces::IJoystickFloat Joystick;

  // Type information.
  USUL_DECLARE_TYPE_ID ( JoystickHorizontal );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( JoystickHorizontal );

  JoystickHorizontal ( Unknown *caller = 0x0, const std::string &name = "" );
  JoystickHorizontal ( const JoystickHorizontal &a );

  // Update the analog input.
  virtual void          operator()();

  // Set the caller.
  virtual void          caller ( Unknown* caller );

  // Clone this functor.
  virtual BaseFunctor*  clone ();

protected:

  virtual ~JoystickHorizontal();

private:

  Joystick::QueryPtr _j;
};


} // namespace Input
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_JOYSTICK_HORIZONTAL_INPUT_FUNCTOR_H_
