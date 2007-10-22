
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_
#define _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_

#include "GN/Macros/ErrorCheck.h"
#include "GN/MPL/TypeCheck.h"
#include "GN/MPL/StaticAssert.h"
#include "GN/Math/Matrix.h"
#include "GN/Algorithms/FindSpan.h"
#include "GN/Algorithms/BasisFunctions.h"
#include "GN/Algorithms/Parameterize.h"
#include "GN/Algorithms/KnotVector.h"

#include <algorithm>


namespace GN {
namespace Interpolate {


///////////////////////////////////////////////////////////////////////////////
//
//  Fill matrix with blending functions and zeros.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template
  <
    class SizeType_,
    class MatrixType_, 
    class IndependentSequence_, 
    class BlendingCoefficients_,
    class WorkSpace_,
    class ErrorCheckerType_
  >
  struct BlendingMatrix
  {
    typedef SizeType_ SizeType;
    typedef MatrixType_ MatrixType;
    typedef IndependentSequence_ IndependentSequence;
    typedef BlendingCoefficients_ BlendingCoefficients;
    typedef WorkSpace_ WorkSpace;
    typedef ErrorCheckerType_ ErrorCheckerType;
    typedef GN::Algorithms::Detail::KnotSpan<IndependentSequence,SizeType,ErrorCheckerType> KnotSpan;
    typedef GN::Algorithms::Detail::BasisFunctions
    <
      IndependentSequence,
      SizeType,
      BlendingCoefficients,
      WorkSpace,
      ErrorCheckerType
    >
    BasisFunctions;

