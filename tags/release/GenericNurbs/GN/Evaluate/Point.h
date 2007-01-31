
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV and Andrew Mckean 
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NURBS point evaluator.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_EVALUATE_POINT_H_
#define _GENERIC_NURBS_LIBRARY_EVALUATE_POINT_H_

#include "GN/Macros/ErrorCheck.h"
#include "GN/MPL/TypeCheck.h"
#include "GN/Algorithms/FindSpan.h"
#include "GN/Algorithms/BasisFunctions.h"

#include <algorithm>


namespace GN {
namespace Evaluate {


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for evaluating the curve point.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct Calculate
{
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineType::UIntType UIntType;
  typedef typename SplineType::KnotArgument KnotArgument;
  typedef typename SplineType::ControlPointType ControlPointType;
  typedef typename SplineType::Vector Vector;
  typedef typename SplineType::WorkSpace WorkSpace;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Evaluate the point on the spline given the parameter.
  //
  //  curve: The curve being evaluated.
  //  u:     The parameter we are evaluating the point at.
  //  pt:    The point (the answer).
  //
  /////////////////////////////////////////////////////////////////////////////

  static void curvePoint ( const SplineType &curve, KnotArgument u, Vector &pt )
  {
    GN_ERROR_CHECK ( u >= curve.firstKnot ( 0 ) );
    GN_ERROR_CHECK ( u <= curve.lastKnot  ( 0 )  );
    GN_ERROR_CHECK ( 1 == curve.numIndepVars() );

    // Needed below.
    UIntType index;
    UIntType order ( curve.order ( 0 ) );
    UIntType degree ( order - 1 );

    // Find the knot span.
    UIntType span ( GN::Algorithms::findKnotSpan ( curve, 0, u ) );

    // Calculate the blending coefficients.
    WorkSpace &N = curve.work().basis;
    N.accommodate ( order );
    GN::Algorithms::basisFunctions ( curve, 0, span, u, N );

    // Initialize the point.
    std::fill ( pt.begin(), pt.end(), static_cast<ControlPointType> ( 0 ) );

    // We calculate the minimum of the point size and the dimenion.
    UIntType dimension ( std::min ( pt.size(), curve.dimension() ) );

		// If it is rational...
		if ( curve.rational() )
		{
			 // Initialize.
			 ControlPointType weight = 0;

			 if ( order == 2 ) // 1st degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 weight += N[1] * curve.weight(index + 1);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 pt[2]  += N[0] * curve.controlPoint(2, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 pt[2]  += N[1] * curve.controlPoint(2, index + 1);
						 weight += N[1] * curve.weight(index + 1);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j]  += N[0] * curve.controlPoint(j, index);
								weight += N[0] * curve.weight(index);
								pt[j]  += N[1] * curve.controlPoint(j, index + 1);
								weight += N[1] * curve.weight(index + 1);
						 }
					}
			 }
			 else if ( order == 3 ) // 2nd degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 weight += N[1] * curve.weight(index + 1);
						 pt[0]  += N[2] * curve.controlPoint(0, index + 2);
						 pt[1]  += N[2] * curve.controlPoint(1, index + 2);
						 weight += N[2] * curve.weight(index + 2);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 pt[2]  += N[0] * curve.controlPoint(2, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 pt[2]  += N[1] * curve.controlPoint(2, index + 1);
						 weight += N[1] * curve.weight(index + 1);
						 pt[0]  += N[2] * curve.controlPoint(0, index + 2);
						 pt[1]  += N[2] * curve.controlPoint(1, index + 2);
						 pt[2]  += N[2] * curve.controlPoint(2, index + 2);
						 weight += N[2] * curve.weight(index + 2);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j]  += N[0] * curve.controlPoint(j, index);
								weight += N[0] * curve.weight(index);
								pt[j]  += N[1] * curve.controlPoint(j, index + 1);
								weight += N[1] * curve.weight(index + 1);
								pt[j]  += N[2] * curve.controlPoint(j, index + 2);
								weight += N[2] * curve.weight(index + 2);
						 }
					}
			 }
			 else if ( order == 4 ) // 3rd degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 weight += N[1] * curve.weight(index + 1);
						 pt[0]  += N[2] * curve.controlPoint(0, index + 2);
						 pt[1]  += N[2] * curve.controlPoint(1, index + 2);
						 weight += N[2] * curve.weight(index + 2);
						 pt[0]  += N[3] * curve.controlPoint(0, index + 3);
						 pt[1]  += N[3] * curve.controlPoint(1, index + 3);
						 weight += N[3] * curve.weight(index + 3);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 pt[2]  += N[0] * curve.controlPoint(2, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 pt[2]  += N[1] * curve.controlPoint(2, index + 1);
						 weight += N[1] * curve.weight(index + 1);
						 pt[0]  += N[2] * curve.controlPoint(0, index + 2);
						 pt[1]  += N[2] * curve.controlPoint(1, index + 2);
						 pt[2]  += N[2] * curve.controlPoint(2, index + 2);
						 weight += N[2] * curve.weight(index + 2);
						 pt[0]  += N[3] * curve.controlPoint(0, index + 3);
						 pt[1]  += N[3] * curve.controlPoint(1, index + 3);
						 pt[2]  += N[3] * curve.controlPoint(2, index + 3);
						 weight += N[3] * curve.weight(index + 3);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j]  += N[0] * curve.controlPoint(j, index);
								weight += N[0] * curve.weight(index);
								pt[j]  += N[1] * curve.controlPoint(j, index + 1);
								weight += N[1] * curve.weight(index + 1);
								pt[j]  += N[2] * curve.controlPoint(j, index + 2);
								weight += N[2] * curve.weight(index + 2);
								pt[j]  += N[3] * curve.controlPoint(j, index + 3);
								weight += N[3] * curve.weight(index + 3);
						 }
					}
			 }
			 else if ( order == 5 ) // 4th degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 weight += N[1] * curve.weight(index + 1);
						 pt[0]  += N[2] * curve.controlPoint(0, index + 2);
						 pt[1]  += N[2] * curve.controlPoint(1, index + 2);
						 weight += N[2] * curve.weight(index + 2);
						 pt[0]  += N[3] * curve.controlPoint(0, index + 3);
						 pt[1]  += N[3] * curve.controlPoint(1, index + 3);
						 weight += N[3] * curve.weight(index + 3);
						 pt[0]  += N[4] * curve.controlPoint(0, index + 4);
						 pt[1]  += N[4] * curve.controlPoint(1, index + 4);
						 weight += N[4] * curve.weight(index + 4);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 pt[2]  += N[0] * curve.controlPoint(2, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 pt[2]  += N[1] * curve.controlPoint(2, index + 1);
						 weight += N[1] * curve.weight(index + 1);
						 pt[0]  += N[2] * curve.controlPoint(0, index + 2);
						 pt[1]  += N[2] * curve.controlPoint(1, index + 2);
						 pt[2]  += N[2] * curve.controlPoint(2, index + 2);
						 weight += N[2] * curve.weight(index + 2);
						 pt[0]  += N[3] * curve.controlPoint(0, index + 3);
						 pt[1]  += N[3] * curve.controlPoint(1, index + 3);
						 pt[2]  += N[3] * curve.controlPoint(2, index + 3);
						 weight += N[3] * curve.weight(index + 3);
						 pt[0]  += N[4] * curve.controlPoint(0, index + 4);
						 pt[1]  += N[4] * curve.controlPoint(1, index + 4);
						 pt[2]  += N[4] * curve.controlPoint(2, index + 4);
						 weight += N[4] * curve.weight(index + 4);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j]  += N[0] * curve.controlPoint(j, index);
								weight += N[0] * curve.weight(index);
								pt[j]  += N[1] * curve.controlPoint(j, index + 1);
								weight += N[1] * curve.weight(index + 1);
								pt[j]  += N[2] * curve.controlPoint(j, index + 2);
								weight += N[2] * curve.weight(index + 2);
								pt[j]  += N[3] * curve.controlPoint(j, index + 3);
								weight += N[3] * curve.weight(index + 3);
								pt[j]  += N[4] * curve.controlPoint(j, index + 4);
								weight += N[4] * curve.weight(index + 4);
						 }
					}
			 }
			 else // n-degree curve
			 {
					for ( UIntType i = 0; i < order; ++i )
					{
						 // Do this here.
						 index = span - degree + i;
						 
						 // Calculate the coordinate for each dimension.
						 for ( UIntType j = 0; j < dimension; ++j )
								pt[j] += N[i] * curve.controlPoint ( j, index );
						 
						 // Calculate the coordinate for the weight.
						 weight += N[i] * curve.weight(index);
					}
			 }

			 // Invert the weight.
			 weight = 1 / weight;

			 // Divide out the weight for each dimension.
			 for ( UIntType j = 0; j < dimension; ++j )
					pt[j] *= weight;
		}

    // If it is non-rational...
    else
		{
			 if ( order == 2 ) // 1st degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[2] += N[0] * curve.controlPoint(2, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[2] += N[1] * curve.controlPoint(2, index + 1);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j] += N[0] * curve.controlPoint(j, index);
								pt[j] += N[1] * curve.controlPoint(j, index + 1);
						 }
					}
			 }
			 else if ( order == 3 ) // 2nd degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[0] += N[2] * curve.controlPoint(0, index + 2);
						 pt[1] += N[2] * curve.controlPoint(1, index + 2);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[2] += N[0] * curve.controlPoint(2, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[2] += N[1] * curve.controlPoint(2, index + 1);
						 pt[0] += N[2] * curve.controlPoint(0, index + 2);
						 pt[1] += N[2] * curve.controlPoint(1, index + 2);
						 pt[2] += N[2] * curve.controlPoint(2, index + 2);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j] += N[0] * curve.controlPoint(j, index);
								pt[j] += N[1] * curve.controlPoint(j, index + 1);
								pt[j] += N[2] * curve.controlPoint(j, index + 2);
						 }
					}
			 }
			 else if ( order == 4 ) // 3rd degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[0] += N[2] * curve.controlPoint(0, index + 2);
						 pt[1] += N[2] * curve.controlPoint(1, index + 2);
						 pt[0] += N[3] * curve.controlPoint(0, index + 3);
						 pt[1] += N[3] * curve.controlPoint(1, index + 3);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[2] += N[0] * curve.controlPoint(2, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[2] += N[1] * curve.controlPoint(2, index + 1);
						 pt[0] += N[2] * curve.controlPoint(0, index + 2);
						 pt[1] += N[2] * curve.controlPoint(1, index + 2);
						 pt[2] += N[2] * curve.controlPoint(2, index + 2);
						 pt[0] += N[3] * curve.controlPoint(0, index + 3);
						 pt[1] += N[3] * curve.controlPoint(1, index + 3);
						 pt[2] += N[3] * curve.controlPoint(2, index + 3);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j] += N[0] * curve.controlPoint(j, index);
								pt[j] += N[1] * curve.controlPoint(j, index + 1);
								pt[j] += N[2] * curve.controlPoint(j, index + 2);
								pt[j] += N[3] * curve.controlPoint(j, index + 3);
						 }
					}
			 }
			 else if ( order == 5 ) // 4th degree curve
			 {
					index = span - (order - 1);
					if ( dimension == 2 ) // 2-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[0] += N[2] * curve.controlPoint(0, index + 2);
						 pt[1] += N[2] * curve.controlPoint(1, index + 2);
						 pt[0] += N[3] * curve.controlPoint(0, index + 3);
						 pt[1] += N[3] * curve.controlPoint(1, index + 3);
						 pt[0] += N[4] * curve.controlPoint(0, index + 4);
						 pt[1] += N[4] * curve.controlPoint(1, index + 4);
					}
					else if ( dimension == 3 ) // 3-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[2] += N[0] * curve.controlPoint(2, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[2] += N[1] * curve.controlPoint(2, index + 1);
						 pt[0] += N[2] * curve.controlPoint(0, index + 2);
						 pt[1] += N[2] * curve.controlPoint(1, index + 2);
						 pt[2] += N[2] * curve.controlPoint(2, index + 2);
						 pt[0] += N[3] * curve.controlPoint(0, index + 3);
						 pt[1] += N[3] * curve.controlPoint(1, index + 3);
						 pt[2] += N[3] * curve.controlPoint(2, index + 3);
						 pt[0] += N[4] * curve.controlPoint(0, index + 4);
						 pt[1] += N[4] * curve.controlPoint(1, index + 4);
						 pt[2] += N[4] * curve.controlPoint(2, index + 4);
					}
					else // n-D curve
					{
						 for ( UIntType j = 0; j < dimension; ++j )
						 {
								pt[j] += N[0] * curve.controlPoint(j, index);
								pt[j] += N[1] * curve.controlPoint(j, index + 1);
								pt[j] += N[2] * curve.controlPoint(j, index + 2);
								pt[j] += N[3] * curve.controlPoint(j, index + 3);
								pt[j] += N[4] * curve.controlPoint(j, index + 4);
						 }
					}
			 }
			 else // n-degree curve
			 {
					for ( UIntType i = 0; i < order; ++i )
					{
						 // Do this here.
						 index = span - degree + i;
						 
						 // Calculate the coordinate for each dimension.
						 for ( UIntType j = 0; j < dimension; ++j )
								pt[j] += N[i] * curve.controlPoint ( j, index );
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
//  Evaluate the point on the curve given the parameter.
//
//  c:  Must be a curve.
//  u:  The parameter we are evaluating the point at.
//  pt: The point.
//
/////////////////////////////////////////////////////////////////////////////

template < class SplineType >
void point ( const SplineType &c,
             typename SplineType::KnotArgument u,
             typename SplineType::Vector &pt )
{
  GN_CAN_BE_CURVE ( SplineType );
  typedef typename SplineType::SplineClass SplineClass;
  Detail::Calculate<SplineClass>::curvePoint ( c, u, pt );
}


}; // namespace Evaluate
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_EVALUATE_POINT_H_
