
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

OsgTools::Images::Histogram OsgTools::Images::calculateHistogram ( const osg::Image& image )
{
  Histogram histogram;

  histogram.resize( 256 );

  const int width  ( image.s() );
  const int height ( image.t() );

  switch ( image.getPixelFormat() )
  {
  case(GL_ALPHA):
  case(GL_LUMINANCE):
  case(GL_LUMINANCE_ALPHA):
    {
      for( int s = 0; s < width; ++s )
      {
        for(int t=0;t<height;++t)
        {
          for(int r = 0; r < image.r(); ++ r )
          {
            const unsigned char value ( *image.data(s,t,r) );

            histogram.at( value )++;
          }
        }
      }
    }
    break;
  default:
    throw std::runtime_error ( "Image format not supported." );
  };


  return histogram;
}
