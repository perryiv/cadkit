
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Distance formula.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_MATH_DISTANCE_FORMULA_H_
#define _GENERIC_NURBS_LIBRARY_MATH_DISTANCE_FORMULA_H_

#include "GN/Macros/ErrorCheck.h"


namespace GN {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the distance between two 1 x n vectors.
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType > struct Distance1
{
  typedef typename CurveType::Vector Vector;
  typedef typename CurveType::Vec2 Vec2;
  typedef typename CurveType::Vec3 Vec3;
  typedef typename CurveType::Vec4 Vec4;
  typedef typename CurveType::SquareRoot SquareRoot;
  typedef typename CurveType::DependentType RealType;
  typedef typename CurveType::ErrorCheckerType ErrorCheckerType;
  typedef Distance1 < CurveType > ThisType;

public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the distance squared.
  //
  /////////////////////////////////////////////////////////////////////////////

  static RealType squared ( const Vec2 &a, const Vec2 &b )
  {
    return 
      ( a[0] - b[0] ) * ( a[0] - b[0] ) +
      ( a[1] - b[1] ) * ( a[1] - b[1] );
  }
  static RealType squared ( const Vec3 &a, const Vec3 &b )
  {
    return 
      ( a[0] - b[0] ) * ( a[0] - b[0] ) +
      ( a[1] - b[1] ) * ( a[1] - b[1] ) +
      ( a[2] - b[2] ) * ( a[2] - b[2] );
  }
  static RealType squared ( const Vec4 &a, const Vec4 &b )
  {
    return 
      ( a[0] - b[0] ) * ( a[0] - b[0] ) +
      ( a[1] - b[1] ) * ( a[1] - b[1] ) +
      ( a[2] - b[2] ) * ( a[2] - b[2] ) +
      ( a[3] - b[3] ) * ( a[3] - b[3] );
  }
  static RealType squared ( const Vector &a, const Vector &b )
  {
    typedef typename Vector::const_iterator Iterator;
    GN_ERROR_CHECK ( a.size() == b.size() );
    RealType dist ( 0 ), diff;
    Iterator i = a.begin();
    Iterator j = b.begin();
    while ( i != a.end() && j != b.end() )
    {
      diff = (*i) - (*j);
      dist += diff * diff;
      ++i;
      ++j;
    }
    return dist;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the distance.
  //
  /////////////////////////////////////////////////////////////////////////////

  static RealType get ( const Vec2 &a, const Vec2 &b )
  {
    return SquareRoot::calculate ( ThisType::squared ( a, b ) );
  }
  static RealType get ( const Vec3 &a, const Vec3 &b )
  {
    return SquareRoot::calculate ( ThisType::squared ( a, b ) );
  }
  static RealType get ( const Vec4 &a, const Vec4 &b )
  {
    return SquareRoot::calculate ( ThisType::squared ( a, b ) );
  }
  static RealType get ( const Vector &a, const Vector &b )
  {
    return SquareRoot::calculate ( squared ( a, b ) );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the squared-distance between two points, each in 2D vectors 
//  where the first index is the dimension.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ContainerType, 
  class ErrorCheckerType
>
struct DistanceSquared2d
{
  typedef typename ContainerType::size_type SizeType;
  typedef typename ContainerType::value_type InnerContainer;
  typedef typename InnerContainer::value_type ValueType;

  static ValueType calculate ( const ContainerType &a, SizeType i, const ContainerType &b, SizeType j )
  {
    GN_ERROR_CHECK ( a.size() == b.size() );
    const SizeType dimension ( a.size() );
    ValueType dist ( 0 );
    for ( SizeType d = 0; d < dimension; ++d )
    {
      GN_ERROR_CHECK ( a[d].size() == b[d].size() );
      ValueType diff ( a[d][i] - b[d][j] );
      dist += diff * diff;
    }
    return dist;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the distance between two points, each in 2D vectors where the 
//  first index is the dimension.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ContainerType, 
  class ErrorCheckerType,
  class SquareRootType
>
struct Distance2d
{
  typedef typename ContainerType::value_type::value_type ValueType;
  typedef GN::Math::DistanceSquared2d < ContainerType, ErrorCheckerType > DistanceSquared;

  static ValueType calculate ( const ContainerType &a, const ContainerType &b )
  {
    return SquareRootType::calculate ( DistanceSquared::calculate ( a, b ) );
  }
};


}; // namespace Math
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_MATH_DISTANCE_FORMULA_H_
