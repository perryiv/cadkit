
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlColor.h: Color vector class.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_COLOR_TEMPLATE_H_
#define _CADKIT_STANDARD_LIBRARY_COLOR_TEMPLATE_H_

#include "SlVec4.h"
#include "SlTypedefs.h"
#include "SlDeclareConst.h"
#include "SlMinMax.h"

// For convenience.
#define SL_COLOR_CLASS_ARGS class Real, class Integer, Integer MAX_INTEGER, class PackedInteger
#define SL_COLOR_FUNCT_ARGS Real, Integer, MAX_INTEGER, PackedInteger
#define SL_COLOR_VEC4_ARGS Real


namespace CadKit
{
template<SL_COLOR_CLASS_ARGS> class SlColor : public SlVec4<SL_COLOR_VEC4_ARGS>
{
public:

  // For convenience, use with getChannel().
  enum { RED = 0, GREEN, BLUE, ALPHA };
  enum { HUE = 0, SATURATION, VALUE };

  // Possible hex formats.
  enum HexFormat { BPP_15, BPP_16, BPP_24, BPP_32 };

  SlColor();
  SlColor ( const PackedInteger &color, const bool &hasAlpha );
  SlColor ( const Real &c0, const Real &c1, const Real &c2, const Real &alpha = 1 );
  SlColor ( const Integer &c0, const Integer &c1, const Integer &c2, const Integer &alpha = MAX_INTEGER );
  SlColor ( const SlVec4<SL_COLOR_VEC4_ARGS> &color );

  // Get the color channel.
  const Real &            getChannel ( const Integer &which ) const;

  // Get the color.
  void                    getColor ( PackedInteger &color ) const;
  void                    getColor ( Real &c0, Real &c1, Real &c2, Real &alpha ) const;
  void                    getColor ( Integer &c0, Integer &c1, Integer &c2, Integer &alpha  ) const;
  void                    getColor ( SlVec4<SL_COLOR_VEC4_ARGS> &color ) const;

  // Get the packed integer representation (based on the template argument).
  // This will retain the alpha channel.
  PackedInteger           getPacked() const;

  // Get the specific packed integer representation.
  // This will not retain the alpha channel, alpha information is lost.
  SlUint16                get15BitHex() const;
  SlUint16                get16BitHex() const;
  SlUint32                get24BitHex() const;
  SlUint32                get32BitHex() const;

  // See if the individual channels are in range.
  bool                    isValid() const;

  // Convert internal state from one color model to another.
  void                    hsv2rgb();
  void                    rgb2hsv();

  // Put all channels in range [0,1] by doing a vector normalization.
  void                    normalize();

  // Set the color.
  void                    setColor ( const PackedInteger &color, const bool &hasAlpha );
  void                    setColor ( const Real &c0, const Real &c1, const Real &c2, const Real &alpha = 1 );
  void                    setColor ( const Integer &c0, const Integer &c1, const Integer &c2, const Integer &alpha = MAX_INTEGER );
  void                    setColor ( const SlVec4<SL_COLOR_VEC4_ARGS> &color );

  // Truncate the channels at [0,1].
  void                    truncate();

protected:

  static void             _hsv2rgb ( const Real &h, const Real &s, const Real &v, Real &r, Real &g, Real &b );
  static void             _rgb2hsv ( const Real &r, const Real &g, const Real &b, Real &h, Real &s, Real &v );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlColor<SL_COLOR_FUNCT_ARGS>::SlColor() : 
  SlVec4<SL_COLOR_VEC4_ARGS> ( 0, 0, 0, 1 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlColor<SL_COLOR_FUNCT_ARGS>::SlColor ( const PackedInteger &color, const bool &hasAlpha ) : SlVec4<SL_COLOR_VEC4_ARGS>()
{
  this->setColor ( color, hasAlpha );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlColor<SL_COLOR_FUNCT_ARGS>::SlColor ( const Real &c0, const Real &c1, const Real &c2, const Real &alpha ) : 
  SlVec4<SL_COLOR_VEC4_ARGS> ( c0, c1, c2, alpha )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlColor<SL_COLOR_FUNCT_ARGS>::SlColor ( const Integer &c0, const Integer &c1, const Integer &c2, const Integer &alpha ) : 
  SlVec4<SL_COLOR_VEC4_ARGS> ( c0, c1, c2, alpha )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlColor<SL_COLOR_FUNCT_ARGS>::SlColor ( const SlVec4<SL_COLOR_VEC4_ARGS> &color ) : 
  SlVec4<SL_COLOR_VEC4_ARGS> ( color )
{
  // This constructor is needed for using the inherited operators.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color channel.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline const Real &SlColor<SL_COLOR_FUNCT_ARGS>::getChannel ( const Integer &which ) const
{
  return SlVec4<SL_COLOR_VEC4_ARGS>::operator [] ( which );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color. This function assumes little-endian byte order.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::getColor ( PackedInteger &color ) const
{
  // This is similar to what we do in the other getColor(), however we have 
  // to use "PackedInteger" instead of "Integer" for the casts.
  const Real factor ( static_cast<Real> ( MAX_INTEGER ) );
  PackedInteger c0    = static_cast<PackedInteger> ( this->_v[0] * factor );
  PackedInteger c1    = static_cast<PackedInteger> ( this->_v[1] * factor );
  PackedInteger c2    = static_cast<PackedInteger> ( this->_v[2] * factor );
  PackedInteger alpha = static_cast<PackedInteger> ( this->_v[3] * factor );

  // Shift the bits into place.
  switch ( sizeof ( PackedInteger ) )
  {
  case 8: // 64 bits

    color = ( c0 << 0 ) | ( c1 << 16 ) | ( c2 << 32 ) | ( alpha << 48 );
    break;

  case 4: // 32 bits

    color = ( c0 << 0 ) | ( c1 << 8 ) | ( c2 << 16 ) | ( alpha << 24 );
    break;

  case 2: // 16 bits

    color = ( c0 << 0 ) | ( c1 << 4 ) | ( c2 << 8 ) | ( alpha << 12 );
    break;

  default:

    SL_ASSERT ( 0 ); // Unexpected size for the "PackedInteger" type.
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::getColor ( Real &c0, Real &c1, Real &c2, Real &alpha ) const
{
  this->getValue ( c0, c1, c2, alpha );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::getColor ( Integer &c0, Integer &c1, Integer &c2, Integer &alpha ) const
{
  const Real factor ( static_cast<Real> ( MAX_INTEGER ) );
  c0    = static_cast<Integer> ( this->_v[0] * factor );
  c1    = static_cast<Integer> ( this->_v[1] * factor );
  c2    = static_cast<Integer> ( this->_v[2] * factor );
  alpha = static_cast<Integer> ( this->_v[3] * factor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::getColor ( SlVec4<SL_COLOR_VEC4_ARGS> &color ) const
{
  color.setValue ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the packed integer representation.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline PackedInteger SlColor<SL_COLOR_FUNCT_ARGS>::getPacked() const
{
  PackedInteger packed ( 0 );
  this->getColor ( packed );
  return packed;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Algorithm from: "Computer Graphics, Principles and Practice", 
//  2nd edition in "C", Foley, van Dam, page 593.
//
//  Convert (hue,saturation,value) to (red,green,blue).
//  All are in the range [0,1].
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::_hsv2rgb ( const Real &h, const Real &s, const Real &v, Real &r, Real &g, Real &b )
{
  const Real CONST_360 ( static_cast<Real> ( 360 ) );
  const Real CONST_60  ( static_cast<Real> ( 60 ) );
  const Real CONST_1   ( static_cast<Real> ( 1 ) );

  Real hue ( h * CONST_360 );
  Real saturation ( s ), value ( v ), f, p, q, t;
  Integer i;

  // If the color is on the black and white center line...
  if ( 0 == saturation )
  {
    // Achromatic color, there is no hue...
    if ( 0 == hue )
    {
      // This is an approximation.
      r = g = b = value;
    }

    // Hue is ok...
    else
    {
      // Set the rgb triplet.
      r = g = b = value;
    }
  }

  // Otherwise...
  else
  {
    // Chromatic color, s != 0, so there is a hue.
    if ( CONST_360 == hue )
    {
      hue = 0;
    }

    hue /= CONST_60; // Hue is now in [0,6) ?
    i = static_cast<Integer> ( hue ); // We want to truncate here.

    f = hue - i;  // Fractional part of hue.
    p = value * ( CONST_1 - saturation );
    q = value * ( CONST_1 - ( saturation * f ) );
    t = value * ( CONST_1 - ( saturation * ( CONST_1 - f ) ) );

    switch ( i )
    {
    case 0: r = value; g =  t; b =  p; break;
    case 1: r =  q; g = value; b =  p; break;
    case 2: r =  p; g = value; b =  t; break;
    case 3: r =  p; g =  q; b = value; break;
    case 4: r =  t; g =  p; b = value; break;
    case 5: r = value; g =  p; b =  q; break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Algorithm from: "Computer Graphics, Principles and Practice", 
//  2nd edition in "C", Foley, van Dam, page 593.
//
//  Convert (red,green,blue) to (hue,saturation,value).
//  All are in the range [0,1].
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::_rgb2hsv ( const Real &r, const Real &g, const Real &b, Real &h, Real &s, Real &v )
{
  const Real CONST_360 ( static_cast<Real> ( 360 ) );
  const Real CONST_60  ( static_cast<Real> ( 60 ) );
  const Real CONST_4   ( static_cast<Real> ( 4 ) );
  const Real CONST_2   ( static_cast<Real> ( 2 ) );
  const Real CONST_0   ( static_cast<Real> ( 0 ) );

  Real delta ( 0 );
  Real maximum ( CadKit::max ( r, g, b ) );
  Real minimum ( CadKit::min ( r, g, b ) );
  
  // Set the value.
  v = maximum;
  
  // Set the saturation.
  s = ( CONST_0 != maximum ) ? ( ( maximum - minimum ) / maximum ) : CONST_0;

  // Chromatic case, saturation is not 0, so determine hue.
  if ( CONST_0 != s )
  {
    delta = maximum - minimum;

    if ( r == maximum )
    {
      h = ( g - b ) / delta;
    }

    else if ( g == maximum )
    {
      h = CONST_2 + ( b - r ) / delta;
    }

    else if ( b == maximum )  
    {
      h = CONST_4 + ( r - g ) / delta;
    }

    h *= CONST_60;

    if ( h < CONST_0 ) // Should we change "if" to "while" here?
    {
      h += CONST_360;
    }
  }
  
  // Otherwise, hue is undefined.
  else 
  {
    h = CONST_0;
  }

  // Scale hue to [0,1].
  h /= CONST_360;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the given "color" to the hex format specified with "bpp"
//  (bits per pixel). Red, green, and blue can be from [0,1].
//  Note: This is a rewrite of something Kevin wrote.
//  Does this function assume little-endian byte order?
//  This function is intentionally not a member of the class.
//
///////////////////////////////////////////////////////////////////////////////

template<class Real, class Integer> inline void _rgb2hex 
  ( const Real &red, const Real &green, const Real &blue, const short &bpp, Integer &color )
{
  const Real CONST_255 ( static_cast<Real> ( 255 ) );
  const Real CONST_63  ( static_cast<Real> ( 63 ) );
  const Real CONST_31  ( static_cast<Real> ( 31 ) );

  Integer r, g, b;

  switch ( bpp )
  {
  case 15:

    SL_ASSERT ( sizeof ( Integer ) >= 2 ); // Catch truncations.
    r = static_cast<Integer> ( CONST_31 * red );
    g = static_cast<Integer> ( CONST_31 * green );
    b = static_cast<Integer> ( CONST_31 * blue );
    color = ( ( ( r << 10 ) & 0x7C00 ) | ( ( g << 5 ) & 0x03E0 ) | ( b & 0x001F ) );
    break;

  case 16: 

    SL_ASSERT ( sizeof ( Integer ) >= 2 ); // Catch truncations.
    r = static_cast<Integer> ( CONST_31 * red );
    g = static_cast<Integer> ( CONST_63 * green );
    b = static_cast<Integer> ( CONST_31 * blue );
    color = ( ( ( r << 11 ) & 0xF800 ) | ( ( g << 5 ) & 0x07E0 ) | ( b & 0x001F ) );
    break;

  case 24:
  case 32: 

    SL_ASSERT ( sizeof ( Integer ) >= 4 ); // Catch truncations.
    r = static_cast<Integer> ( CONST_255 * red );
    g = static_cast<Integer> ( CONST_255 * green );
    b = static_cast<Integer> ( CONST_255 * blue );
    color = ( ( ( r << 16 ) & 0x00FF0000 ) | ( ( g << 8 ) & 0x0000FF00 ) | ( b & 0x000000FF ) );
    break;

  default:

    SL_ASSERT ( 0 ); // Unsupported.
    color = 0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert internal state from HSV to RGB.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::hsv2rgb()
{
  Real r, g, b;
  this->_hsv2rgb ( this->_v[0], this->_v[1], this->_v[2], r, g, b );
  this->setValue ( r, g, b, this->_v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert internal state from RGB to HSV.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::rgb2hsv()
{
  Real h, s, v;
  this->_rgb2hsv ( this->_v[0], this->_v[1], this->_v[2], h, s, v );
  this->setValue ( h, s, v, this->_v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return 15 bpp (bits per pixel) representation.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlUint16 SlColor<SL_COLOR_FUNCT_ARGS>::get15BitHex() const
{
  SlUint16 color;
  CadKit::_rgb2hex ( this->_v[0], this->_v[1], this->_v[2], 15, color );
  return color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return 16 bpp (bits per pixel) representation.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlUint16 SlColor<SL_COLOR_FUNCT_ARGS>::get16BitHex() const
{
  SlUint16 color;
  CadKit::_rgb2hex ( this->_v[0], this->_v[1], this->_v[2], 16, color );
  return color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return 24 bpp (bits per pixel) representation.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlUint32 SlColor<SL_COLOR_FUNCT_ARGS>::get24BitHex() const
{
  SlUint32 color;
  CadKit::_rgb2hex ( this->_v[0], this->_v[1], this->_v[2], 24, color );
  return color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return 32 bpp (bits per pixel) representation.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline SlUint32 SlColor<SL_COLOR_FUNCT_ARGS>::get32BitHex() const
{
  SlUint32 color;
  CadKit::_rgb2hex ( this->_v[0], this->_v[1], this->_v[2], 32, color );
  return color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Put all channels in range [0,1].
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::normalize()
{
  SlVec4<SL_COLOR_VEC4_ARGS>::normalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the individual channels are in range.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline bool SlColor<SL_COLOR_FUNCT_ARGS>::isValid() const
{
  return (
    this->_v[0] >= 0 && this->_v[0] <= 1 &&
    this->_v[1] >= 0 && this->_v[1] <= 1 &&
    this->_v[2] >= 0 && this->_v[2] <= 1 &&
    this->_v[3] >= 0 && this->_v[3] <= 1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Truncate the channels at [0,1].
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::truncate()
{
  const Real CONST_1 ( static_cast<Real> ( 1 ) );
  const Real CONST_0 ( static_cast<Real> ( 0 ) );
  this->_v[0] = std::max ( CONST_0, std::min ( CONST_1, this->_v[0] ) );
  this->_v[1] = std::max ( CONST_0, std::min ( CONST_1, this->_v[1] ) );
  this->_v[2] = std::max ( CONST_0, std::min ( CONST_1, this->_v[2] ) );
  this->_v[3] = std::max ( CONST_0, std::min ( CONST_1, this->_v[3] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color. This function assumes little-endian byte order.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::setColor ( const PackedInteger &color, const bool &hasAlpha )
{
  Integer c0 ( 0 ), c1 ( 0 ), c2 ( 0 ), alpha ( MAX_INTEGER );
  const Real factor ( ( static_cast<Real>(1) ) / ( static_cast<Real> ( MAX_INTEGER ) ) );

  // Break the packed values into the separate channels.
  switch ( sizeof ( PackedInteger ) )
  {
  case 8: // 64 bits

    c0 =                   static_cast<Integer> ( ( color & SL_CONST_UINT64 ( 0x000000000000FFFF ) ) >>  0 );
    c1 =                   static_cast<Integer> ( ( color & SL_CONST_UINT64 ( 0x00000000FFFF0000 ) ) >> 16 );
    c2 =                   static_cast<Integer> ( ( color & SL_CONST_UINT64 ( 0x0000FFFF00000000 ) ) >> 32 );
    alpha = ( hasAlpha ) ? static_cast<Integer> ( ( color & SL_CONST_UINT64 ( 0xFFFF000000000000 ) ) >> 48 ) : MAX_INTEGER;
    break;

  case 4: // 32 bits

    c0 =                   static_cast<Integer> ( ( color & 0x000000FF ) >>  0 );
    c1 =                   static_cast<Integer> ( ( color & 0x0000FF00 ) >>  8 );
    c2 =                   static_cast<Integer> ( ( color & 0x00FF0000 ) >> 16 );
    alpha = ( hasAlpha ) ? static_cast<Integer> ( ( color & 0xFF000000 ) >> 24 ) : MAX_INTEGER;
    break;

  case 2: // 16 bits

    c0 =                   static_cast<Integer> ( ( color & 0x000F ) >>  0 );
    c1 =                   static_cast<Integer> ( ( color & 0x00F0 ) >>  4 );
    c2 =                   static_cast<Integer> ( ( color & 0x0F00 ) >>  8 );
    alpha = ( hasAlpha ) ? static_cast<Integer> ( ( color & 0xF000 ) >> 12 ) : MAX_INTEGER;
    break;

  default:

    SL_ASSERT ( 0 ); // Unexpected size for the "PackedInteger" type.
    break;
  }

  // Now set the color with the individual channels.
  this->setColor ( c0, c1, c2, alpha );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::setColor ( const Real &c0, const Real &c1, const Real &c2, const Real &alpha )
{
  this->setValue ( c0, c1, c2, alpha );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::setColor ( const Integer &c0, const Integer &c1, const Integer &c2, const Integer &alpha )
{
  const Real factor ( ( static_cast<Real>(1) ) / ( static_cast<Real> ( MAX_INTEGER ) ) );

  this->setValue ( 
    static_cast<Real>(c0)    * factor, 
    static_cast<Real>(c1)    * factor, 
    static_cast<Real>(c2)    * factor, 
    static_cast<Real>(alpha) * factor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

template<SL_COLOR_CLASS_ARGS> inline void SlColor<SL_COLOR_FUNCT_ARGS>::setColor ( const SlVec4<SL_COLOR_VEC4_ARGS> &color )
{
  this->setValue ( color );
}


//////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
//////////////////////////////////////////////////////////////////////////

typedef SlColor < double, SlUint32, 255, SlUint32 > SlColord;
typedef SlColor < float,  SlUint32, 255, SlUint32 > SlColorf;


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_COLOR_TEMPLATE_H_
