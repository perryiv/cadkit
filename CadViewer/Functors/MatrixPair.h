
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

#ifndef _CV_MATRIX_FUNCTOR_PAIR_H_
#define _CV_MATRIX_FUNCTOR_PAIR_H_

#include "VRV/Functors/Matrix/MatrixFunctor.h"


namespace CV {
namespace Functors {


class MatrixPair : public VRV::Functors::Matrix::MatrixFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( MatrixPair );
  typedef VRV::Functors::Matrix::MatrixFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  MatrixPair ( 
    Unknown *unknown, 
    MatrixFunctor *c1, 
    MatrixFunctor *c2 );
  MatrixPair ( const MatrixPair &tp );

  // Update the matrix.
  virtual void          operator()();

protected:

  virtual ~MatrixPair();

  MatrixFunctor::ValidRefPtr _c1;
  MatrixFunctor::ValidRefPtr _c2;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_MATRIX_FUNCTOR_PAIR_H_
