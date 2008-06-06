
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for wand-pitch input.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_WAND_ROLL_INPUT_FUNCTOR_H_
#define _USUL_FUNCTOR_WAND_ROLL_INPUT_FUNCTOR_H_

#include "Usul/Functors/Interaction/Input/WandAngle.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Input {


class USUL_EXPORT WandRoll : public WandAngle
{
public:

  // Useful typedef(s).
  typedef WandAngle BaseClass;
  typedef BaseClass::Unknown Unknown;

  // Type information.
  USUL_DECLARE_TYPE_ID ( WandRoll );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WandRoll );

  WandRoll ( Unknown *caller = 0x0, const std::string &name = "" );
  WandRoll ( const WandRoll &a );

  // Update the analog input.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

protected:

  virtual ~WandRoll();
};


} // namespace Input
} // namespace Interaction
} // namespace Functors
} // namespace CV


#endif // _USUL_FUNCTOR_WAND_ROLL_INPUT_FUNCTOR_H_
