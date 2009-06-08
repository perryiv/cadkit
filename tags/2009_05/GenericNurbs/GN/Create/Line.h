
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms for lines.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_LINE_ALGORITHMS_H_
#define _GENERIC_NURBS_LIBRARY_LINE_ALGORITHMS_H_

#include "GN/MPL/TypeCheck.h"


namespace GN {
namespace Create {


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Sets the integer data and knot vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > struct CommonLineCode
{
  typedef typename SplineType::SplineClass SplineClass;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::SizeContainer SizeContainer;

  static void set ( SplineClass &s, SizeType dimension )
  {
    GN_CAN_BE_CURVE ( SplineType );

    // The specs for a line.
    const SizeType numIndepVars ( 1 );
    SizeContainer numCtrPts, order;
    numCtrPts.resize ( numIndepVars );
    order.resize     ( numIndepVars );
    numCtrPts[0]     = 2;
    order[0]         = 2;
    const bool rational ( false );

    // Resize the spline.
    s.resize ( dimension, order, numCtrPts, rational );

    // Fill in the knot vector.
    s.knot(0,0) = 0;
    s.knot(0,1) = 0;
    s.knot(0,2) = 1;
    s.knot(0,3) = 1;
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
//  Make a 1D line.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void line ( 
  SplineType &s, 
  typename SplineType::DependentArgument p0,
  typename SplineType::DependentArgument p1 )
{
  GN_CAN_BE_CURVE ( SplineType );
  GN::Create::Detail::CommonLineCode<SplineType>::set ( s, 1 );
  s.controlPoint(0,0) = p0;
  s.controlPoint(0,1) = p1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a 2D line.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void line ( 
  SplineType &s, 
  const typename SplineType::Vec2 &p0, 
  const typename SplineType::Vec2 &p1 )
{
  GN_CAN_BE_CURVE ( SplineType );
  GN::Create::Detail::CommonLineCode<SplineType>::set ( s, 2 );
  s.controlPoint(0,0) = p0[0];
  s.controlPoint(0,1) = p1[0];
  s.controlPoint(1,0) = p0[1];
  s.controlPoint(1,1) = p1[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a 3D line.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > inline void line ( 
  SplineType &s, 
  const typename SplineType::Vec3 &p0, 
  const typename SplineType::Vec3 &p1 )
{
  GN_CAN_BE_CURVE ( SplineType );
  GN::Create::Detail::CommonLineCode<SplineType>::set ( s, 3 );
  s.controlPoint(0,0) = p0[0];
  s.controlPoint(0,1) = p1[0];
  s.controlPoint(1,0) = p0[1];
  s.controlPoint(1,1) = p1[1];
  s.controlPoint(2,0) = p0[2];
  s.controlPoint(2,1) = p1[2];
}


}; // namespace Create
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_LINE_ALGORITHMS_H_
