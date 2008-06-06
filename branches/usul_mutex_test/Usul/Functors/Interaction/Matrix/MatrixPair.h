
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pair of matrix functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_MATRIX_FUNCTOR_PAIR_H_
#define _USUL_FUNCTOR_MATRIX_FUNCTOR_PAIR_H_

#include "Usul/Functors/Interaction/Matrix/MatrixFunctor.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Matrix {


class USUL_EXPORT MatrixPair : public Usul::Functors::Interaction::Matrix::MatrixFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Matrix::MatrixFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  // Type information.
  USUL_DECLARE_TYPE_ID ( MatrixPair );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MatrixPair );

  MatrixPair ( 
    Unknown *caller = 0x0, 
    MatrixFunctor *c1 = 0x0, 
    MatrixFunctor *c2 = 0x0,
    const std::string &name= "" );
  MatrixPair ( const MatrixPair &tp );

  // Set the matrices.
  void                  left  ( MatrixFunctor * );
  void                  right ( MatrixFunctor * );

  // Update the matrix.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

protected:

  virtual ~MatrixPair();

  MatrixFunctor::RefPtr _c1;
  MatrixFunctor::RefPtr _c2;
};

} // namespace Matrix
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_MATRIX_FUNCTOR_PAIR_H_
