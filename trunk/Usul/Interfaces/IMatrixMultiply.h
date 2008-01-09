
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for multiplying matrices.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MATRIX_MULTIPLY_H_
#define _USUL_INTERFACE_MATRIX_MULTIPLY_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"


namespace Usul {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IMatrixMultiply : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMatrixMultiply );
  typedef Usul::Math::Matrix44<RealType> Matrix;

  /// Id for this interface.
  enum { IID = iid };

  // Post-multiply the component's matrix by the given matrix.
  virtual void              postMultiply ( const Matrix &M ) = 0;

  // Pre-multiply the component's matrix by the given matrix.
  virtual void              preMultiply ( const Matrix &M ) = 0;
};


typedef IMatrixMultiply<1214152049u,float>  IMatrixMultiplyFloat;
typedef IMatrixMultiply<3189708650u,double> IMatrixMultiplyDouble;


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MATRIX_MULTIPLY_H_
