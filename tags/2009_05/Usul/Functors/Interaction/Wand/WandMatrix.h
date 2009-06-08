
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wand matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_WAND_MATRIX_FUNCTOR_H_
#define _USUL_FUNCTOR_WAND_MATRIX_FUNCTOR_H_

#include "Usul/Functors/Interaction/Matrix/MatrixFunctor.h"
#include "Usul/Interfaces/IWandState.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Wand {


class USUL_EXPORT WandMatrix : public Usul::Functors::Interaction::Matrix::MatrixFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Matrix::MatrixFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Interfaces::IWandStateDouble WandState;
  typedef Usul::Math::Matrix44d Matrix;

  // Type information.
  USUL_DECLARE_TYPE_ID ( WandMatrix );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WandMatrix );

  WandMatrix ( Unknown *caller = 0x0, const std::string &name = "" );
  WandMatrix ( const WandMatrix &a );

  // Update the matrix.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

  // Set the caller.
  virtual void          caller ( Unknown* caller );

protected:

  virtual ~WandMatrix();

  const WandState *   _wandState() const;
  WandState *         _wandState();

private:

  WandState::QueryPtr _ws;
};


} // namespace Wand
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_WAND_MATRIX_FUNCTOR_H_
