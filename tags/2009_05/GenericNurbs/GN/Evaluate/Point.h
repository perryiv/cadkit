
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
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::IndependentArgument IndependentArgument;
  typedef typename SplineType::DependentType DependentType;
  typedef typename SplineType::Vector Vector;
  typedef typename SplineType::WorkSpace WorkSpace;
  typedef typename WorkSpace::value_type WorkSpaceValueType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Evaluate the point on the spline given the parameter. 
  //
  //  surface: The surface being evaluated.
  //  u:       The independent parameter in the u-direction.
  //  v:       The independent parameter in the v-direction.
  //  pt:      The point (the answer).
  //
  /////////////////////////////////////////////////////////////////////////////

  static void surfacePoint ( const SplineType &surface, IndependentArgument u, IndependentArgument v, Vector &pt )
  {
    GN_ERROR_CHECK ( u >= surface.firstKnot ( 0 ) );
    GN_ERROR_CHECK ( u <= surface.lastKnot  ( 0 ) );
    GN_ERROR_CHECK ( v >= surface.firstKnot ( 1 ) );
    GN_ERROR_CHECK ( v <= surface.lastKnot  ( 1 ) );
    GN_ERROR_CHECK ( 2 == surface.numIndepVars() );

    // Find the knot span.
    const SizeType spanU ( GN::Algorithms::findKnotSpan ( surface, 0, u ) );
    const SizeType spanV ( GN::Algorithms::findKnotSpan ( surface, 1, v ) );

    // Get the order and degree.
    const SizeType orderU ( surface.order ( 0 ) );
    const SizeType orderV ( surface.order ( 1 ) );
    const SizeType degreeU ( surface.degree ( 0 ) );
    const SizeType degreeV ( surface.degree ( 1 ) );

    // Number of control points.
    const SizeType numCtrPtsU ( surface.numControlPoints ( 0 ) );
    const SizeType numCtrPtsV ( surface.numControlPoints ( 1 ) );

    // Calculate the blending coefficients.
    WorkSpace &Nu = surface.work ( 0 ).basis;
    WorkSpace &Nv = surface.work ( 1 ).basis;
    Nu.accommodate ( orderU );
    Nv.accommodate ( orderV );
    GN::Algorithms::basisFunctions ( surface, 0, spanU, u, Nu );
    GN::Algorithms::basisFunctions ( surface, 1, spanV, v, Nv );

    // Initialize the point.
    std::fill ( pt.begin(), pt.end(), static_cast<DependentType> ( 0 ) );

    // Number of dependent variables.
    const SizeType numDepVars ( surface.numDepVars() );

    // The dimension.
    const SizeType dimension ( surface.dimension() );

    // We calculate the minimum of the point size and the real dimenion.
    const SizeType dimensionsToCalculate ( std::min<SizeType> ( pt.size(), dimension ) );

    // Needed in the loop.
    SizeType index ( 0 ), indexU ( 0 ), indexV ( 0 ), i ( 0 ), ii ( 0 ), k ( 0 );

		// If it is rational. See "The NURBS Book", page 134.
		if ( surface.rational() )
    {
      // We need a temporary holder.
      WorkSpace &temp = surface.work ( 0 ).temp;
      temp.accommodate ( orderV );

      // Space for the homogeneous coordinate.
      WorkSpace &pw = surface.work ( 0 ).pw;
      pw.accommodate ( numDepVars );

      // Do it once for each dependent variable.
      for ( i = 0; i < numDepVars; ++i )
      {
        for ( ii = 0; ii < orderV; ++ii )
        {
          // Initialize the temp space.
          temp[ii] = static_cast<WorkSpaceValueType> ( 0 );

          // Do this here.
          indexV = spanV - degreeV + ii;

          // Adjust index for 1D control point array.
          index = indexV * numCtrPtsU + indexU;

          // Blend in the u-direction.
          for ( SizeType k = 0; k < orderU; ++k )
          {
            GN_ERROR_CHECK ( ( index + k ) < surface.totalNumControlPoints() );
            temp[ii] += ( Nu[k] * surface.controlPoint ( i, index + k ) );
          }
        }

        // Initialize the homogeneous point.
        pw[i] = static_cast<WorkSpaceValueType> ( 0 );

        // Blend in the v-direction.
        for ( ii = 0; ii < orderV; ++ii )
        {
          pw[i] += ( Nv[ii] * temp[ii] );
        }
      }

      // Invert the weight. Note: use the real dimension.
      DependentType weight ( static_cast<DependentType> ( 1 ) / pw[dimension] );

      // Divide out the weight for each dimension the caller wants.
      for ( i = 0; i < dimensionsToCalculate; ++i )
      {
        pt[i] = pw[i] * weight;
      }
    }

    // If it is non-rational. See "The NURBS Book", page 103.
    else
    {
      // We need a temporary holder.
      DependentType temp ( static_cast<DependentType> ( 0 ) );

      // Do it once for each dimension.
      for ( i = 0; i < dimension; ++i )
      {
        // Initialize the point.
        pt[i] = static_cast<DependentType> ( 0 );

        for ( ii = 0; ii < orderV; ++ii )
        {
          // Initialize the temp space.
          temp = static_cast<DependentType> ( 0 );

          // Do this here.
          indexV = spanV - degreeV + ii;

          // Adjust index for 1D control point array.
          index = indexV * numCtrPtsU + indexU;

          // Blend in the u-direction.
          for ( k = 0; k < orderU; ++k )
          {
            GN_ERROR_CHECK ( ( index + k ) < surface.totalNumControlPoints() );
            temp += ( Nu[k] * surface.controlPoint ( i, index + k ) );
          }

          // Blend in the v-direction.
          pt[i] += ( Nv[ii] * temp );
        }
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Evaluate the point on the spline given the parameter.
  //
  //  curve: The curve being evaluated.
  //  u:     The parameter we are evaluating the point at.
  //  pt:    The point (the answer).
  //
  /////////////////////////////////////////////////////////////////////////////

  static void curvePoint ( const SplineType &curve, IndependentArgument u, Vector &pt )
  {
    GN_ERROR_CHECK ( u >= curve.firstKnot ( 0 ) );
    GN_ERROR_CHECK ( u <= curve.lastKnot  ( 0 )  );
    GN_ERROR_CHECK ( 1 == curve.numIndepVars() );

    // Needed below.
    SizeType index;
    const SizeType order ( curve.order ( 0 ) );
    const SizeType degree ( order - 1 );

    // Find the knot span.
    const SizeType span ( GN::Algorithms::findKnotSpan ( curve, 0, u ) );

    // Calculate the blending coefficients.
    WorkSpace &N = curve.work ( 0 ).basis;
    N.accommodate ( order );
    GN::Algorithms::basisFunctions ( curve, 0, span, u, N );

    // Initialize the point.
    std::fill ( pt.begin(), pt.end(), static_cast<DependentType> ( 0 ) );

    // We calculate the minimum of the point size and the dimenion.
    const SizeType dimension ( std::min<SizeType> ( pt.size(), curve.dimension() ) );

		// If it is rational...
		if ( curve.rational() )
		{
			 // Initialize.
			 DependentType weight = 0;

			 if ( 2 == order ) // 1st degree curve
			 {
					index = span - (order - 1);
					if ( 2 == dimension ) // 2-D curve
					{
						 pt[0]  += N[0] * curve.controlPoint(0, index);
						 pt[1]  += N[0] * curve.controlPoint(1, index);
						 weight += N[0] * curve.weight(index);
						 pt[0]  += N[1] * curve.controlPoint(0, index + 1);
						 pt[1]  += N[1] * curve.controlPoint(1, index + 1);
						 weight += N[1] * curve.weight(index + 1);
					}
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
						 {
								pt[j]  += N[0] * curve.controlPoint(j, index);
								weight += N[0] * curve.weight(index);
								pt[j]  += N[1] * curve.controlPoint(j, index + 1);
								weight += N[1] * curve.weight(index + 1);
						 }
					}
			 }
			 else if ( 3 == order ) // 2nd degree curve
			 {
					index = span - (order - 1);
					if ( 2 == dimension ) // 2-D curve
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
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
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
			 else if ( 4 == order ) // 3rd degree curve
			 {
					index = span - (order - 1);
					if ( 2 == dimension ) // 2-D curve
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
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
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
			 else if ( 5 == order ) // 4th degree curve
			 {
					index = span - (order - 1);
					if ( 2 == dimension ) // 2-D curve
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
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
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
					for ( SizeType i = 0; i < order; ++i )
					{
						 // Do this here.
						 index = span - degree + i;
						 
						 // Calculate the coordinate for each dimension.
						 for ( SizeType j = 0; j < dimension; ++j )
								pt[j] += N[i] * curve.controlPoint ( j, index );
						 
						 // Calculate the coordinate for the weight.
						 weight += N[i] * curve.weight(index);
					}
			 }

			 // Invert the weight.
			 weight = 1 / weight;

			 // Divide out the weight for each dimension.
			 for ( SizeType j = 0; j < dimension; ++j )
					pt[j] *= weight;
		}

    // If it is non-rational...
    else
		{
			 if ( 2 == order ) // 1st degree curve
			 {
					index = span - (order - 1);
					if ( 2 == dimension ) // 2-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
					}
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
						 {
								pt[j] += N[0] * curve.controlPoint(j, index);
								pt[j] += N[1] * curve.controlPoint(j, index + 1);
						 }
					}
			 }
			 else if ( 3 == order ) // 2nd degree curve
			 {
					index = span - (order - 1);
					if ( 2 == dimension ) // 2-D curve
					{
						 pt[0] += N[0] * curve.controlPoint(0, index);
						 pt[1] += N[0] * curve.controlPoint(1, index);
						 pt[0] += N[1] * curve.controlPoint(0, index + 1);
						 pt[1] += N[1] * curve.controlPoint(1, index + 1);
						 pt[0] += N[2] * curve.controlPoint(0, index + 2);
						 pt[1] += N[2] * curve.controlPoint(1, index + 2);
					}
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
						 {
								pt[j] += N[0] * curve.controlPoint(j, index);
								pt[j] += N[1] * curve.controlPoint(j, index + 1);
								pt[j] += N[2] * curve.controlPoint(j, index + 2);
						 }
					}
			 }
			 else if ( 4 == order ) // 3rd degree curve
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
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
						 {
								pt[j] += N[0] * curve.controlPoint(j, index);
								pt[j] += N[1] * curve.controlPoint(j, index + 1);
								pt[j] += N[2] * curve.controlPoint(j, index + 2);
								pt[j] += N[3] * curve.controlPoint(j, index + 3);
						 }
					}
			 }
			 else if ( 5 == order ) // 4th degree curve
			 {
					index = span - (order - 1);
					if ( 2 == dimension ) // 2-D curve
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
					else if ( 3 == dimension ) // 3-D curve
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
						 for ( SizeType j = 0; j < dimension; ++j )
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
					for ( SizeType i = 0; i < order; ++i )
					{
						 // Do this here.
						 index = span - degree + i;
						 
						 // Calculate the coordinate for each dimension.
						 for ( SizeType j = 0; j < dimension; ++j )
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
             typename SplineType::IndependentArgument u,
             typename SplineType::Vector &pt )
{
  GN_CAN_BE_CURVE ( SplineType );
  typedef typename SplineType::SplineClass SplineClass;
  Detail::Calculate<SplineClass>::curvePoint ( c, u, pt );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Evaluate the point on the surface given the parameters.
//
//  s:  Must be a surface.
//  u:  The u-direction parameter we are evaluating the point at.
//  v:  The v-direction parameter we are evaluating the point at.
//  pt: The point being evaluated (the answer).
//
/////////////////////////////////////////////////////////////////////////////

template < class SplineType >
void point ( const SplineType &c,
             typename SplineType::IndependentArgument u,
             typename SplineType::IndependentArgument v,
             typename SplineType::Vector &pt )
{
  GN_CAN_BE_SURFACE ( SplineType );
  typedef typename SplineType::SplineClass SplineClass;
  Detail::Calculate<SplineClass>::surfacePoint ( c, u, v, pt );
}


}; // namespace Evaluate
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_EVALUATE_POINT_H_
