
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms for applying a transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_TRANSFORMATION_MATRIX_ALGORITHMS_H_
#define _GENERIC_NURBS_LIBRARY_TRANSFORMATION_MATRIX_ALGORITHMS_H_


namespace GN {
namespace Transform {


///////////////////////////////////////////////////////////////////////////////
//
//  Transform the spline by the given matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType_ > inline void matrix ( 
  SplineType_ &spline, 
  const typename SplineType_::Matrix44 &matrix )
{
  typedef SplineType_ SplineType;
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::Vec4 Vec4;
  typedef typename SplineType::Vec3 Vec3;
  typedef typename SplineType::Multiply Multiply;

  // Caller needs to make sure the spline is at least 3D.
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  GN_ERROR_CHECK ( spline.dimension() >= 3 );

  // Needed below.
  SizeType totalNumCtrPts ( spline.totalNumControlPoints() );

  // If it is rational...
  if ( spline.rational() )
  {
    Vec4 v;

    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Populate the vector.
      v[0] = spline.controlPoint ( 0, i ); // wx
      v[1] = spline.controlPoint ( 1, i ); // wy
      v[2] = spline.controlPoint ( 2, i ); // wz
      v[3] = spline.weight ( i );          // w

      // Transform the vector.
      v = Multiply::calculate ( matrix, v );

      // Set the control point.
      spline.controlPoint ( 0, i ) = v[0]; // wx
      spline.controlPoint ( 1, i ) = v[1]; // wy
      spline.controlPoint ( 2, i ) = v[2]; // wz
      spline.weight ( i )          = v[3]; // w
    }
  }

  // Non-rational...
  else
  {
    Vec3 v;

    // Loop through the control points.
    for ( SizeType i = 0; i < totalNumCtrPts; ++i )
    {
      // Populate the vector.
      v[0] = spline.controlPoint ( 0, i ); // x
      v[1] = spline.controlPoint ( 1, i ); // y
      v[2] = spline.controlPoint ( 2, i ); // z

      // Transform the vector.
      v = Multiply::calculate ( matrix, v );

      // Set the control point.
      spline.controlPoint ( 0, i ) = v[0]; // x
      spline.controlPoint ( 1, i ) = v[1]; // y
      spline.controlPoint ( 2, i ) = v[2]; // z
    }
  }
}


}; // namespace Transform
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_TRANSFORMATION_MATRIX_ALGORITHMS_H_
