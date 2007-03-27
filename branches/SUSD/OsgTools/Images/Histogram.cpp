
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/Histogram.h"

#include "osg/Image"

#include <iostream>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
//
//  Calculate histogram for the image
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::calculateHistogram ( Histogram &red, Histogram &green, Histogram &blue, Histogram &gray, const osg::Image& image )
{
  const int width  ( image.s() );
  const int height ( image.t() );
  const int depth  ( image.r() );

  switch ( image.getPixelFormat() )
  {
  case GL_ALPHA:
  case GL_LUMINANCE:
  case GL_LUMINANCE_ALPHA:
    {
      gray.resize(256);

      for( int s = 0; s < width; ++s )
      {
        for(int t = 0; t < height; ++t)
        {
          for(int r = 0; r < depth; ++ r )
          {
            const unsigned char value ( *image.data(s,t,r) );

            gray.at( value )++;
          }
        }
      }
    }
    break;

  case GL_RGB:
  case GL_RGBA:
    {
      red.resize ( 256 );
      green.resize ( 256 );
      blue.resize ( 256 );

      for( int s = 0; s < width; ++s )
      {
        for(int t = 0; t < height; ++t)
        {
          for(int r = 0; r < depth; ++ r )
          {
            const unsigned char *pixel ( image.data( s, t, r ) );

            const unsigned char redValue   ( *pixel );
            const unsigned char greenValue ( *++pixel );
            const unsigned char blueValue  ( *++pixel );

            red.at( redValue )++;
            green.at( greenValue )++;
            blue.at( blueValue )++;
          }
        }
      }
    }
    break;
  default:
    throw std::runtime_error ( "Image format not supported." );
  }


}
