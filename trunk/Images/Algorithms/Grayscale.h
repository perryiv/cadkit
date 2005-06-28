
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_GRAYSCALE_H__
#define __IMAGES_GRAYSCALE_H__


namespace Images 
{


///////////////////////////////////////////////////////////////////////////////
//
//  Is this image already in grayscale
//
///////////////////////////////////////////////////////////////////////////////

template < class Image > bool isGrayscale    ( const Image& image )
{
  if ( image.pixelFormat() == Image::GRAYSCALE || image.pixelFormat() == Image::GRAYSCALE_ALPHA )
    return true;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convertrgb to grayscale value
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  Image::ValueType getGrayscaleValue ( Image::ValueType r, Image::ValueType g, Image::ValueType b )
  {
    return ( r + g + b ) / 3;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image from rgb to grayscale
//
///////////////////////////////////////////////////////////////////////////////

template < class RGBImage, class GrayImage > void rgbToGrayscale ( RGBImage& image, GrayImage& grayImage  )
{
  // Return now if image is already grayscale
  if( isGrayscale ( image ) )
  {
    return;
  }

  Image newImage;

  const unsigned int width ( image.width() );
  const unsigned int height ( image.height() );


  switch ( image.pixelFormat( ) )
  {
  case ( Image::RGB ):
    {
      newImage.allocate( width, height, Image::GRAYSCALE );

      for( unsigned int h = 0; h < height; ++h )
      {
        for( unsigned int w = 0; w < width; ++w )
        {
          newImage(h, w, Image::GRAY )  = Detail::getGrayscaleValue( image( h, w, Image::RED), image( h, w, Image::GREEN), image( h, w, Image::BLUE) );
        }
      }
    }
    break;
  case ( Image::RGBA ):
    {
      newImage.allocate( image.width(), image.height(), Image::GRAYSCALE_ALPHA );

      for( unsigned int h = 0; h < height; ++h )
      {
        for( unsigned int w = 0; w < width; ++w )
        {
            newImage(h, w, Image::GRAY )  = Detail::getGrayscaleValue( image( h, w, Image::RED), image( h, w, Image::GREEN), image( h, w, Image::BLUE) );
            newImage(h, w, Image::GALPHA ) = image(h, w, Image::ALPHA );
        }
      }
    }
    break;
  }

  image = newImage;
}

}


#endif // __IMAGES_GRAYSCALE_H__
