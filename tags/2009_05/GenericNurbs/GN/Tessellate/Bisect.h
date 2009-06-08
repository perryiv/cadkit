
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Tessellation via bisection.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_BISECTION_TESSELLATION_H_
#define _GENERIC_NURBS_LIBRARY_BISECTION_TESSELLATION_H_

#include "GN/Evaluate/Point.h"
#include "GN/Macros/ErrorCheck.h"
#include "GN/MPL/TypeCheck.h"
#include "GN/Math/Distance.h"
#include "GN/Math/Absolute.h"
#include "GN/Math/LinearBlend.h"
#include "GN/Predicates/Finite.h"
#include "GN/Algorithms/Sort.h"


namespace GN {
namespace Tessellate {


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class that bisects a curve.
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType, class IndependentSequenceType > struct Bisector
{
  typedef typename CurveType::ErrorCheckerType ErrorCheckerType;
  typedef typename CurveType::IndependentArgument IndependentArgument;
  typedef typename CurveType::IndependentType IndependentType;
  typedef typename CurveType::IndependentTester IndependentTester;
  typedef typename CurveType::DependentTester DependentTester;
  typedef typename CurveType::DependentType DependentType;
  typedef typename CurveType::DependentArgument DependentArgument;
  typedef typename CurveType::Vector PointType;
  typedef typename GN::Math::Distance1<CurveType> Distance;
  typedef typename GN::Math::LinearBlend<PointType> LinearBlend;
  typedef typename GN::Predicates::Finite<CurveType> Finite;
  typedef typename IndependentSequenceType::const_iterator IndependentIterator;
  typedef typename std::iterator_traits<IndependentIterator>::iterator_category IndependentIteratorTag;
  typedef typename GN::Algorithms::Sort<IndependentIteratorTag> Sort;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Fill the given container with the tessellation points. Note: we do not 
  //  initialize the incoming container to support repeated calls.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void tessellate ( const CurveType &curve,
                           IndependentArgument u0, 
                           IndependentArgument u1, 
                           DependentArgument chordHeight, 
                           IndependentSequenceType &u )
  {
    GN_CAN_BE_CURVE ( CurveType );
    GN_ERROR_CHECK ( u0 >= curve.firstKnot ( 0 ) );
    GN_ERROR_CHECK ( u0 < u1 );
    GN_ERROR_CHECK ( u1 <= curve.lastKnot ( 0 ) );
    GN_ERROR_CHECK ( chordHeight > 0 );

    // Get the points on the boundaries.
    PointType pt0, pt1;
    pt0.resize ( curve.dimension() );
    pt1.resize ( curve.dimension() );
    GN::Evaluate::point ( curve, u0, pt0 );
    GN::Evaluate::point ( curve, u1, pt1 );

    // Append the container.
    u.insert ( u.end(), u0 );
    u.insert ( u.end(), u1 );

    // Bisect the curve until the tolerance is met.
    _bisect ( curve, u0, u1, pt0, pt1, chordHeight * chordHeight, u );

    // Sort the parameters.
    Sort::ascending ( u );
  }

private:

  //////////////////////////////////////////////////////////////////////////////
  //
  //  Test the midpoint between u0 and u1. If it's not within tolerance then 
  //  bisect the curve again by recursively calling this function.
  //
  //////////////////////////////////////////////////////////////////////////////

  static void _bisect ( const CurveType &curve, 
                        IndependentArgument u0, 
                        IndependentArgument u2, 
                        const PointType &pt0, 
                        const PointType &pt2, 
                        DependentArgument chordHeightSquared, 
                        IndependentSequenceType &u )
  {
    GN_CAN_BE_CURVE ( CurveType );
    GN_ERROR_CHECK ( chordHeightSquared > 0 );
    GN_ERROR_CHECK ( IndependentTester::finite ( u0 ) );
    GN_ERROR_CHECK ( IndependentTester::finite ( u2 ) );
    GN_ERROR_CHECK ( Finite::check ( pt0 ) );
    GN_ERROR_CHECK ( Finite::check ( pt2 ) );
    GN_ERROR_CHECK ( u0 >= curve.firstKnot ( 0 ) );
    GN_ERROR_CHECK ( u0 < u2 );
    GN_ERROR_CHECK ( u2 <= curve.lastKnot ( 0 ) );
    GN_ERROR_CHECK ( pt0.size() == pt2.size() );
    GN_ERROR_CHECK ( pt0.size() == curve.dimension() );

    // If the given range is zero then return. Sometimes this happens when 
    // the curve has repeated knots or a discontinuity. The right way to fix 
    // it is to tessellate each knot span separately.
    if ( GN::Math::absolute ( u0 - u2 ) < 1e-8f ) // TODO, remove hard-coded value.
      return;

    // Get the parametric middle.
    IndependentType u1 ( ( u0 + u2 ) * 0.5f );

    // Make sure we're still in the range of good numbers.
    GN_ERROR_CHECK ( IndependentTester::finite ( u1 ) );

    // Evaluate the point at the parametric middle.
    PointType pt1;
    pt1.resize ( curve.dimension() );
    GN::Evaluate::point ( curve, u1, pt1 );

    // Make sure it's a good number.
    GN_ERROR_CHECK ( Finite::check ( pt1 ) );

    // Calculate the mid-point of the end points.
    PointType mid;
    mid.resize ( pt0.size() );
    LinearBlend average ( 0.5f );
    std::transform ( pt0.begin(), pt0.end(), pt2.begin(), mid.begin(), average );

    // Make sure it's a good number.
    GN_ERROR_CHECK ( Finite::check ( mid ) );

    // Get the squared distance between the geometric and 
    // parametric midpoints.
    DependentType dist ( Distance::squared ( mid, pt1 ) );

    // Make sure it's a valid number.
    GN_ERROR_CHECK ( DependentTester::finite ( dist ) );

    // If the distance is within tolerance then return now.
    if ( dist < chordHeightSquared )
      return;

    // Otherwise, keep the parameter and bisect again.
    u.insert ( u.end(), u1 );
    _bisect ( curve, u0, u1, pt0, pt1, chordHeightSquared, u );
    _bisect ( curve, u1, u2, pt1, pt2, chordHeightSquared, u );
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
//  Bisect the curve until a chord-height tolerance is met.
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType, class IndependentSequenceType > inline
void bisect ( const CurveType &curve,
              typename CurveType::IndependentArgument u0,
              typename CurveType::IndependentArgument u1,
              typename CurveType::DependentArgument chordHeight,
              IndependentSequenceType &u )
{
  GN_CAN_BE_CURVE ( CurveType );
  typedef typename CurveType::SplineClass SC;
  typedef IndependentSequenceType PCT;
  return Detail::Bisector<SC,PCT>::tessellate ( curve, u0, u1, chordHeight, u );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bisect the curve until a chord-height tolerance is met.
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType, class IndependentSequenceType > inline
void bisect ( const CurveType &curve,
              typename CurveType::DependentArgument chordHeight,
              IndependentSequenceType &u )
{
  GN_CAN_BE_CURVE ( CurveType );
  return bisect ( curve, curve.firstKnot ( 0 ), curve.lastKnot ( 0 ), chordHeight, u );
}


}; // namespace Tessellate
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_BISECTION_TESSELLATION_H_
