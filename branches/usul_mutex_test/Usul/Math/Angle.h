
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Calculate angle between vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_VECTOR_ANGLE_H_
#define _USUL_MATH_VECTOR_ANGLE_H_

#include "Usul/Math/Functions.h"


namespace Usul {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class ScalarType, unsigned int Size > struct Angle;


///////////////////////////////////////////////////////////////////////////////
//
//  3D vectors.
//
///////////////////////////////////////////////////////////////////////////////

template < class ScalarType > struct Angle < ScalarType, 3 >
{
  template < class VectorType > static ScalarType get ( const VectorType &a, const VectorType &b )
  {
    // Get the dot products.
    const ScalarType AdotB ( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] );
    const ScalarType AdotA ( a[0] * a[0] + a[1] * a[1] + a[2] * a[2] );
    const ScalarType BdotB ( b[0] * b[0] + b[1] * b[1] + b[2] * b[2] );

    // Get the lengths.
    const ScalarType lengthA ( Usul::Math::sqrt ( AdotA ) );
    const ScalarType lengthB ( Usul::Math::sqrt ( BdotB ) );

    // Return the angle between the vectors.
    return ( Usul::Math::acos ( AdotB / ( lengthA * lengthB ) ) );
  }
};


} // namespace Math
} // namespace Usul


#endif // _USUL_MATH_VECTOR_ANGLE_H_
