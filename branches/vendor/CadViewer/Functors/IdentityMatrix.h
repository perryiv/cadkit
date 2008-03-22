
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Identity matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_IDENTITY_MATRIX_FUNCTOR_H_
#define _CV_IDENTITY_MATRIX_FUNCTOR_H_

#include "CadViewer/Functors/MatrixFunctor.h"


namespace CV {
namespace Functors {


class IdentityMatrix : public MatrixFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( IdentityMatrix );
  typedef MatrixFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  explicit IdentityMatrix ( Unknown *unknown );
  IdentityMatrix ( const IdentityMatrix &a );

  // Update the matrix.
  virtual void          operator()();

protected:

  virtual ~IdentityMatrix();
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_IDENTITY_MATRIX_FUNCTOR_H_