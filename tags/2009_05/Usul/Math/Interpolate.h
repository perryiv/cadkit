
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_MATH_INTERPOLATE_H__
#define __USUL_MATH_INTERPOLATE_H__


namespace Usul {
namespace Math {

template < class T >
struct Interpolate
{
  /// Linear interpolate.
  static T linear ( double u, const T& a, const T& b )
  {
    return ( ( a * ( 1.0 - u ) ) + ( b * u ) );
  }

  /// Bilinear interpolate.
  /* Corners are:
       b ----- c
    ^  |       |
    |  |       |
    v  |       |
       a ----- d 
         u->
   */
  static T bilinear ( double u, double v, const T& a, const T& b, const T& c, const T& d )
  {
    return ( ( a *( 1 - u ) * ( 1 - v ) ) + ( d * ( 1 - u ) * v ) + ( b * u * ( 1 - v ) ) + ( c * u * v ) );
  }

  /// Trilinear interpolate.
  static T trilinear ( double u, double v, double w, const T& a, const T& b, const T& c, const T& d, const T& e, const T& f, const T& g, const T& h )
  {
    T t0 ( bilinear ( u, v, a, b, c, d ) );
    T t1 ( bilinear ( u, v, e, f, g, h ) );

    return  ( ( ( 1 - w ) * t0 ) + ( w * t1 ) );
  }

};


}
}

#endif // __USUL_MATH_INTERPOLATE_H__
