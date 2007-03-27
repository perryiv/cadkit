
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_MATRIX_FUNCTOR_BASE_CLASS_H_
#define _CV_MATRIX_FUNCTOR_BASE_CLASS_H_

#include "CadViewer/Functors/BaseFunctor.h"

#include "Usul/Math/Matrix44.h"


namespace CV {
namespace Functors {


class MatrixFunctor : public BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( MatrixFunctor );
  typedef BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  // Get the matrix.
  const Matrix &        matrix() const { return _matrix; }

protected:

  explicit MatrixFunctor ( Unknown *unknown );
  MatrixFunctor ( const MatrixFunctor &a );
  virtual ~MatrixFunctor();

  Matrix _matrix;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_MATRIX_FUNCTOR_BASE_CLASS_H_
