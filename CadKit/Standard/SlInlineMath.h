
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlInlineMath: Inline functions for math.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INLINE_MATH_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_INLINE_MATH_FUNCTIONS_H_

#include "SlAssert.h"

#ifdef CADKIT_DEFINE_SL_FUNCTIONS_THAT_NEED_TEMPLATE_SUPPORT
# include "SlTemplateSupport.h"
#endif

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#endif // _CADKIT_USE_PRECOMPILED_HEADERS


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Swap the values of a and b.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void swap ( T &a, T &b, T &temp )
{
  temp = a;
  a = b;
  b = temp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is a equal to (within tolerance of) b?
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool isEqual ( const T &a, const T &b, const T &tol )
{
  return ( a < b ) ? ( b - a ) < tol : ( a - b ) < tol;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the array increasing?
//
///////////////////////////////////////////////////////////////////////////////

template<class ArrayType, class IndexType> inline bool isIncreasing ( const ArrayType *array, const IndexType &num )
{
  for ( IndexType i = 1; i < num; ++i )
  {
    if ( array[i] <= array[i-1] ) 
      return false;
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Absolute value. Object must have operator - () defined.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T absolute ( const T &num )
{
  return ( num < 0 ) ? -num : num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the maximum of a, b, and c.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &maximum ( const T &a, const T &b, const T &c )
{
  return ( a >= b ) ? ( ( a >= c ) ? a : c ) : ( ( b >= c ) ? b : c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the minimum of a, b, and c.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &minimum ( const T &a, const T &b, const T &c )
{
  return ( a <= b ) ? ( ( a <= c ) ? a : c ) : ( ( b <= c ) ? b : c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the middle of a, b, and c. For example, if a < b < c, return b.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> const T &middle ( const T &a, const T &b, const T &c )
{
  if ( b < c )
  {
         if ( a < b ) return b;
    else if ( a < c ) return a;
    else              return c;
  }

  else // b >= c
  {
         if ( a > b ) return b;
    else if ( a > c ) return a;
    else              return c;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the maximum of a and b.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &maximum ( const T &a, const T &b )
{
  return ( a > b ) ? a : b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the minimum of a and b.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &minimum ( const T &a, const T &b )
{
  return ( a < b ) ? a : b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Something to the power of 2.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T power2 ( const T &num )
{
  return ( num * num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Something to the power of 3.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T power3 ( const T &num )
{
  return ( CadKit::power2 ( num ) * num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Something to the power of 4.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T power4 ( const T &num )
{
  return ( CadKit::power3 ( num ) * num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Something to the power of 5.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T power5 ( const T &num )
{
  return ( CadKit::power4 ( num ) * num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Square (for convenience).
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T square ( const T &num )
{
  return ( CadKit::power2 ( num ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cube (for convenience).
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T cube ( const T &num )
{
  return ( CadKit::power3 ( num ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Round the floating point value up or down 
//  result: rounds up if value is >= x.5
//          rounds down if value is < x.5
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T round ( T value )
{
  return static_cast<T> ( static_cast<long> ( value + static_cast<T> ( 0.5 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the dot product.
//
///////////////////////////////////////////////////////////////////////////////

template<class VecType, class IndexType> inline VecType getDotProduct ( 
  const VecType *vecA, 
  const VecType *vecB, 
  const IndexType &dimension )
{
  SL_ASSERT ( vecA && vecB && dimension > 0 );
  VecType product ( 0 );
  for ( IndexType i = 0; i < dimension; ++i ) 
    product += vecA[i] * vecB[i];
  return product;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set all the elements of the array.
//
///////////////////////////////////////////////////////////////////////////////

template<class ArrayType, class IndexType> inline void setArray ( 
  const IndexType &size, 
  const IndexType &value, 
  ArrayType *array )
{
  for ( IndexType i = 0; i < size; ++i ) 
    array[i] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the distance squared.
//
///////////////////////////////////////////////////////////////////////////////

template<class DistanceType, class DimensionType> inline DistanceType getDistanceSquared ( 
  const DistanceType *ptA, const DistanceType *ptB, const DimensionType &dimension )
{
  SL_ASSERT ( ptA && ptB && dimension > 0 );
  DistanceType dist ( 0 );
  for ( DimensionType i = 0; i < dimension; ++i )
    dist += CadKit::square ( ptA[i] - ptB[i] );
  return dist;
}


#ifdef CADKIT_DEFINE_SL_FUNCTIONS_THAT_NEED_TEMPLATE_SUPPORT


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance between the n-dimensional points.
//
///////////////////////////////////////////////////////////////////////////////

template<class DistanceType, class DimensionType> inline DistanceType getDistance 
  ( const DistanceType *ptA, const DistanceType *ptB, const DimensionType &dimension )
{
  return CadKit::getSquareRoot ( CadKit::getDistanceSquared ( ptA, ptB, dimension ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the length of the n-dimensional vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class VectorType, class DimensionType> inline VectorType getVectorLength 
  ( const VectorType *vec, const DimensionType &dimension )
{
  return CadKit::getSquareRoot ( CadKit::getDotProduct ( vec, vec, dimension ) );
}


#endif // CADKIT_DEFINE_SL_FUNCTIONS_THAT_NEED_TEMPLATE_SUPPORT



///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector difference (vecC = vecA - vecB).
//
///////////////////////////////////////////////////////////////////////////////

template<class VectorType, class DimensionType> inline void getVectorDifference ( 
  const VectorType *vecA, const VectorType *vecB, const DimensionType &dimension, VectorType *vecC )
{
  SL_ASSERT ( vecA && vecB && vecC && dimension > 0 );
  for ( DimensionType i = 0; i < dimension; ++i )
    vecC[i] = vecA[i] - vecB[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale the vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class VectorType, class DimensionType> inline void scaleVector ( 
  const VectorType &scale, const DimensionType &dimension, VectorType *vec )
{
  SL_ASSERT ( vec && dimension > 0 );
  for ( DimensionType i = 0; i < dimension; ++i ) 
    vec[i] *= scale;
}


}; // namespace CadKit.


///////////////////////////////////////////////////////////////////////////////
//
// For convenience and backward compatability.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_SWAP   CadKit::swap
#define SL_ABS    CadKit::absolute
#define SL_MIN    CadKit::minimum
#define SL_MAX    CadKit::maximum
#define SL_MID    CadKit::middle
#define SL_SQUARE CadKit::square
#define SL_CUBE   CadKit::cube
#define SL_POW2   CadKit::power2
#define SL_POW3   CadKit::power3
#define SL_POW4   CadKit::power4
#define SL_POW5   CadKit::power5
#define SL_EQUAL  CadKit::isEqual


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_MATH_FUNCTIONS_H_
