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
  class SplineType, 
  class BlendingCoefficients
>
struct BasisFunctions
{
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineType::IndependentSequence IndependentSequence;
  typedef typename SplineType::IndependentArgument IndependentArgument;
  typedef typename SplineType::DependentType DependentType;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::WorkSpace WorkSpace;


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

  static void calculate ( const SplineType &spline,
                          SizeType whichIndepVar,
                          SizeType span,
                          IndependentArgument u,
                          BlendingCoefficients &N )
  {
    const IndependentSequence &knots = spline.knotVector ( whichIndepVar );
    SizeType order ( spline.order ( whichIndepVar ) );
    GN_ERROR_CHECK ( knots.size() == spline.numControlPoints ( whichIndepVar ) + order );
    GN_ERROR_CHECK ( N.size() >= order );

    SizeType rPlus1, jMinusR, r;
    DependentType saved, temp;

    WorkSpace &left  = spline.work().left;
    WorkSpace &right = spline.work().right;

    left.accommodate  ( order );
    right.accommodate ( order );

    N[0] = 1;

		if ( order == 2 ) // 1st degree basis functions
		{
			 left[1]  = u - knots[span+1-1];
			 right[1] = knots[span+1] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 1 - 0;
			 temp    = N[0] / ( right[1] + left[1] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[1] * temp;

			 N[1] = saved;
		}
		else if ( order == 3 ) // 2nd degree basis functions
		{
			 left[1]  = u - knots[span+1-1];
			 right[1] = knots[span+1] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 1 - 0;
			 temp    = N[0] / ( right[1] + left[1] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[1] * temp;

			 N[1] = saved;

			 left[2]  = u - knots[span+1-2];
			 right[2] = knots[span+2] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 2 - 0;
			 temp    = N[0] / ( right[1] + left[2] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[2] * temp;

			 //rPlus1  = 1 + 1;
			 //jMinusR = 2 - 1;
			 temp    = N[1] / ( right[2] + left[1] );
			 N[1]    = right[2] * temp + saved;
			 saved   = left[1] * temp;

			 N[2] = saved;
		}
		else if ( order == 4 ) // 3rd degree basis functions
		{
			 left[1]  = u - knots[span+1-1];
			 right[1] = knots[span+1] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 1 - 0;
			 temp    = N[0] / ( right[1] + left[1] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[1] * temp;

			 N[1] = saved;

			 left[2]  = u - knots[span+1-2];
			 right[2] = knots[span+2] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 2 - 0;
			 temp    = N[0] / ( right[1] + left[2] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[2] * temp;

			 //rPlus1  = 1 + 1;
			 //jMinusR = 2 - 1;
			 temp    = N[1] / ( right[2] + left[1] );
			 N[1]    = right[2] * temp + saved;
			 saved   = left[1] * temp;

			 N[2] = saved;

			 left[3]  = u - knots[span+1-3];
			 right[3] = knots[span+3] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 3 - 0;
			 temp    = N[0] / ( right[1] + left[3] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[3] * temp;

			 //rPlus1  = 1 + 1;
			 //jMinusR = 3 - 1;
			 temp    = N[1] / ( right[2] + left[2] );
			 N[1]    = right[2] * temp + saved;
			 saved   = left[2] * temp;

			 //rPlus1  = 2 + 1;
			 //jMinusR = 3 - 2;
			 temp    = N[2] / ( right[3] + left[1] );
			 N[2]    = right[3] * temp + saved;
			 saved   = left[1] * temp;

			 N[3] = saved;
		}
		else if ( order == 5 ) // 4th degree basis functions
		{
			 left[1]  = u - knots[span+1-1];
			 right[1] = knots[span+1] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 1 - 0;
			 temp    = N[0] / ( right[1] + left[1] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[1] * temp;

			 N[1] = saved;

			 left[2]  = u - knots[span+1-2];
			 right[2] = knots[span+2] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 2 - 0;
			 temp    = N[0] / ( right[1] + left[2] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[2] * temp;

			 //rPlus1  = 1 + 1;
			 //jMinusR = 2 - 1;
			 temp    = N[1] / ( right[2] + left[1] );
			 N[1]    = right[2] * temp + saved;
			 saved   = left[1] * temp;

			 N[2] = saved;

			 left[3]  = u - knots[span+1-3];
			 right[3] = knots[span+3] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 3 - 0;
			 temp    = N[0] / ( right[1] + left[3] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[3] * temp;

			 //rPlus1  = 1 + 1;
			 //jMinusR = 3 - 1;
			 temp    = N[1] / ( right[2] + left[2] );
			 N[1]    = right[2] * temp + saved;
			 saved   = left[2] * temp;

			 //rPlus1  = 2 + 1;
			 //jMinusR = 3 - 2;
			 temp    = N[2] / ( right[3] + left[1] );
			 N[2]    = right[3] * temp + saved;
			 saved   = left[1] * temp;

			 N[3] = saved;
		
			 left[4]  = u - knots[span+1-4];
			 right[4] = knots[span+4] - u;
			 saved    = 0;

			 //rPlus1  = 0 + 1;
			 //jMinusR = 4 - 0;
			 temp    = N[0] / ( right[1] + left[4] );
			 N[0]    = right[1] * temp + saved;
			 saved   = left[4] * temp;

			 //rPlus1  = 1 + 1;
			 //jMinusR = 4 - 1;
			 temp    = N[1] / ( right[2] + left[3] );
			 N[1]    = right[2] * temp + saved;
			 saved   = left[3] * temp;

			 //rPlus1  = 2 + 1;
			 //jMinusR = 4 - 2;
			 temp    = N[2] / ( right[3] + left[2] );
			 N[2]    = right[3] * temp + saved;
			 saved   = left[2] * temp;

			 //rPlus1  = 3 + 1;
			 //jMinusR = 4 - 3;
			 temp    = N[3] / ( right[4] + left[1] );
			 N[3]    = right[4] * temp + saved;
			 saved   = left[1] * temp;

			 N[4] = saved;
		}
		else
		{
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

template < class SplineType, class BlendingCoefficients >
void basisFunctions ( const SplineType &spline,
                      typename SplineType::SizeType whichIndepVar,
                      typename SplineType::SizeType span,
                      typename SplineType::IndependentArgument u,
                      BlendingCoefficients &N )
{
   typedef typename SplineType::SplineClass SC;
   typedef BlendingCoefficients BC;
   Detail::BasisFunctions<SC,BC>::calculate ( spline, whichIndepVar, span, u, N );
}


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_BASIS_FUNCTIONS_ALGORITHM_H_
