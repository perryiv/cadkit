///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  B-spline basis function algorithm.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_BASIS_FUNCTIONS_ALGORITHM_H_
#define _GENERIC_NURBS_LIBRARY_BASIS_FUNCTIONS_ALGORITHM_H_


#include "GN/Macros/ErrorCheck.h"
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
//  Helper struct for calculating the basis functions.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class IndependentSequence_,
  class SizeType_,
  class BlendingCoefficients_,
  class WorkSpace_,
  class ErrorCheckerType_
>
struct BasisFunctions
{
  typedef IndependentSequence_                                      IndependentSequence;
  typedef typename IndependentSequence::value_type                  IndependentType;
  typedef typename GN::Traits::Argument<IndependentType>::Type      IndependentArgument;
  typedef typename SizeType_                                        SizeType;
  typedef BlendingCoefficients_                                     BlendingCoefficients;
  typedef typename BlendingCoefficients::value_type                 BlendingCoefficient;
  typedef WorkSpace_                                                WorkSpace;
  typedef ErrorCheckerType_                                         ErrorCheckerType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Compute the non-vanishing basis functions.
  //
  //  spline:         The spline.
  //  whichIndepVar:  The independent variable (which knot vector).
  //  span:           The knot span corresponding to the given parameter.
  //  u:              The parameter we are finding the basis functions for.
  //  N:              The basis functions.
  //  left:           Work space.
  //  right:          Work space.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void calculate ( const IndependentSequence &knots,
                          SizeType order,
                          SizeType span,
                          IndependentArgument u,
                          BlendingCoefficients &N,
                          WorkSpace &left,
                          WorkSpace &right )
  {
    GN_ERROR_CHECK ( span < knots.size() - order );

    // Needed below.
    BlendingCoefficient saved, temp;

    // Make sure these are big enough.
    N.accommodate  ( order );
    left.accommodate  ( order );
    right.accommodate ( order );

    // Initialize.
    N[0] = 1;

    // Unrolled loops below.
		switch ( order )
		{
      case 2:

        left[1]  = u - knots[span];
        right[1] = knots[span+1] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[1] );
        N[0]    = right[1] * temp + saved;
        saved   = left[1] * temp;

        N[1] = saved;
        break;

      case 3:
        
        left[1]  = u - knots[span];
        right[1] = knots[span+1] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[1] );
        N[0]    = right[1] * temp + saved;
        saved   = left[1] * temp;

        N[1] = saved;

        left[2]  = u - knots[span-1];
        right[2] = knots[span+2] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[2] );
        N[0]    = right[1] * temp + saved;
        saved   = left[2] * temp;

        temp    = N[1] / ( right[2] + left[1] );
        N[1]    = right[2] * temp + saved;
        saved   = left[1] * temp;

        N[2] = saved;
        break;

      case 4:

        left[1]  = u - knots[span];
        right[1] = knots[span+1] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[1] );
        N[0]    = right[1] * temp + saved;
        saved   = left[1] * temp;

        N[1] = saved;

        left[2]  = u - knots[span-1];
        right[2] = knots[span+2] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[2] );
        N[0]    = right[1] * temp + saved;
        saved   = left[2] * temp;

        temp    = N[1] / ( right[2] + left[1] );
        N[1]    = right[2] * temp + saved;
        saved   = left[1] * temp;

        N[2] = saved;

        left[3]  = u - knots[span-2];
        right[3] = knots[span+3] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[3] );
        N[0]    = right[1] * temp + saved;
        saved   = left[3] * temp;

        temp    = N[1] / ( right[2] + left[2] );
        N[1]    = right[2] * temp + saved;
        saved   = left[2] * temp;

        temp    = N[2] / ( right[3] + left[1] );
        N[2]    = right[3] * temp + saved;
        saved   = left[1] * temp;

        N[3] = saved;
        break;

      case 5:

        left[1]  = u - knots[span];
        right[1] = knots[span+1] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[1] );
        N[0]    = right[1] * temp + saved;
        saved   = left[1] * temp;

        N[1] = saved;

        left[2]  = u - knots[span-1];
        right[2] = knots[span+2] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[2] );
        N[0]    = right[1] * temp + saved;
        saved   = left[2] * temp;

        temp    = N[1] / ( right[2] + left[1] );
        N[1]    = right[2] * temp + saved;
        saved   = left[1] * temp;

        N[2] = saved;

        left[3]  = u - knots[span-2];
        right[3] = knots[span+3] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[3] );
        N[0]    = right[1] * temp + saved;
        saved   = left[3] * temp;

        temp    = N[1] / ( right[2] + left[2] );
        N[1]    = right[2] * temp + saved;
        saved   = left[2] * temp;

        temp    = N[2] / ( right[3] + left[1] );
        N[2]    = right[3] * temp + saved;
        saved   = left[1] * temp;

        N[3] = saved;

        left[4]  = u - knots[span-3];
        right[4] = knots[span+4] - u;
        saved    = 0;

        temp    = N[0] / ( right[1] + left[4] );
        N[0]    = right[1] * temp + saved;
        saved   = left[4] * temp;

        temp    = N[1] / ( right[2] + left[3] );
        N[1]    = right[2] * temp + saved;
        saved   = left[3] * temp;

        temp    = N[2] / ( right[3] + left[2] );
        N[2]    = right[3] * temp + saved;
        saved   = left[2] * temp;

        temp    = N[3] / ( right[4] + left[1] );
        N[3]    = right[4] * temp + saved;
        saved   = left[1] * temp;

        N[4] = saved;
        break;

      default:
      {
        SizeType rPlus1, jMinusR, r;
        for ( SizeType j = 1; j < order; ++j )
        {
          left[j]  = u - knots[span+1-j];
          right[j] = knots[span+j] - u;
          saved       = 0;

          for ( r = 0; r < j; ++r )
          {
            rPlus1  = r + 1;
            jMinusR = j - r;
            temp    = N[r] / ( right[rPlus1] + left[jMinusR] );
            N[r]    = right[rPlus1] * temp + saved;
            saved   = left[jMinusR] * temp;
          }

          N[j] = saved;
        }
      }
		}
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

};


