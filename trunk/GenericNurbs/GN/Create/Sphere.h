
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms for spheres.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_SPHERE_ALGORITHMS_H_
#define _GENERIC_NURBS_LIBRARY_SPHERE_ALGORITHMS_H_

#include "GN/Transform/Translate.h"
#include "GN/Transform/Scale.h"
#include "GN/MPL/TypeCheck.h"


namespace GN {
namespace Create {


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct Sphere
{
  typedef typename SplineType::SplineClass SplineClass;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::SizeContainer SizeContainer;
  typedef typename SplineType::DependentType DependentType;

  static void make ( SplineClass &s, SizeType dimension = 3 )
  {
    GN_CAN_BE_SURFACE ( SplineType );

    // The specs for a sphere.
    const SizeType numIndepVars ( 2 );
    SizeContainer numCtrPts;
    numCtrPts.resize ( numIndepVars );
    numCtrPts[0] = 9;
    numCtrPts[1] = 5;
    SizeContainer order ;
    order.resize ( numIndepVars );
    order[0] = 3;
    order[1] = 3;
    const bool rational ( true );

    // Constant value used below.
    const DependentType SQRT_2_OVER_2 ( 0.70710678118654752440084436210485f );

    // Resize the spline.
    s.resize ( dimension, order, numCtrPts, rational );

    // Fill in the u-direction knot vector.
    s.knot(0, 0) = 0;
    s.knot(0, 1) = 0;
    s.knot(0, 2) = 0;
    s.knot(0, 3) = 0.25f;
    s.knot(0, 4) = 0.25f;
    s.knot(0, 5) = 0.50f;
    s.knot(0, 6) = 0.50f;
    s.knot(0, 7) = 0.75f;
    s.knot(0, 8) = 0.75f;
    s.knot(0, 9) = 1;
    s.knot(0,10) = 1;
    s.knot(0,11) = 1;

    // Fill in the v-direction knot vector.
    s.knot(1,0) = 0;
    s.knot(1,1) = 0;
    s.knot(1,2) = 0;
    s.knot(1,3) = 0.5f;
    s.knot(1,4) = 0.5f;
    s.knot(1,5) = 1;
    s.knot(1,6) = 1;
    s.knot(1,7) = 1;

    // Fill in the x control points.
    s.controlPoint(0, 0) = -1;
    s.controlPoint(0, 1) = -SQRT_2_OVER_2;
    s.controlPoint(0, 2) = -1;
    s.controlPoint(0, 3) = -SQRT_2_OVER_2;
    s.controlPoint(0, 4) = -1;
    s.controlPoint(0, 5) = -SQRT_2_OVER_2;
    s.controlPoint(0, 6) = -1;
    s.controlPoint(0, 7) = -SQRT_2_OVER_2;
    s.controlPoint(0, 8) = -1;
    s.controlPoint(0, 9) = -SQRT_2_OVER_2;
    s.controlPoint(0,10) = -0.5f;
    s.controlPoint(0,11) = -SQRT_2_OVER_2;
    s.controlPoint(0,12) = -0.5f;
    s.controlPoint(0,13) = -SQRT_2_OVER_2;
    s.controlPoint(0,14) = -0.5f;
    s.controlPoint(0,15) = -SQRT_2_OVER_2;
    s.controlPoint(0,16) = -0.5f;
    s.controlPoint(0,17) = -SQRT_2_OVER_2;
    s.controlPoint(0,18) =  0;
    s.controlPoint(0,19) =  0;
    s.controlPoint(0,20) =  0;
    s.controlPoint(0,21) =  0;
    s.controlPoint(0,22) =  0;
    s.controlPoint(0,23) =  0;
    s.controlPoint(0,24) =  0;
    s.controlPoint(0,25) =  0;
    s.controlPoint(0,26) =  0;
    s.controlPoint(0,27) =  SQRT_2_OVER_2;
    s.controlPoint(0,28) =  0.5f;
    s.controlPoint(0,29) =  SQRT_2_OVER_2;
    s.controlPoint(0,30) =  0.5f;
    s.controlPoint(0,31) =  SQRT_2_OVER_2;
    s.controlPoint(0,32) =  0.5f;
    s.controlPoint(0,33) =  SQRT_2_OVER_2;
    s.controlPoint(0,34) =  0.5f;
    s.controlPoint(0,35) =  SQRT_2_OVER_2;
    s.controlPoint(0,36) =  1;
    s.controlPoint(0,37) =  SQRT_2_OVER_2;
    s.controlPoint(0,38) =  1;
    s.controlPoint(0,39) =  SQRT_2_OVER_2;
    s.controlPoint(0,40) =  1;
    s.controlPoint(0,41) =  SQRT_2_OVER_2;
    s.controlPoint(0,42) =  1;
    s.controlPoint(0,43) =  SQRT_2_OVER_2;
    s.controlPoint(0,44) =  1;

    // Fill in the y control points.
    s.controlPoint(1, 0) =  0;
    s.controlPoint(1, 1) =  0;
    s.controlPoint(1, 2) =  0;
    s.controlPoint(1, 3) =  0;
    s.controlPoint(1, 4) =  0;
    s.controlPoint(1, 5) =  0;
    s.controlPoint(1, 6) =  0;
    s.controlPoint(1, 7) =  0;
    s.controlPoint(1, 8) =  0;
    s.controlPoint(1, 9) =  SQRT_2_OVER_2;
    s.controlPoint(1,10) =  0.5f;
    s.controlPoint(1,11) =  0;
    s.controlPoint(1,12) = -0.5f;
    s.controlPoint(1,13) = -SQRT_2_OVER_2;
    s.controlPoint(1,14) = -0.5f;
    s.controlPoint(1,15) =  0;
    s.controlPoint(1,16) =  0.5f;
    s.controlPoint(1,17) =  SQRT_2_OVER_2;
    s.controlPoint(1,18) =  1;
    s.controlPoint(1,19) =  SQRT_2_OVER_2;
    s.controlPoint(1,20) =  0;
    s.controlPoint(1,21) = -SQRT_2_OVER_2;
    s.controlPoint(1,22) = -1;
    s.controlPoint(1,23) = -SQRT_2_OVER_2;
    s.controlPoint(1,24) =  0;
    s.controlPoint(1,25) =  SQRT_2_OVER_2;
    s.controlPoint(1,26) =  1;
    s.controlPoint(1,27) =  SQRT_2_OVER_2;
    s.controlPoint(1,28) =  0.5f;
    s.controlPoint(1,29) =  0;
    s.controlPoint(1,30) = -0.5f;
    s.controlPoint(1,31) = -SQRT_2_OVER_2;
    s.controlPoint(1,32) = -0.5f;
    s.controlPoint(1,33) =  0;
    s.controlPoint(1,34) =  0.5f;
    s.controlPoint(1,35) =  SQRT_2_OVER_2;
    s.controlPoint(1,36) =  0;
    s.controlPoint(1,37) =  0;
    s.controlPoint(1,38) =  0;
    s.controlPoint(1,39) =  0;
    s.controlPoint(1,40) =  0;
    s.controlPoint(1,41) =  0;
    s.controlPoint(1,42) =  0;
    s.controlPoint(1,43) =  0;
    s.controlPoint(1,44) =  0;

    // Fill in the z control points.
    s.controlPoint(2, 0) =  0;
    s.controlPoint(2, 1) =  0;
    s.controlPoint(2, 2) =  0;
    s.controlPoint(2, 3) =  0;
    s.controlPoint(2, 4) =  0;
    s.controlPoint(2, 5) =  0;
    s.controlPoint(2, 6) =  0;
    s.controlPoint(2, 7) =  0;
    s.controlPoint(2, 8) =  0;
    s.controlPoint(2, 9) =  0;
    s.controlPoint(2,10) =  0.5f;
    s.controlPoint(2,11) =  SQRT_2_OVER_2;
    s.controlPoint(2,12) =  0.5f;
    s.controlPoint(2,13) =  0;
    s.controlPoint(2,14) = -0.5f;
    s.controlPoint(2,15) = -SQRT_2_OVER_2;
    s.controlPoint(2,16) = -0.5f;
    s.controlPoint(2,17) =  0;
    s.controlPoint(2,18) =  0;
    s.controlPoint(2,19) =  SQRT_2_OVER_2;
    s.controlPoint(2,20) =  1;
    s.controlPoint(2,21) =  SQRT_2_OVER_2;
    s.controlPoint(2,22) =  0;
    s.controlPoint(2,23) = -SQRT_2_OVER_2;
    s.controlPoint(2,24) = -1;
    s.controlPoint(2,25) = -SQRT_2_OVER_2;
    s.controlPoint(2,26) =  0;
    s.controlPoint(2,27) =  0;
    s.controlPoint(2,28) =  0.5f;
    s.controlPoint(2,29) =  SQRT_2_OVER_2;
    s.controlPoint(2,30) =  0.5f;
    s.controlPoint(2,31) =  0;
    s.controlPoint(2,32) = -0.5f;
    s.controlPoint(2,33) = -SQRT_2_OVER_2;
    s.controlPoint(2,34) = -0.5f;
    s.controlPoint(2,35) =  0;
    s.controlPoint(2,36) =  0;
    s.controlPoint(2,37) =  0;
    s.controlPoint(2,38) =  0;
    s.controlPoint(2,39) =  0;
    s.controlPoint(2,40) =  0;
    s.controlPoint(2,41) =  0;
    s.controlPoint(2,42) =  0;
    s.controlPoint(2,43) =  0;
    s.controlPoint(2,44) =  0;

    // Fill in all other dimensions.
    if ( dimension > 3 )
    {
      SizeType totalCtrPts ( s.totalNumControlPoints() );
      for ( SizeType i = 3; i < dimension; ++i )
      {
        for ( SizeType j = 0; j < totalCtrPts; ++j )
        {
          s.controlPoint(i,j) =  0;
        }
      }
    }

    // Fill in the weights.
    s.weight( 0) =  1;
    s.weight( 1) =  SQRT_2_OVER_2;
    s.weight( 2) =  1;
    s.weight( 3) =  SQRT_2_OVER_2;
    s.weight( 4) =  1;
    s.weight( 5) =  SQRT_2_OVER_2;
    s.weight( 6) =  1;
    s.weight( 7) =  SQRT_2_OVER_2;
    s.weight( 8) =  1;
    s.weight( 9) =  SQRT_2_OVER_2;
    s.weight(10) =  0.5f;
    s.weight(11) =  SQRT_2_OVER_2;
    s.weight(12) =  0.5f;
    s.weight(13) =  SQRT_2_OVER_2;
    s.weight(14) =  0.5f;
    s.weight(15) =  SQRT_2_OVER_2;
    s.weight(16) =  0.5f;
    s.weight(17) =  SQRT_2_OVER_2;
    s.weight(18) =  1;
    s.weight(19) =  SQRT_2_OVER_2;
    s.weight(20) =  1;
    s.weight(21) =  SQRT_2_OVER_2;
    s.weight(22) =  1;
    s.weight(23) =  SQRT_2_OVER_2;
    s.weight(24) =  1;
    s.weight(25) =  SQRT_2_OVER_2;
    s.weight(26) =  1;
    s.weight(27) =  SQRT_2_OVER_2;
    s.weight(28) =  0.5f;
    s.weight(29) =  SQRT_2_OVER_2;
    s.weight(30) =  0.5f;
    s.weight(31) =  SQRT_2_OVER_2;
    s.weight(32) =  0.5f;
    s.weight(33) =  SQRT_2_OVER_2;
    s.weight(34) =  0.5f;
    s.weight(35) =  SQRT_2_OVER_2;
    s.weight(36) =  1;
    s.weight(37) =  SQRT_2_OVER_2;
    s.weight(38) =  1;
    s.weight(39) =  SQRT_2_OVER_2;
    s.weight(40) =  1;
    s.weight(41) =  SQRT_2_OVER_2;
    s.weight(42) =  1;
    s.weight(43) =  SQRT_2_OVER_2;
    s.weight(44) =  1;
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
//  Make a sphere.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void sphere ( 
  SplineType &s, 
  typename SplineType::SizeType dimension = 3 )
{
  GN_CAN_BE_SURFACE ( SplineType );
  GN::Create::Detail::Sphere<SplineType>::make ( s, dimension );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void sphere ( 
  SplineType &s, 
  const typename SplineType::Vec3 &center,
  typename SplineType::DependentType radius )
{
  GN_CAN_BE_SURFACE ( SplineType );
  typedef typename SplineType::Matrix44 Matrix44;
  typedef typename SplineType::Vec3 Vec3;

  // Make a 3D unit sphere at the origin.
  GN::Create::sphere ( s, 3 );

  // Scale it by the radius.
  GN::Transform::scale ( s, Vec3 ( radius, radius, 1 ) );

  // Translate it to the center.
  GN::Transform::translate ( s, center );
}


}; // namespace Create
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_SPHERE_ALGORITHMS_H_
