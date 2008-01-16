
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

#ifndef _USUL_FUNCTOR_MATRIX_FUNCTOR_BASE_CLASS_H_
#define _USUL_FUNCTOR_MATRIX_FUNCTOR_BASE_CLASS_H_

#include "Usul/Functors/Interaction/Common/BaseFunctor.h"
#include "Usul/Math/Matrix44.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Matrix {


class USUL_EXPORT MatrixFunctor : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44d Matrix;

  // Type information.
  USUL_DECLARE_TYPE_ID ( MatrixFunctor );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MatrixFunctor );

  // Get the matrix.
  const Matrix &        matrix() const { return _matrix; }

protected:

  explicit MatrixFunctor ( Unknown *caller, const std::string &name );
  MatrixFunctor ( const MatrixFunctor &a );
  virtual ~MatrixFunctor();

  Matrix _matrix;
};


} // namespace Matrix
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_MATRIX_FUNCTOR_BASE_CLASS_H_
