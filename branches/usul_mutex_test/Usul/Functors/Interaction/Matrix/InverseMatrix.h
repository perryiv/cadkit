
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Inverse matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_INVERSE_MATRIX_FUNCTOR_H_
#define _USUL_FUNCTOR_INVERSE_MATRIX_FUNCTOR_H_

#include "Usul/Functors/Interaction/Matrix/MatrixFunctor.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Matrix {


class USUL_EXPORT InverseMatrix : public Usul::Functors::Interaction::Matrix::MatrixFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Matrix::MatrixFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  // Type information.
  USUL_DECLARE_TYPE_ID ( InverseMatrix );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( InverseMatrix );

  InverseMatrix ( Unknown *caller = 0x0, const std::string &name = "", BaseClass *mf = 0x0 );
  InverseMatrix ( const InverseMatrix &a );

  // Set the matrix that we watch.
  void                  internalMatrix ( BaseClass * );

  // Update the matrix.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

protected:

  virtual ~InverseMatrix();

  BaseClass::RefPtr _mf;
};


} // namespace Matrix
} // namespace Interaction
} // namespace Functors
} // namespace CV


#endif // _USUL_FUNCTOR_INVERSE_MATRIX_FUNCTOR_H_
