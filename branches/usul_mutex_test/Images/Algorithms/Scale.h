
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_HISTOGRAM_H__
#define __IMAGES_HISTOGRAM_H__


namespace Images 
{

template < class Image >  void   scale ( Image& image, unsigned int value )
{
  switch ( image.pixelFormat() )
  {
  case( Image::GRAYSCALE ):
  case( Image::GRAYSCALE_ALPHA ):
    {
      for( unsigned int w = 0; w < image.width(); ++w )
      {
        for( unsigned int h = 0; h < image.height(); ++h )
        {
          Image::ValueType value ( image( w, h ) * scale );

          if( value > 1.0  )
            value = 1.0;
          
          image( w, h ) = value; 
        }
      }
    }
    break;
  default:
    throw std::runtime_error ( "Image format not supported." );
  };
}

}

#endif // __IMAGES_HISTOGRAM_H__
