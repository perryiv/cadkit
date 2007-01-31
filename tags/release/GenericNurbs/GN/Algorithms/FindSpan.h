
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The find-span algorithm.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_FIND_SPAN_ALGORITHM_H_
#define _GENERIC_NURBS_LIBRARY_FIND_SPAN_ALGORITHM_H_

#include "GN/Macros/ErrorCheck.h"


namespace GN {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for finding the knot span.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct KnotSpan
{
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineType::KnotVector KnotVector;
  typedef typename SplineType::UIntType UIntType;
  typedef typename SplineType::KnotType KnotType;
  typedef typename SplineType::KnotArgument KnotArgument;
  typedef typename SplineType::Limits Limits;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Find the span in the knot vector given the parameter.
  //
  //  knots:      The knot vector.
  //  numCtrPts:  The number of control points for the independent 
  //              direction associated with the given knot vector.
  //  low:        The starting point in the knot vector to look, The 
  //              lowest "low" is the degree (which is what usually 
  //              is passed in).
  //  u:          The parameter we are finding the span for.
  //
  /////////////////////////////////////////////////////////////////////////////

  static UIntType find ( const KnotVector &knots, UIntType numCtrPts, UIntType low, KnotArgument u )
  {
    GN_ERROR_CHECK ( !knots.empty() );
    GN_ERROR_CHECK ( numCtrPts > Limits::MIN_NUM_CTR_PTS );
    GN_ERROR_CHECK ( low > 0 );

    // See if it's the last knot.
    if ( u == knots[numCtrPts] ) 
      return numCtrPts - 1;

    // Set the high value, the low is passed in (usually the degree).
    UIntType high ( numCtrPts ), mid;

    while ( low <= high )
    {
      mid = UIntType ( KnotType ( low + high ) * 0.5f );

      if ( u == knots[mid] )
      {
        // Handle knot multiplicities.
        while ( u == knots[mid + 1] ) 
          ++mid;
        return mid;
      }

      if ( u < knots[mid] ) 
        high = mid - 1; 
      else 
        low = mid + 1; 
    }

    if ( u == knots[high] )
    {
      // Handle knot multiplicities.
      while( u == knots[high + 1] ) 
        ++high;
      return high;
    }

    return low - 1;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Find the span in the knot vector given the parameter.
  //
  //  spline:         The spline.
  //  whichIndepVar:  The independent variable (which knot vector).
  //  u:              The parameter we are finding the span for.
  //
  /////////////////////////////////////////////////////////////////////////////

  static UIntType find ( const SplineType &spline, UIntType whichIndepVar, KnotArgument u )
  {
    const KnotVector &knots = spline.knotVector ( whichIndepVar );
    UIntType numCtrPts ( spline.numControlPoints ( whichIndepVar ) );
    UIntType low ( spline.degree ( whichIndepVar ) );
    return find ( knots, numCtrPts, low, u );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

};


///////////////////////////////////////////////////////////////////////////////
//
//  Find the span in the knot vector given the parameter.
//
//  spline:         The spline.
//  whichIndepVar:  The independent variable (which knot vector).
//  u:              The parameter we are finding the span for.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType >
typename SplineType::UIntType findKnotSpan ( 
  const SplineType &spline, 
  typename SplineType::UIntType whichIndepVar, 
  typename SplineType::KnotArgument u )
{
  typedef typename SplineType::SplineClass SplineClass;
  return Detail::KnotSpan<SplineClass>::find ( spline, whichIndepVar, u );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the span in the knot vector given the parameter.
//
//  curve: Must be a curve.
//  u:     The parameter we are finding the span for.
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType >
typename CurveType::UIntType findKnotSpan ( 
  const CurveType &curve, 
  typename CurveType::KnotArgument u )
{
  GN_IS_CURVE ( CurveType );
  typedef typename CurveType::SplineClass SplineClass;
  return Detail::KnotSpan<SplineClass>::find ( curve, 0, u );
}


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_FIND_SPAN_ALGORITHM_H_
