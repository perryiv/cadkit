
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
# ifdef __CYGWIN__
#  include <numeric>
# else
#  include <limits>
# endif
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the value to the range.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void clamp ( const T &minValue, const T &maxValue, T &value )
{
  if ( value < minValue )
    value = minValue;
  else if ( value > maxValue )
    value = maxValue;
}


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
//  Return the maximum element in the array.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline const T maximum ( const I &num, const T *array )
{
  SL_ASSERT ( num > 0 );
  SL_ASSERT ( array );

  // Initialize.
  T theMax ( std::numeric_limits<T>::min() );

  // Loop through.
  for ( I i = 0; i < num; ++i )
  {
    if ( array[i] > theMax )
    {
      theMax = array[i];
    }
  }

  // Return the max.
  return theMax;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the minimum element in the array.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline const T &minimum ( const I &num, const T *array )
{
  SL_ASSERT ( num > 0 );
  SL_ASSERT ( array );

  // Initialize.
  T theMin ( numeric_limits<T>::max() );

  // Loop through.
  for ( I i = 0; i < num; ++i )
  {
    if ( array[i] < theMin )
    {
      theMin = array[i];
    }
  }

  // Return the min.
  return theMin;
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
#define SL_CLAMP  CadKit::clamp


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_MATH_FUNCTIONS_H_
