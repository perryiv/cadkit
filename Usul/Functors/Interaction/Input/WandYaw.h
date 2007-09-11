
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for wand-yaw input.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_WAND_YAW_INPUT_FUNCTOR_H_
#define _USUL_FUNCTOR_WAND_YAW_INPUT_FUNCTOR_H_

#include "Usul/Functors/Interaction/Input/WandAngle.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Input {


class USUL_EXPORT WandYaw : public WandAngle
{
public:

  // Useful typedef(s).
  typedef WandAngle BaseClass;
  typedef BaseClass::Unknown Unknown;

  // Type information.
  USUL_DECLARE_TYPE_ID ( WandYaw );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WandYaw );

  WandYaw ( Unknown *caller = 0x0, const std::string &name = "" );
  WandYaw ( const WandYaw &a );

  // Update the analog input.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

protected:

  virtual ~WandYaw();
};


} // namespace Input
} // namespace Interaction
} // namespace Functors
} // namespace CV


#endif // _USUL_FUNCTOR_WAND_YAW_INPUT_FUNCTOR_H_
