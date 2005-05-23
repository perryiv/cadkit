
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/Grayscale.h"

#include "osg/Image"
#include "osg/ref_ptr"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Is this image already in grayscale
//
///////////////////////////////////////////////////////////////////////////////

bool OsgTools::Images::isGrayscale    ( const osg::Image& image )
{
  if ( image.getPixelFormat() == GL_LUMINANCE || image.getPixelFormat() == GL_LUMINANCE_ALPHA )
    return true;
  return false;
}

namespace Detail
{
  unsigned char getGrayscaleValue ( unsigned char r, unsigned char g, unsigned char b )
  {
    unsigned int value ( r + g + b );
    return value / 3;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image from rgb to grayscale
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* OsgTools::Images::rgbToGrayscale ( osg::Image* image )
{
  // Return now if image is already grayscale
  if( isGrayscale ( *image ) )
  {
    return image;
  }

  typedef osg::ref_ptr< osg::Image > ImagePtr;

  ImagePtr newImage ( new osg::Image );

  const int width ( image->s() );
  const int height ( image->t() );


  switch ( image->getPixelFormat( ) )
  {
  case ( GL_RGB ):
    {
      newImage->allocateImage(width, height, 1, GL_LUMINANCE, image->getDataType());

      for(int t=0;t<height;++t)
      {
        unsigned char* dest = newImage->data(0,t,0);
        unsigned char* source = image->data(0,t,0);

        for(int i=0;i<width;++i)
        {
          *dest++ = Detail::getGrayscaleValue( *source++, *source++, *source++ );
        }
      }
      image = new osg::Image ( *newImage, osg::CopyOp::DEEP_COPY_ALL );
    }
    break;
  case ( GL_RGBA ):
    {
      std::cout << "Converting from RGBA to Grayscale with alpha." << std::endl;

      newImage->allocateImage(width, height, 1, GL_LUMINANCE_ALPHA, image->getDataType());

      for(int t=0;t<height;++t)
      {
        unsigned char* dest = newImage->data(0,t,0);
        unsigned char* source = image->data(0,t,0);

        for(int i=0;i<width;++i)
        {
            *dest++ = Detail::getGrayscaleValue( *source++, *source++, *source++ );
            *dest++ = *source++;
        }
      }
    }
    break;
  }

  return newImage.release();
}

