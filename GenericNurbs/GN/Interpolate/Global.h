
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
#include "GN/Math/Matrix.h"
#include "GN/Algorithms/FindSpan.h"
#include "GN/Algorithms/BasisFunctions.h"

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
    class SizeType,
    class MatrixType, 
    class IndependentSequenceType, 
    class BasisFunctionsType
  >
  struct BlendingMatrix
  {
    static void fill ( const SizeType &order,
                       const IndependentSequenceType &params,
                       const IndependentSequenceType &knots,
                       BasisFunctionsType &N,
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
        span = GN::Algorithms::findKnotSpan ( knots, numCtrPts, low, params[i] );

        // Calculate the blending functions.
        GN::Algorithms::basisFunctions ( spline, 0, span, params[i], N );

        // Fill in the matrix.
        for ( SlInt32 j = 0; j < order; ++j )
          matrix[i][span-degree+j] = N[j];
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
  GN_CAN_BE_CURVE ( CurveType );
  typedef typename CurveType::SplineClass SplineClass;
  typedef typename SplineClass::SizeType SizeType;
  typedef typename SplineClass::SizeContainer SizeContainer;
  typedef typename SplineClass::DependentContainer MatrixContainer;
  typedef typename SplineClass::DependentTester DependentTester;
  typedef typename SplineClass::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineClass::WorkSpace BasisFunctions;
  typedef GN::Interpolate::Detail::BlendingMatrix<SizeType,MatrixType,IndependentSequenceType,BasisFunctions> BlendingMatrix;

  // Get reference to spline.
  SplineClass &spline = curve.spline();

  // Needed properties.
  SizeType dimension ( points.size() );
  SizeType numCtrPts ( params.size() );
  bool rational ( false );

  // Make sure there is room.
  spline.resize ( dimension, order, numCtrPts, rational );

  // Should be true.
  SizeType numKnots ( knots.size()  );
  GN_ERROR_CHECK ( numCtrPts + order == numKnots );
  GN_ERROR_CHECK ( 1 == spline.numIndepVars() );
  GN_ERROR_CHECK ( spline.numKnots ( 0 ) == numKnots );

  // Assign the knots.
  for ( SizeType i = 0; i < numKnots; ++i )
    spline.knot(0,i) = knots[i];

  // Perform LU decomposition if requested.
  if ( doLU )
  {
    // Fill the matrix.
    BlendingMatrix::fill ( order, params, knots, spline.work().basis, matrix );

    // Perform the LU decomposition.
    matrix.luDecomp ( pivots );

    // Make sure we have all good numbers.
    matrix.apply ( DependentTester() );
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
  typedef typename CurveType::DependentContainer SizeContainer;
  typedef typename CurveType::DependentContainer MatrixContainer;
  typedef typename CurveType::DependentTester DependentTester;
  typedef typename CurveType::ErrorCheckerType ErrorCheckerType;
  typedef GN::Math::Matrix < SizeType, MatrixContainer, SizeContainer, DependentTester, ErrorCheckerType > Matrix;

  // Declare matrix "A" in Ax = b.
  Matrix matrix ( params.size(), params.size() );

  // Need the pivot vector too.
  SizeContainer pivots;

  // Call the other one.
  // Uncomment and deal with compiler errors...
  //GN::Interpolate::global ( order, params, knots, points, true, matrix, pivots, curve );
}


}; // namespace Interpolate
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_
