
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcBasisFunctions.h: Algorithms for finding the basis functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SPLINE_BASIS_FUNCTIONS_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SPLINE_BASIS_FUNCTIONS_CLASS_H_

#include "NcAlgorithm.h"
#include "NcCurve.h"
#include "NcSurface.h"

#include "Standard/SlAssert.h"


namespace CadKit
{
template<NCSDTA> class NcBasisFunctions : public NcAlgorithm<NCSDCA>
{
public:

  /// Calculate the basis functions.
  static void                   basis ( 
                                  const NcCurve<NCSDCA> &curve,
                                  const ParamType &u, 
                                  const IndexType &span, 
                                  ParamType *N );

  /// Calculate the basis functions.
  static void                   basis ( 
                                  const NcSpline<NCSDCA> &spline,
                                  const IndexType &whichIndepVar, 
                                  const ParamType &u, 
                                  const IndexType &span, 
                                  ParamType *N );

  /// Calculate the basis functions.
  static void                   basis ( 
                                  const ParamType *knots, 
                                  const IndexType &order, 
                                  const IndexType &span, 
                                  const ParamType &u, 
                                  ParamType *left,
                                  ParamType *right,
                                  ParamType *N );
};


///////////////////////////////////////////////////////////////////////////////
///
/// Calculate the basis functions.
///
/// \param knots      The knot vector.
/// \param order      The order in the independent direction associated 
///                   with the given knot vector.
/// \param span       The span in the knot vector where u falls.
/// \param u          The parameter we are finding the basis functions for.
/// \param left       Used internally. Must be size >= order.
/// \param right      Used internally. Must be size >= order.
/// \param N          The basis functions that are calculated. The number of 
///                   basis functions is equal to the order.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcBasisFunctions<NCSDCA>::basis ( 
  const ParamType *knots, 
  const IndexType &order, 
  const IndexType &span, 
  const ParamType &u, 
  ParamType *left,
  ParamType *right,
  ParamType *N )
{
  SL_ASSERT ( NULL != knots );
  SL_ASSERT ( NULL != left );
  SL_ASSERT ( NULL != right );
  SL_ASSERT ( NULL != N );
  SL_ASSERT ( order > 1 );
  SL_ASSERT ( span == order - 1 || span > order - 1 ); // Bypass g++ warning.

  IndexType rPlus1, jMinusR, j, r;
  ParamType saved, temp;

  N[0] = 1.0f;

  for ( j = 1; j < order; ++j ) 
  {
    left[j] = u - knots[span+1-j];
    right[j] = knots[span+j] - u;
    saved = 0.0f;

    for ( r = 0; r < j; ++r )
    {
      rPlus1 = r + 1;
      jMinusR = j - r;
      temp = N[r] / ( right[rPlus1] + left[jMinusR] );
      N[r] = right[rPlus1] * temp + saved;
      saved = left[jMinusR] * temp;
    }

    N[j] = saved;
  }
}


///////////////////////////////////////////////////////////////////////////////
///
/// Calculate the basis functions.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcBasisFunctions<NCSDCA>::basis ( 
  const NcSpline<NCSDCA> &spline,
  const IndexType &whichIndepVar, 
  const ParamType &u, 
  const IndexType &span, 
  ParamType *N )
{
  SL_ASSERT ( true == spline.isInRange ( whichIndepVar, u ) );
  SL_ASSERT ( span >= spline.getDegree ( whichIndepVar ) );
  SL_ASSERT ( span <= spline.getNumCtrPts ( whichIndepVar ) );

  // Get the start of the knot vector.
  const ParamType *knots = spline.getKnots ( whichIndepVar );

  // Get the order.
  const IndexType order ( spline.getOrder ( whichIndepVar ) );

  // Space for the algorithm.
  ParamType *left  = spline.getWork().getBasisFunctionsLeft  ( whichIndepVar );
  ParamType *right = spline.getWork().getBasisFunctionsRight ( whichIndepVar );

  // Call the algorithm.
  NcBasisFunctions<NCSDCA>::basis ( knots, order, span, u, left, right, N );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Calculate the basis functions.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcBasisFunctions<NCSDCA>::basis ( 
  const NcCurve<NCSDCA> &curve,
  const ParamType &u, 
  const IndexType &span, 
  ParamType *N )
{
  // Call the other one.
  NcBasisFunctions<NCSDCA>::basis ( curve, 0, u, span, N );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_BASIS_FUNCTIONS_CLASS_H_
