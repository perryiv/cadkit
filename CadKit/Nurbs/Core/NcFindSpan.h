
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcFindSpan.h: Algorithm for finding the span in the knot vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SPLINE_FIND_SPAN_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SPLINE_FIND_SPAN_CLASS_H_

#include "NcAlgorithm.h"
#include "NcCurve.h"
#include "NcSurface.h"

#include "Standard/SlAssert.h"


namespace CadKit
{
template<NCSDTA> class NcFindSpan : public NcAlgorithm<NCSDCA>
{
public:

  /// Find the span.
  static IndexType              find ( 
                                  const NcCurve<NCSDCA> &curve,
                                  const ParameterType &u );

  /// Find the span.
  static IndexType              find ( 
                                  const NcCurve<NCSDCA> &curve,
                                  const ParameterType &u, 
                                  const IndexType &low );

  /// Find the span.
  static IndexType              find ( 
                                  const NcSpline<NCSDCA> &spline,
                                  const IndexType &whichIndepVar, 
                                  const ParameterType &u );

  /// Find the span.
  static IndexType              find ( 
                                  const NcSpline<NCSDCA> &spline,
                                  const IndexType &whichIndepVar, 
                                  const ParameterType &u, 
                                  const IndexType &low );

  /// Find the span.
  static IndexType              find ( 
                                  const ParameterType *knots, 
                                  const IndexType &numCtrPts, 
                                  const ParameterType &u, 
                                  IndexType low );
};


///////////////////////////////////////////////////////////////////////////////
///
/// Find the span in the knot vector given the parameter.
///
/// \param knots      The knot vector.
/// \param numCtrPts  The number of control points for the independent 
///                   direction associated with the given knot vector.
/// \param u          The parameter we are finding the span for.
/// \param low        The starting point in the knot vector to look, The 
///                   lowest "low" is the degree (which is what usually 
///                   is passed in). It gets changed.
/// \return           The span index.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline IndexType NcFindSpan<NCSDCA>::find ( 
  const ParameterType *knots, 
  const IndexType &numCtrPts, 
  const ParameterType &u, 
  IndexType low )
{
  SL_ASSERT ( NULL != knots );
  SL_ASSERT ( numCtrPts > 1 );
  SL_ASSERT ( low == 0 || low > 0 ); // Bypass g++ warning.

  // See if it's the last knot.
  if ( u == knots[numCtrPts] ) 
    return numCtrPts - 1;

  // Set the high value, the low is passed in (usually the degree).
  IndexType high ( numCtrPts ), mid;

  while ( low <= high )
  {
    mid = static_cast<IndexType> ( static_cast<ParameterType> ( low + high ) * 0.5f );  

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
    while ( u == knots[high + 1] ) 
      ++high;
    return high;
  }

  return low - 1;
}


///////////////////////////////////////////////////////////////////////////////
///
/// Find the span in the knot vector given the parameter.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline IndexType NcFindSpan<NCSDCA>::find (
  const NcSpline<NCSDCA> &spline,
  const IndexType &whichIndepVar, 
  const ParameterType &u, 
  const IndexType &low )
{
  SL_ASSERT ( true == spline.isValid() );
  SL_ASSERT ( low > spline.getDegree ( whichIndepVar ) || low == spline.getDegree ( whichIndepVar ) );
  SL_ASSERT ( true == spline.isInRange ( whichIndepVar, u ) );

  // Get the start of the knot vector.
  const ParameterType *knots = spline.getKnots ( whichIndepVar );

  // Get the number of controls points.
  const IndexType numCtrPtr ( spline.getNumCtrPts ( whichIndepVar ) );

  // Call the function to find the span.
  return NcFindSpan<NCSDCA>::find ( knots, numCtrPtr, u, low );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Find the span in the knot vector given the parameter.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline IndexType NcFindSpan<NCSDCA>::find (
  const NcSpline<NCSDCA> &spline,
  const IndexType &whichIndepVar, 
  const ParameterType &u )
{
  // Call the other one.
  return NcFindSpan<NCSDCA>::find ( spline, whichIndepVar, u, spline.getDegree ( whichIndepVar ) );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Find the span in the knot vector given the parameter.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline IndexType NcFindSpan<NCSDCA>::find (
  const NcCurve<NCSDCA> &curve,
  const ParameterType &u, 
  const IndexType &low )
{
  // Call the other one.
  return NcFindSpan<NCSDCA>::find ( curve, 0, u, low );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Find the span in the knot vector given the parameter.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline IndexType NcFindSpan<NCSDCA>::find (
  const NcCurve<NCSDCA> &curve,
  const ParameterType &u )
{
  // Call the other one.
  return NcFindSpan<NCSDCA>::find ( curve, 0, u, curve.getDegree() );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_FIND_SPAN_CLASS_H_
