
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms for applying a scale.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_SCALE_ALGORITHMS_H_
#define _GENERIC_NURBS_LIBRARY_SCALE_ALGORITHMS_H_


namespace GN {
namespace Transform {


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the spline by the given vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType_ > inline void scale ( 
  SplineType_ &spline, 
  const typename SplineType_::Vec4 &s )
{
  typedef SplineType_ SplineType;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::DependentType DependentType;

  // Caller needs to make sure the spline is at least 3D.
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  GN_ERROR_CHECK ( spline.dimension() >= 3 );

  // Needed below.
  SizeType totalNumCtrPts ( spline.totalNumControlPoints() );

  // If it is rational...
  if ( spline.rational() )
  {
    DependentType x0, x1, x2, x3, w, iw;

    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Divide out the weight and scale.
      w = spline.weight ( i );
      iw = 1 / w;
      x0 = spline.controlPoint(0,i) * iw * s[0];
      x1 = spline.controlPoint(1,i) * iw * s[1];
      x2 = spline.controlPoint(2,i) * iw * s[2];
      x3 = spline.controlPoint(3,i) * iw * s[3];

      // Set the control point. Put the weight back.
      spline.controlPoint(0,i) = x0 * w;
      spline.controlPoint(1,i) = x1 * w;
      spline.controlPoint(2,i) = x2 * w;
      spline.controlPoint(3,i) = x3 * w;
    }
  }

  // Non-rational...
  else
  {
    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Scale the control point.
      spline.controlPoint(0,i) *= s[0]; // x0
      spline.controlPoint(1,i) *= s[1]; // x1
      spline.controlPoint(2,i) *= s[2]; // x2
      spline.controlPoint(3,i) *= s[3]; // x3
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the spline by the given vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType_ > inline void scale ( 
  SplineType_ &spline, 
  const typename SplineType_::Vec3 &s )
{
  typedef SplineType_ SplineType;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::DependentType DependentType;

  // Caller needs to make sure the spline is at least 3D.
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  GN_ERROR_CHECK ( spline.dimension() >= 3 );

  // Needed below.
  SizeType totalNumCtrPts ( spline.totalNumControlPoints() );

  // If it is rational...
  if ( spline.rational() )
  {
    DependentType x, y, z, w, iw;

    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Divide out the weight and translate.
      w = spline.weight ( i );
      iw = 1 / w;
      x = spline.controlPoint(0,i) * iw * s[0];
      y = spline.controlPoint(1,i) * iw * s[1];
      z = spline.controlPoint(2,i) * iw * s[2];

      // Set the control point. Put the weight back.
      spline.controlPoint(0,i) = x * w;
      spline.controlPoint(1,i) = y * w;
      spline.controlPoint(2,i) = z * w;
    }
  }

  // Non-rational...
  else
  {
    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Scale the control point.
      spline.controlPoint(0,i) *= s[0]; // x
      spline.controlPoint(1,i) *= s[1]; // y
      spline.controlPoint(2,i) *= s[2]; // z
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the spline by the given vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType_ > inline void scale ( 
  SplineType_ &spline, 
  const typename SplineType_::Vec2 &s )
{
  typedef SplineType_ SplineType;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::DependentType DependentType;

  // Caller needs to make sure the spline is at least 2D.
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  GN_ERROR_CHECK ( spline.dimension() >= 2 );

  // Needed below.
  SizeType totalNumCtrPts ( spline.totalNumControlPoints() );

  // If it is rational...
  if ( spline.rational() )
  {
    DependentType x, y, w, iw;

    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Divide out the weight and translate.
      w = spline.weight ( i );
      iw = 1 / w;
      x = spline.controlPoint(0,i) * iw * s[0];
      y = spline.controlPoint(1,i) * iw * s[1];

      // Set the control point. Put the weight back.
      spline.controlPoint(0,i) = x * w;
      spline.controlPoint(1,i) = y * w;
    }
  }

  // Non-rational...
  else
  {
    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Scale the control point.
      spline.controlPoint(0,i) *= s[0]; // x
      spline.controlPoint(1,i) *= s[1]; // y
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the spline by the given vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType_ > inline void scale ( 
  SplineType_ &spline, 
  const typename SplineType_::DependentType &s )
{
  typedef SplineType_ SplineType;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::DependentType DependentType;

  // Caller needs to make sure the spline is at least 1D.
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  GN_ERROR_CHECK ( spline.dimension() >= 1 );

  // Needed below.
  SizeType totalNumCtrPts ( spline.totalNumControlPoints() );

  // If it is rational...
  if ( spline.rational() )
  {
    DependentType x, w;

    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Divide out the weight and translate.
      w = spline.weight ( i );
      x = ( spline.controlPoint(0,i) / w ) * s[0];

      // Set the control point. Put the weight back.
      spline.controlPoint(0,i) = x * w;
    }
  }

  // Non-rational...
  else
  {
    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Scale the control point.
      spline.controlPoint(0,i) *= s[0]; // x
    }
  }
}


}; // namespace Transform
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_SCALE_ALGORITHMS_H_
