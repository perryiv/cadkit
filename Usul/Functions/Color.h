
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FUNCTIONS_COLOR_H__
#define __USUL_FUNCTIONS_COLOR_H__

#include "Usul/Math/MinMax.h"

namespace Usul {
namespace Functions {

///////////////////////////////////////////////////////////////////////////////
//
//  Convert red, green, blue to hue, saturation, value.
//
///////////////////////////////////////////////////////////////////////////////

template < class Real >
void rgbToHsv ( Real& h, Real& s, Real& v, Real r, Real g, Real b )
{
  v = Usul::Math::maximum ( r, g, b );
  Real t ( Usul::Math::minimum (r, g, b ) );
  Real delta ( v - t );
  if( v != 0.0 )
    s = delta / v;
  else
    s = 0.0;
  if( s == 0.0 )
  {
    h = 0.0;
  }
  else
  {
    if( r == v )
      h = ( g - b ) / delta;
    else if( g == v )
      h = 2.0f + ( b - r ) / delta;
    else if( b == v )
      h = 4.0f + ( r - g ) / delta;
    h = h * 60.0f;
    if( h < 0.0 ) 
      h = h + 360;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert hue, saturation, value to red, green, blue.
//
///////////////////////////////////////////////////////////////////////////////

template < class Real >
void hsvToRgb ( Real& r, Real& g, Real& b, Real h, Real s, Real v )
{ 
  if( s == 0.0 )
  {
    r = v;
    g = v;
    b = v;
  }
  else
  {
    if( h == 360.0 ) 
      h = 0.0;
    h /= 60.0f;
    unsigned int i ( static_cast < unsigned int > ( h ) );
    Real f ( h - i );
    Real w ( v * ( 1.0f - s ) );
    Real q ( v * ( 1.0f - ( s * f ) ) );
    Real t ( v * ( 1.0f - ( s * ( 1.0f - f ) ) ) );
    switch( i )
    {
      case 0: r=v; g=t; b=w; break;
      case 1: r=q; g=v; b=w; break;
      case 2: r=w; g=v; b=t; break;
      case 3: r=w; g=q; b=v; break;
      case 4: r=t; g=w; b=v; break;
      case 5: r=v; g=w; b=q; break;
    }
  }
}


} // namespace Functions
} // namespace Usul


#endif

