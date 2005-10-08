
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_HISTOGRAM_H__
#define __IMAGES_HISTOGRAM_H__

#include <vector>

namespace Images 
{

typedef std::vector< unsigned int > Histogram;


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate histogram for the image
//
///////////////////////////////////////////////////////////////////////////////

template < class Image > Histogram   calculateHistogram ( const Image& image )
{
  Histogram histogram;

  histogram.resize( 256 );

  const int width  ( image.width() );
  const int height ( image.height() );

  switch ( image.pixelFormat() )
  {
  case( Image::GRAYSCALE ):
  case( Image::GRAYSCALE_ALPHA ):
    {
      for( int s = 0; s < width; ++s )
      {
        for( int t = 0; t < height; ++t)
        {
          const double value ( image ( s,t, Image::GRAY ) );

          histogram.at( static_cast < unsigned int > ( value * 255 ) )++;
        }
      }
    }
    break;
  default:
    throw std::runtime_error ( "Image format not supported." );
  };


  return histogram;
}


}

#endif // __IMAGES_HISTOGRAM_H__
