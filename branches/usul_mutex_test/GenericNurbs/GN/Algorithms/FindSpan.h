
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
#include "GN/Traits/Arguments.h"
#include "GN/Limits/Nurbs.h"
#include "GN/MPL/TypeCheck.h"


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

template
<
  class IndependentSequence_,
  class SizeType_,
  class ErrorCheckerType_
>
struct KnotSpan
{
  typedef IndependentSequence_                                  IndependentSequence;
  typedef typename IndependentSequence::value_type              IndependentType;
  typedef typename GN::Traits::Argument<IndependentType>::Type  IndependentArgument;
  typedef SizeType_                                             SizeType;
  typedef GN::Limits::Nurbs<SizeType>                           Limits;
  typedef ErrorCheckerType_                                     ErrorCheckerType;


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

  static SizeType find ( const IndependentSequence &knots, SizeType numCtrPts, SizeType low, IndependentArgument u )
  {
    GN_ERROR_CHECK ( !knots.empty() );
    GN_ERROR_CHECK ( numCtrPts >= Limits::MIN_NUM_CTR_PTS );
    GN_ERROR_CHECK ( low > 0 );

    // See if it's the last knot.
    if ( u == knots[numCtrPts] ) 
      return numCtrPts - 1;

    // Set the high value, the low is passed in (usually the degree).
    SizeType high ( numCtrPts ), mid;

    while ( low <= high )
    {
      mid = SizeType ( IndependentType ( low + high ) * 0.5f );

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
//  knots:          The knot vector.
//  u:              The parameter we are finding the span for.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline
typename SplineType::SizeType findKnotSpan ( const SplineType &spline, 
                                             typename SplineType::SizeType whichIndepVar, 
                                             typename SplineType::IndependentArgument u )
{
  // Declare types.
  typedef typename SplineType::SplineClass SplineClass;
  typedef typename SplineType::IndependentSequence IndependentSequence;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef Detail::KnotSpan<IndependentSequence,SizeType,ErrorCheckerType> KnotSpan;

  // Get the data we need.
  const IndependentSequence &knots = spline.knotVector ( whichIndepVar );
  SizeType numCtrPts ( spline.numControlPoints ( whichIndepVar ) );
  SizeType low ( spline.degree ( whichIndepVar ) );

  // Call helper function.
  return KnotSpan::find ( knots, numCtrPts, low, u );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the span in the knot vector given the parameter.
//
//  curve: Must be a curve.
//  u:     The parameter we are finding the span for.
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType > inline
typename CurveType::SizeType findKnotSpan ( const CurveType &curve, 
                                            typename CurveType::IndependentArgument u )
{
  // Has to be a curve.
  GN_IS_CURVE ( CurveType );

  // Delegate.
  return GN::Algorithms::findKnotSpan ( curve.spline(), 0, u );
}


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_FIND_SPAN_ALGORITHM_H_