    static void fill ( const SizeType &order,
                       const IndependentSequence &params,
                       const IndependentSequence &knots,
                       BlendingCoefficients &N,
                       WorkSpace &left,
                       WorkSpace &right,
                       MatrixType &matrix )
    {
      // Initialize the matrix.
      SizeType numCtrPts ( params.size() );
      matrix.resize ( numCtrPts, numCtrPts );
      matrix.set ( 0 );

      // Needed below.
      SizeType span ( 0 ), low;
      SizeType degree ( order - 1 );

      // Space for the blending functions.
      N.accommodate ( order );

      // Fill the matrix with the appropriate blending function values.
      // Adopted from "The NURBS Book", Algorithm A9.1, page 369.
      for ( SizeType i = 0; i < numCtrPts; ++i )
      {
        // Find the span.
        low = std::max ( degree, span );
        span = KnotSpan::find ( knots, numCtrPts, low, params[i] );

        // Calculate the blending functions.
        BasisFunctions::calculate ( knots, order, span, params[i], N, left, right );

        // Fill in the matrix.
        for ( SizeType j = 0; j < order; ++j )
          matrix(i,span-degree+j) = N[j];
      }
    }
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class CurveType, 
  class IndependentSequenceType, 
  class DependentContainerType,
  class MatrixType
>
void global ( const typename CurveType::SizeType &order,
              const IndependentSequenceType &params,
              const IndependentSequenceType &knots,
              const DependentContainerType &points,
              bool doLU,
              MatrixType &matrix,
              typename MatrixType::SizeContainer &pivots,
              CurveType &curve )
{
  typedef typename CurveType::SplineClass SplineClass;
  typedef typename SplineClass::SizeType SizeType;
  typedef typename SplineClass::SizeContainer SizeContainer;
  typedef typename SplineClass::DependentContainer MatrixContainer;
  typedef typename SplineClass::DependentTester DependentTester;
  typedef typename SplineClass::DependentSequence DependentSequence;
  typedef typename SplineClass::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineClass::WorkSpace WorkSpace;
  typedef GN::Interpolate::Detail::BlendingMatrix
  <
    SizeType,
    MatrixType,
    IndependentSequenceType,
    WorkSpace,
    WorkSpace,
    ErrorCheckerType
  >
  BlendingMatrix;

  GN_CAN_BE_CURVE ( CurveType );
  GN_ASSERT_SAME_TYPE ( typename SizeContainer::value_type, typename MatrixType::SizeContainer::value_type );
  GN_ASSERT_SAME_TYPE ( typename SizeContainer::value_type, SizeType );

  // Get reference to spline.
  SplineClass &spline = curve.spline();

  // Needed properties.
  SizeType dimension ( points.size() );
  SizeType numCtrPts ( params.size() );
  bool rational ( false );

  // Make sure there is room.
  spline.resize ( dimension, order, numCtrPts, rational );

  // Should be true.
  SizeType numKnots ( knots.size() );
  GN_ERROR_CHECK ( numCtrPts + order == numKnots );
  GN_ERROR_CHECK ( 1 == spline.numIndepVars() );
  GN_ERROR_CHECK ( spline.numKnots ( 0 ) == numKnots );

  // Assign the knots.
  for ( SizeType i = 0; i < numKnots; ++i )
    spline.knot(0,i) = knots[i];

  // Perform LU decomposition if requested.
  if ( doLU )
  {
    // Shortcuts.
    WorkSpace &N = spline.work ( 0 ).basis;
    WorkSpace &left = spline.work ( 0 ).left;
    WorkSpace &right = spline.work ( 0 ).right;

    // Fill the matrix.
    BlendingMatrix::fill ( order, params, knots, N, left, right, matrix );

#if 0
    matrix.print ( "%12.8f", std::cout );
#endif

    // Perform the LU decomposition.
    matrix.luDecomp ( pivots );

    // Make sure we have all good numbers.
    matrix.apply ( DependentTester::finite );
  }

  // For each dimension...
  for ( SizeType i = 0; i < dimension; ++i )
  {
    // Shortcut to this dimension's control points.
    DependentSequence &ctrPts = spline.controlPoints ( i );

    // Copy the data points to the control points.
    GN_ERROR_CHECK ( points[i].size() == ctrPts.size() );
    std::copy ( points[i].begin(), points[i].end(), ctrPts.begin() );

    // Solve for the control points (Ax=b).
    matrix.luSolve ( pivots, ctrPts );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class CurveType, 
  class IndependentSequenceType, 
  class DependentContainerType
>
void global ( const typename CurveType::SizeType &order,
              const IndependentSequenceType &params,
              const IndependentSequenceType &knots,
              const DependentContainerType &points,
              CurveType &curve )
{
  GN_CAN_BE_CURVE ( CurveType );
  typedef typename CurveType::SizeType SizeType;
  typedef typename CurveType::SizeContainer SizeContainer;
  typedef typename CurveType::DependentContainer MatrixContainer;
  typedef typename CurveType::DependentTester DependentTester;
  typedef typename CurveType::ErrorCheckerType ErrorCheckerType;
  typedef GN::Math::Matrix < SizeType, MatrixContainer, SizeContainer, DependentTester, ErrorCheckerType > Matrix;

  // Declare matrix "A" in Ax = b.
  Matrix matrix ( params.size(), params.size() );

  // Need the pivot vector too.
  SizeContainer pivots;

  // Call the other one.
  GN::Interpolate::global ( order, params, knots, points, true, matrix, pivots, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class CurveType, 
  class DependentContainerType
>
void global ( const typename CurveType::SizeType &order,
              const DependentContainerType &points,
              typename CurveType::IndependentArgument power,
              CurveType &curve )
{
  GN_CAN_BE_CURVE ( CurveType );
  typedef typename CurveType::SizeType SizeType;
  typedef typename CurveType::IndependentSequence IndependentSequence;
  typedef typename CurveType::Power PowerFunctor;
  typedef typename CurveType::ErrorCheckerType ErrorCheckerType;
  typedef GN::Algorithms::Parameterize < IndependentSequence, DependentContainerType, PowerFunctor, ErrorCheckerType > Parameterize;
  typedef GN::Algorithms::KnotVector < IndependentSequence, ErrorCheckerType > KnotVectorBuilder;
  GN_ERROR_CHECK ( false == points.empty() );

  // Make the parameters.
  IndependentSequence params;
  Parameterize::fit ( points, power, params );

#if 0
  {
    typedef typename CurveType::IndependentType IndependentType;
    std::ostringstream out;
    std::copy ( params.begin(), params.end(), std::ostream_iterator<IndependentType> ( out, "\n" ) );
    ::OutputDebugString ( "params = \n" );
    ::OutputDebugString ( out.str().c_str() );
  }
#endif

  // Make the knot vector. Size it for interpolation.
  IndependentSequence knots;
  SizeType numDataPts ( points.at(0).size() );
  knots.resize ( numDataPts + order );
  KnotVectorBuilder::build ( params, order, knots );

#if 0
  {
    typedef typename CurveType::IndependentType IndependentType;
    std::ostringstream out;
    std::copy ( knots.begin(), knots.end(), std::ostream_iterator<IndependentType> ( out, "\n" ) );
    ::OutputDebugString ( "knots = \n" );
    ::OutputDebugString ( out.str().c_str() );
  }
#endif

  // Call the other one.
  GN::Interpolate::global ( order, params, knots, points, curve );
}


}; // namespace Interpolate
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_
