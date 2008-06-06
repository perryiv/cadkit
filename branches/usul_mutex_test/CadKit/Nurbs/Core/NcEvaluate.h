
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcEvaluate.h: Algorithms for evaluating a spline.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SPLINE_EVALUATION_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SPLINE_EVALUATION_CLASS_H_

#include "NcFindSpan.h"
#include "NcBasisFunctions.h"
#include "NcCurve.h"
#include "NcDefine.h"

#include "Standard/SlAssert.h"
#include "Standard/SlConstants.h"


namespace CadKit
{
template<NCSDTA> class NcEvaluate : public NcAlgorithm<NCSDCA>
{
public:

  /// Evaluate a point on the curve.
  static void                   evaluate ( 
                                  const NcCurve<NCSDCA> &curve, 
                                  const ParamType &u, 
                                  CtrPtType *point );

  /// Evaluate a point on the curve.
  static void                   evaluate ( 
                                  const IndexType &degree, 
                                  const IndexType &span, 
                                  const IndexType &dimension,
                                  const ParamType &u, 
                                  const ParamType *N,
                                  const CtrPtType **ctrPts,
                                  const CtrPtType *weights,
                                  CtrPtType *point );
};


///////////////////////////////////////////////////////////////////////////////
///
/// Evaluate a point on the curve.
///
/// \param degree     The degree of the curve.
/// \param span       The span in the knot vector where u falls.
/// \param dimension  The dimension of the curve.
/// \param u          The parameter we are finding the point at.
/// \param N          The basis functions (array size >= order).
/// \param weights    The control points.
/// \param weights    Weights if the curve is rational, NULL otherwise.
/// \param point      The evaluated point (array size >= dimension).
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcEvaluate<NCSDCA>::evaluate ( 
  const IndexType &degree, 
  const IndexType &span, 
  const IndexType &dimension,
  const ParamType &u, 
  const ParamType *N,
  const CtrPtType **ctrPts,
  const CtrPtType *weights,
  CtrPtType *point )
{
  SL_ASSERT ( span == degree || span > degree ); // Bypass g++ warning.
  SL_ASSERT ( CADKIT_NURBS_MINIMUM_DEGREE == degree || CADKIT_NURBS_MINIMUM_DEGREE < degree );
  SL_ASSERT ( CADKIT_NURBS_MINIMUM_DIMENSION == dimension || CADKIT_NURBS_MINIMUM_DIMENSION < dimension );
  SL_ASSERT ( 0x0 != N );
  SL_ASSERT ( 0x0 != point );

  // Needed below.
  IndexType i, j, index, order ( degree + 1 );

  // Initialize the point.
  for ( i = 0; i < dimension; ++i ) 
    point[i] = SlConstants<CtrPtType>::zero();

  //
  // Nonrational.
  //

  if ( 0x0 == weights )
  {
    for ( i = 0; i < order; ++i )
    {
      // Do this here.
      index = span - degree + i;

      // Calculate the coordinate for each dimension.
      for ( j = 0; j < dimension; ++j )
      {
        point[j] += N[i] * ctrPts[j][index];
      }
    }

    return;
  }

  //
  // Rational.
  //

  // Initialize.
  CtrPtType weight ( SlConstants<CtrPtType>::zero() );

  for ( i = 0; i < order; ++i )
  {
    // Do this here.
    index = span - degree + i;

    // Calculate the coordinate for each dimension.
    for ( j = 0; j < dimension; ++j )
    {
      point[j] += N[i] * ctrPts[j][index];
    }

    // Calculate the coordinate for the weight.
    weight += N[i] * weights[index];
  }

  // Invert the weight.
  weight = SlConstants<CtrPtType>::one() / weight;

  // Divide out the weight for each dimension.
  for ( j = 0; j < dimension; ++j )
  {
    point[j] *= weight;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Evaluate the point.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcEvaluate<NCSDCA>::evaluate ( 
  const NcCurve<NCSDCA> &curve, 
  const ParamType &u, 
  CtrPtType *point )
{
  SL_ASSERT ( NULL != point );
  SL_ASSERT ( curve.isInRange ( u ) );

  // Get the degree.
  const IndexType degree ( curve.getDegree() );

  // Get the span.
  const IndexType span ( Nurbs::findSpan<NCSDCA> ( curve, u ) );

  // Get the dimension.
  const IndexType dimension ( curve.getDimension() );

  // Get the basis functions.
  ParamType *N = curve.getWork().getBasisFunctions ( 0 );
  NcBasisFunctions<NCSDCA>::basis ( curve, u, span, N );

  // Get the control points.
  const CtrPtType **ctrPts = curve.getCtrPts();

  // Get the weights.
  const CtrPtType *weights = curve.getWeights();

  // Call the function to evaluate a point.
  NcEvaluate<NCSDCA>::evaluate ( degree, span, dimension, u, N, ctrPts, weights, point );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_EVALUATION_CLASS_H_
