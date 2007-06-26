
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

#ifndef _VRV_INVERSE_MATRIX_FUNCTOR_H_
#define _VRV_INVERSE_MATRIX_FUNCTOR_H_

#include "VRV/Export.h"
#include "VRV/Functors/Matrix/MatrixFunctor.h"


namespace VRV {
namespace Functors {
namespace Matrix {

class VRV_EXPORT InverseMatrix : public MatrixFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( InverseMatrix );
  typedef VRV::Functors::Matrix::MatrixFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  InverseMatrix ( Unknown *unknown, MatrixFunctor *mf );
  InverseMatrix ( const InverseMatrix &a );

  // Update the matrix.
  virtual void          operator()();

protected:

  virtual ~InverseMatrix();

  MatrixFunctor::RefPtr _mf;
};


} // namespace Matrix
} // namespace Functors
} // namespace CV


#endif // _VRV_INVERSE_MATRIX_FUNCTOR_H_
