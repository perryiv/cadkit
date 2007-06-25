
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

#ifndef _VRV_MATRIX_FUNCTOR_BASE_CLASS_H_
#define _VRV_MATRIX_FUNCTOR_BASE_CLASS_H_

#include "VRV/Export.h"
#include "VRV/Functors/BaseFunctor.h"

#include "Usul/Math/Matrix44.h"


namespace VRV {
namespace Functors {
namespace Matrix {


class VRV_EXPORT MatrixFunctor : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( MatrixFunctor );
  typedef VRV::Functors::BaseFunctor BaseClass;
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


} // namespace Matrix
} // namespace Functors
} // namespace VRV


#endif // _VRV_MATRIX_FUNCTOR_BASE_CLASS_H_
