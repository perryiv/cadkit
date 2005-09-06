
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ImageOr.h"

#include "osg/Image"
#include "osg/ref_ptr"

osg::Image* OsgTools::Images::imageOr ( const osg::Image& imageOne, const osg::Image& imageTwo )
{
  osg::ref_ptr< osg::Image > image ( new osg::Image );

  image->allocateImage( imageOne.s(), imageOne.t(), 1, imageOne.getPixelFormat(), imageOne.getDataType() );

  for( int s = 0; s < imageOne.s(); ++s )
  {
    for( int t = 0; t < imageOne.t(); ++t )
    {
      if ( *imageOne.data( s, t ) == 255 || *imageTwo.data( s, t ) == 255 )
        *image->data( s, t ) = 255;
      else
        *image->data( s, t ) = 0;
    }
  }

  return image.release();
}
