
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wand position matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_WAND_POSITION_MATRIX_FUNCTORS_H_
#define _USUL_FUNCTOR_WAND_POSITION_MATRIX_FUNCTORS_H_

#include "Usul/Functors/Interaction/Wand/WandMatrix.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Wand {


class USUL_EXPORT WandPosition : public Usul::Functors::Interaction::Wand::WandMatrix
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Wand::WandMatrix BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44d Matrix;

  // Type information.
  USUL_DECLARE_TYPE_ID ( WandPosition );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WandPosition );

  WandPosition ( Unknown *caller = 0x0, const std::string &name = "" );
  WandPosition ( const WandPosition &a );

  // Update the matrix.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

protected:

  virtual ~WandPosition();
};


} // namespace Wand
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_WAND_POSITION_MATRIX_FUNCTORS_H_
