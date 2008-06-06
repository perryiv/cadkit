
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_THRESHOLD_H__
#define __IMAGES_THRESHOLD_H__


namespace Images 
{


///////////////////////////////////////////////////////////////////////////////
//
//  Binarized data based on threshold
//
///////////////////////////////////////////////////////////////////////////////

template < class Image > void threshold ( Image& image, double t )
{
  for( unsigned int w = 0; w < image.width(); ++w )
  {
    for( unsigned int h = 0; h < image.height(); ++h )
    {
      Image::ValueType &value ( image( w, h ) );
      if( value < th )
        value = 0;
      else
        value = 1.0;
    }
  }
}

}

#endif // __IMAGES_THRESHOLD_H__
