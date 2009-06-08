
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_INVERT_H__
#define __IMAGES_INVERT_H__


namespace Images 
{

///////////////////////////////////////////////////////////////////////////////
//
//  Invert the image.  Currently only grayscale is implemented.
//
///////////////////////////////////////////////////////////////////////////////


template < class Image >  void invert ( Image& image )
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
          Image::ValueType value ( image( w, h, Image::GRAY ) );
          
          image( w, h, Image::GRAY ) = 1.0 - value;
        }
      }
    }
    break;
  default:
    throw std::runtime_error ( "Image format not supported." );
  };

}
}


}

#endif // __IMAGES_INVERT_H__
