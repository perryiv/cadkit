
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

#include "NcAlgorithm.h"
#include "NcDefine.h"

#include "Standard/SlAssert.h"


namespace CadKit
{
template<NCSDTA> class NcEvaluate : public NcAlgorithm<NCSDCA>
{
public:

  /// Evaluate a point on the curve.
  static void                   evaluate ( const IndexType &order, 
                                           const IndexType &span, 
                                           const ParameterType &u, 
                                           const ParameterType *N,
                                           ControlPointType *pt );
};


///////////////////////////////////////////////////////////////////////////////
///
/// Evaluate a point on the curve.
///
/// \param order      The order of the curve.
/// \param span       The span in the knot vector where u falls.
/// \param u          The parameter we are finding the point at.
/// \param N          The basis functions (order of them).
/// \param pt         The evaluated point.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcEvaluate<NCSDCA>::evaluate ( 
  const IndexType &order, 
  const IndexType &order, 
  const IndexType &span, 
  const ParameterType &u, 
  const ParameterType *N,
  ControlPointType *pt )
{
  SL_ASSERT ( span == order - 1 || span > order - 1 ); // Bypass g++ warning.
  SL_ASSERT ( CADKIT_NURBS_MIN_ORDER == order || CADKIT_NURBS_MIN_ORDER < order );

  // Initialize the point.
  for ( IndexType i = 0; i < _s.dimension; ++i ) 
    point[i] = 0.0;

  // Nonrational.
  if ( !_s.rational )
  {
    for ( IndexType i = 0; i < _s.order[0]; ++i )
    {
      // Do this here.
      index = span - _s.degree[0] + i;

      // Calculate the coordinate for each dimension.
      for ( IndexType j = 0; j < _s.dimension; ++j )
      {
        point[j] += N[i] * _s.ctrPts[j][index];
      }
    }

    return;
  }

  // Rational.
  else
  {
    // Initialize.
    SlFloat64 weight = 0.0;

    for ( IndexType i = 0; i < _s.order[0]; ++i )
    {
      // Do this here.
      index = span - _s.degree[0] + i;

      // Calculate the coordinate for each dimension.
      for ( IndexType j = 0; j < _s.dimension; ++j )
      {
        point[j] += N[i] * _s.ctrPts[j][index];
      }

      // Calculate the coordinate for the weight.
      weight += N[i] * _s.weights[index];
    }

    // Invert the weight.
    weight = 1.0 / weight;

    // Divide out the weight for each dimension.
    for ( IndexType j = 0; j < _s.dimension; ++j )
    {
      point[j] *= weight;
    }

    return;
  }
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_EVALUATION_CLASS_H_
