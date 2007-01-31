
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_POLICIES_H__
#define __IMAGES_POLICIES_H__

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include "Images/Enum.h"

namespace Images {


///////////////////////////////////////////////////////////////////////////////
//
//  Policy for RGB image
//
///////////////////////////////////////////////////////////////////////////////

template < class PixelType_ >
struct RGBPolicy
{
  typedef PixelType_ PixelType;
  typedef Usul::Math::Vec3< PixelType > Pixel;
  typedef Images::RGB PixelFormat;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Policy for RGB image with Alpha
//
///////////////////////////////////////////////////////////////////////////////

template < class PixelType_ >
struct RGBAPolicy
{
  typedef PixelType_ PixelType;
  
  enum  
  {
    ChannelSize = sizeof(PixelType)
  };

  typedef Usul::Math::Vec3< PixelType > Pixel;
  typedef Images::RGBA PixelFormat;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Policy for grayscale image
//
///////////////////////////////////////////////////////////////////////////////

template < class PixelType_ >
struct GPolicy
{
  typedef PixelType_ PixelType;

  template < PixelType >
  class Pixel
  {
    Pixel( PixelType value ) :
    _value ( value )
    {
    }

    PixelType& operator[] ( unsigned int )
    {
      return _value;
    }

    const PixelType& operator[] ( unsigned int ) const
    {
      return _value;
    }

  private:
    PixelType _value;
  };

  typedef Pixel<PixelType> Pixel;
  typedef Images::GRAYSCALE PixelFormat;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Policy for grayscale image with alpha
//
///////////////////////////////////////////////////////////////////////////////

template < class PixelType_ >
struct GAPolicy
{
  typedef PixelType_ PixelType;
  typedef Usul::Math::Vec2< PixelType > Pixel;
  typedef Images::GRAYSCALE_ALPHA PixelFormat;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for image with unsigned char
//
///////////////////////////////////////////////////////////////////////////////

typedef RGBPolicy  < unsigned char > UCharRGB;
typedef RGBAPolicy < unsigned char > UCharRGBA;
typedef GPolicy    < unsigned char > UCharG;
typedef GAPolicy   < unsigned char > UCharGA;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for image with float
//
///////////////////////////////////////////////////////////////////////////////

typedef RGBPolicy  < float > FloatRGB;
typedef RGBAPolicy < float > FloatRGBA;
typedef GPolicy    < float > FloatG;
typedef GAPolicy   < float > FloatGA;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for image with double
//
///////////////////////////////////////////////////////////////////////////////

typedef RGBPolicy  < double > DoubleRGB;
typedef RGBAPolicy < double > DoubleRGBA;
typedef GPolicy    < double > DoubleG;
typedef GAPolicy   < double > DoubleGA;

}

#endif // __IMAGES_POLICIES_H__
