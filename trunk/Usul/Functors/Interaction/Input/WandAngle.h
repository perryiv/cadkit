
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base functor class for wand-angle input.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_BASE_WAND_ANGLE_INPUT_FUNCTOR_H_
#define _USUL_FUNCTOR_BASE_WAND_ANGLE_INPUT_FUNCTOR_H_

#include "Usul/Functors/Interaction/Input/AnalogInput.h"
#include "Usul/Interfaces/IWandState.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Input {


class USUL_EXPORT WandAngle : public AnalogInput
{
public:

  // Useful typedef(s).
  typedef AnalogInput BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Interfaces::IWandStateFloat IWandState;

  // Type information.
  USUL_DECLARE_TYPE_ID ( WandAngle );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WandAngle );

  explicit WandAngle ( Unknown *caller, const std::string &name );
  WandAngle ( const WandAngle &a );

protected:

  virtual ~WandAngle();

  float                 _calculateValue ( unsigned int zero );

private:

  IWandState::QueryPtr _wand;
};


} // namespace Input
} // namespace Interaction
} // namespace Functors
} // namespace CV


#endif // _USUL_FUNCTOR_BASE_WAND_ANGLE_INPUT_FUNCTOR_H_
