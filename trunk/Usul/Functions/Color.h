
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to for colors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FUNCTIONS_COLOR_H__
#define __USUL_FUNCTIONS_COLOR_H__

#include "Usul/Math/MinMax.h"
#include "Usul/Types/Types.h"


namespace Usul {
namespace Functions {
namespace Color {


///////////////////////////////////////////////////////////////////////////////
//
//  Convert red, green, blue to hue, saturation, value.
//
///////////////////////////////////////////////////////////////////////////////

template < class Real > inline void rgbToHsv ( Real& h, Real& s, Real& v, Real r, Real g, Real b )
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

template < class Real > inline void hsvToRgb ( Real& r, Real& g, Real& b, Real h, Real s, Real v )
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


///////////////////////////////////////////////////////////////////////////////
//
//  Pack the colors into a 32-bit unsigned integer.
//
///////////////////////////////////////////////////////////////////////////////

inline Usul::Types::Uint32 pack ( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
  return Usul::Types::Uint32 ( ( r << 24 ) | ( g << 16 ) | ( b << 8 ) | a );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pack the colors into a 32-bit unsigned integer.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline Usul::Types::Uint32 pack ( T red, T green, T blue, T alpha, T maxValue = 1 )
{
  const unsigned char r ( static_cast < unsigned char > ( ( static_cast < double > ( red   ) / maxValue ) * 255 ) );
  const unsigned char g ( static_cast < unsigned char > ( ( static_cast < double > ( green ) / maxValue ) * 255 ) );
  const unsigned char b ( static_cast < unsigned char > ( ( static_cast < double > ( blue  ) / maxValue ) * 255 ) );
  const unsigned char a ( static_cast < unsigned char > ( ( static_cast < double > ( alpha ) / maxValue ) * 255 ) );
  return Usul::Functions::Color::pack ( r, g, b, a );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unpack the colors from the 32-bit unsigned integer.
//
///////////////////////////////////////////////////////////////////////////////

template < class ColorType > inline ColorType unpack ( Usul::Types::Uint32 c )
{
  return ColorType ( ( ( c & 0xFF000000 ) >> 24 ), ( ( c & 0x00FF0000 ) >> 16 ), ( ( c & 0x0000FF00 ) >> 8 ), ( c & 0x000000FF ) );
}


} // namespace Color
} // namespace Functions
} // namespace Usul


#endif

