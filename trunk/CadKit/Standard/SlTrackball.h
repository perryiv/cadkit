
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlTrackball.h: Calculates the roation matrix for a virtual trackball.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TRACKBALL_H_
#define _CADKIT_STANDARD_LIBRARY_TRACKBALL_H_

#include "SlMatrix44.h"
#include "SlVec4.h"
#include "SlVec2.h"
#include "SlVec3.h"
#include "SlConstants.h"


namespace CadKit
{
template<class T> class SlTrackball
{
public:

  typedef SlVec2<T> Vec2;
  typedef SlVec3<T> Vec3;
  typedef SlVec4<T> Vec4;
  typedef SlMatrix44<T> Matrix4;

  // Constructor.
  SlTrackball() : _size ( static_cast<T>(0.8) ){}

  // If point1 and point2 are window coordinates, this will compute the 
  // rotation matrix that will rotate the geometry in such as way as to 
  // behave like a trackball.
  void          getMatrix ( const Vec2 &point1, const Vec2 &point2, Matrix4 &R );

protected:

  // Size as a fraction of window size (default = 0.8).
  const T _size;

  void          _getQuaternion ( const Vec2 &point1, const Vec2 &point2, Vec4 &quat );
  static void   _getQuaternion ( const Vec3 &axis, const T &phi, Vec4 &quat );
  static void   _getMatrix ( const Vec4 &quat, Matrix4 &R );

  static T      _projectToSphere ( const T &r, const T &x, const T &y );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Simulate a track-ball. Project the points onto the virtual
//  trackball, then figure out the axis of rotation, which is the cross
//  product of P1 x P2 and O x P1 (O is the center of the ball, 0,0,0).
//
//  Note: This is a deformed trackball, a trackball in the center,
//  but is deformed into a hyperbolic sheet of rotation away from the
//  center. This particular function was chosen after trying out
//  several variations.
//
//  Pass the x and y coordinates of the last and current positions of 
//  the mouse, scaled so they are from [-1,1]. The resulting rotation 
//  is returned as a quaternion rotation in the first paramater.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTrackball<T>::_getQuaternion ( const Vec2 &point1, const Vec2 &point2, Vec4 &quat )
{
  const T CONST_2 ( static_cast<T> ( 2 ) );

  // Check for zero rotation.
  if ( point1 == point2 )
  {
    quat.setValue ( 0, 0, 0, 1 );
    return;
  }  

  // First, figure out z-coordinates for projection of P1 and P2 to
  // deformed sphere.
  Vec3 P1, P2;
  P1[0] = point1[0];
  P1[1] = point1[1];
  P1[2] = _projectToSphere ( _size, point1[0], point1[1] );
  P2[0] = point2[0];
  P2[1] = point2[1];
  P2[2] = _projectToSphere ( _size, point2[0], point2[1] );

  // The axis of rotation is cross product of P1 and P2.
  Vec3 axis = P2.cross ( P1 );

  // Figure out how much to rotate around that axis.
  Vec3 d = P1 - P2;
  T t = d.getLength() / ( CONST_2 * _size );

  // Avoid problems with out-of-control values.
  if ( t >  1 ) t =  1;
  if ( t < -1 ) t = -1;

  // How much to rotate about axis.
  T phi = CONST_2 * asinf ( t );
  axis.normalize();

  // Call the other one.
  _getQuaternion ( axis, phi, quat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  This function computes a quaternion based on an axis (defined by
//  the given vector) and an angle about which to rotate.  The angle is
//  expressed in radians.  The result is put into the third argument.
//  The given axis must be normalized.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTrackball<T>::_getQuaternion ( const Vec3 &axis, const T &phi, Vec4 &quat )
{
  const T angle ( phi * ( static_cast<T> ( 0.5 ) ) );

  quat[0] = axis[0];
  quat[1] = axis[1];
  quat[2] = axis[2];

  T scale = CadKit::getSine ( angle );

  quat[0] *= scale;
  quat[1] *= scale;
  quat[2] *= scale;
  quat[3] = CadKit::getCosine ( angle );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Project an x,y pair onto a sphere of radius r or a hyperbolic sheet
//  if we are away from the center of the sphere.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlTrackball<T>::_projectToSphere ( const T &r, const T &x, const T &y )
{
  const T ONE_OVER_SQRT_OF_2 ( static_cast<T> ( SL_INV_SQRT_2_D ) );
  T z;

  T d = CadKit::getSquareRoot ( x * x + y * y );

  if ( d < r * ONE_OVER_SQRT_OF_2 ) // Inside sphere.
  {
    z = CadKit::getSquareRoot ( r * r - d * d );
  } 
  
  else // On hyperbola.
  {
    T t = r * ONE_OVER_SQRT_OF_2;
    z = t * t / d;
  }

  return z;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a rotation matrix, given a quaternion rotation.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTrackball<T>::_getMatrix ( const Vec4 &quat, Matrix4 &R )
{
  R.setFromQuaternion ( quat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  If point1 and point2 are window coordinates, this will compute the 
//  rotation matrix that will rotate the geometry in such as way as to 
//  behave like a trackball.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlTrackball<T>::getMatrix ( const Vec2 &point1, const Vec2 &point2, Matrix4 &R )
{
  Vec4 quat;
  this->_getQuaternion ( point1, point2, quat );
  this->_getMatrix ( quat, R );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlTrackball<long double> SlTrackballld;
typedef SlTrackball<double>      SlTrackballd;
typedef SlTrackball<float>       SlTrackballf;


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_TRACKBALL_H_