/////////////////////////////////////////////////////////////////////////////
//
//  Compute the non-vanishing basis functions.
//
//  spline:         The spline.
//  whichIndepVar:  The independent variable (which knot vector).
//  span:           The knot span corresponding to the given parameter.
//  u:              The parameter we are finding the basis functions for.
//  N:              The basis functions.
//
/////////////////////////////////////////////////////////////////////////////

template
<
  class SplineType, 
  class BlendingCoefficients
>
void basisFunctions ( const SplineType &spline,
                      typename SplineType::SizeType whichIndepVar,
                      typename SplineType::SizeType span,
                      typename SplineType::IndependentArgument u,
                      typename BlendingCoefficients &N )
{
  // Declare types.
  typedef typename SplineType::IndependentSequence IndependentSequence;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineType::WorkSpace WorkSpace;
  typedef Detail::BasisFunctions<IndependentSequence,SizeType,BlendingCoefficients,WorkSpace,ErrorCheckerType> BasisFunctions;

  // Get the data we need.
  const IndependentSequence &knots = spline.knotVector ( whichIndepVar );
  SizeType order ( spline.order ( whichIndepVar ) );
  WorkSpace &left = spline.work().left;
  WorkSpace &right = spline.work().right;

  // Call helper function.
  BasisFunctions::calculate ( knots, order, span, u, N, left, right );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Compute the non-vanishing basis functions.
//
//  spline:         The spline.
//  whichIndepVar:  The independent variable (which knot vector).
//  span:           The knot span corresponding to the given parameter.
//  u:              The parameter we are finding the basis functions for.
//  N:              The basis functions.
//
/////////////////////////////////////////////////////////////////////////////

template
<
  class CurveType, 
  class BlendingCoefficients
>
void basisFunctions ( const CurveType &curve,
                      typename CurveType::SizeType span,
                      typename CurveType::IndependentArgument u,
                      typename BlendingCoefficients &N )
{
  // Has to be a curve.
  GN_IS_CURVE ( CurveType );

  // Delegate.
  BasisFunctions::calculate ( curve.spline(), 0, span, u, N );
}


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_BASIS_FUNCTIONS_ALGORITHM_H_
