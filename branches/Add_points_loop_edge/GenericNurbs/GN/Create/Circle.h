
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms for circles.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_CIRCLE_ALGORITHMS_H_
#define _GENERIC_NURBS_LIBRARY_CIRCLE_ALGORITHMS_H_

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
//  Make a circle.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct Circle
{
  typedef typename SplineType::SplineClass SplineClass;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::SizeContainer SizeContainer;

  static void make ( SplineClass &s, SizeType dimension = 3 )
  {
    GN_CAN_BE_CURVE ( SplineType );

    // The specs for a circle.
    const SizeType numIndepVars ( 1 );
    SizeContainer numCtrPts, order;
    numCtrPts.resize ( numIndepVars );
    order.resize     ( numIndepVars );
    numCtrPts[0]     = 9;
    order[0]         = 3;
    const bool rational ( true );

    // Resize the spline.
    s.resize ( dimension, order, numCtrPts, rational );

    // Fill in the knot vector.
    s.knot(0, 0) =  0;
    s.knot(0, 1) =  0;
    s.knot(0, 2) =  0;
    s.knot(0, 3) =  0.25f;
    s.knot(0, 4) =  0.25f;
    s.knot(0, 5) =  0.50f;
    s.knot(0, 6) =  0.50f;
    s.knot(0, 7) =  0.75f;
    s.knot(0, 8) =  0.75f;
    s.knot(0, 9) =  1;
    s.knot(0,10) = 1;
    s.knot(0,11) = 1;

    // Fill in the x control points.
    s.controlPoint(0,0) =  1;
    s.controlPoint(0,1) =  0.707107f;
    s.controlPoint(0,2) =  0;
    s.controlPoint(0,3) = -0.707107f;
    s.controlPoint(0,4) = -1;
    s.controlPoint(0,5) = -0.707107f;
    s.controlPoint(0,6) =  0;
    s.controlPoint(0,7) =  0.707107f;
    s.controlPoint(0,8) =  1;

    if ( dimension > 1 )
    {
      // Fill in the y control points.
      s.controlPoint(1,0) =  0;
      s.controlPoint(1,1) =  0.707107f;
      s.controlPoint(1,2) =  1;
      s.controlPoint(1,3) =  0.707107f;
      s.controlPoint(1,4) =  0;
      s.controlPoint(1,5) = -0.707107f;
      s.controlPoint(1,6) = -1;
      s.controlPoint(1,7) = -0.707107f;
      s.controlPoint(1,8) =  0;
    }

    // Fill in all other dimensions.
    for ( SizeType i = 2; i < dimension; ++i )
    {
      for ( SizeType j = 0; j < numCtrPts[0]; ++j )
      {
        s.controlPoint(i,j) = 0;
      }
    }

    // Fill in the weights.
    s.weight(0) = 1;
    s.weight(1) = 0.707107f;
    s.weight(2) = 1;
    s.weight(3) = 0.707107f;
    s.weight(4) = 1;
    s.weight(5) = 0.707107f;
    s.weight(6) = 1;
    s.weight(7) = 0.707107f;
    s.weight(8) = 1;
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
//  Make a circle.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void circle ( 
  SplineType &s, 
  typename SplineType::SizeType dimension = 3 )
{
  GN_CAN_BE_CURVE ( SplineType );
  GN::Create::Detail::Circle<SplineType>::make ( s, dimension );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a circle.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void circle ( 
  SplineType &s, 
  const typename SplineType::Vec3 &center,
  typename SplineType::DependentType radius )
{
  GN_CAN_BE_CURVE ( SplineType );
  typedef typename SplineType::Matrix44 Matrix44;
  typedef typename SplineType::Vec3 Vec3;

  // Make a 3D unit circle at the origin.
  GN::Create::circle ( s, 3 );

  // Scale it by the radius.
  GN::Transform::scale ( s, Vec3 ( radius, radius, 1 ) );

  // Translate it to the center.
  GN::Transform::translate ( s, center );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a circle.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void circle ( 
  SplineType &s, 
  const typename SplineType::Vec2 &center,
  typename SplineType::DependentType radius )
{
  GN_CAN_BE_CURVE ( SplineType );
  typedef typename SplineType::Matrix44 Matrix44;
  typedef typename SplineType::Vec2 Vec2;

  // Make a 2D unit circle at the origin.
  GN::Create::circle ( s, 2 );

  // Scale it by the radius.
  GN::Transform::scale ( s, Vec2 ( radius, radius ) );

  // Translate it to the center.
  GN::Transform::translate ( s, center );
}


}; // namespace Create
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_CIRCLE_ALGORITHMS_H_
