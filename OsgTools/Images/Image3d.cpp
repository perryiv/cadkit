
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/Image3d.h"

#include "osg/ref_ptr"
#include "osg/Image"

///////////////////////////////////////////////////////////////////////////////
//
//  Convert list of images to a 3d image
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* OsgTools::Images::image3d( ImageList& images, bool ensureProperTextureSize )
{
  // Make an image
  ImagePtr image3d ( new osg::Image );

  ImagePtr front ( images.front() );

  unsigned int width  ( front->s() );
  unsigned int height ( front->t() );

  GLenum pixelFormat ( front->getPixelFormat() );

  //Ensure proper size for texturing
  int new_s ( front->computeNearestPowerOfTwo( width  ) );
  int new_t ( front->computeNearestPowerOfTwo( height ) );

  if( ensureProperTextureSize )
  {
    width  = new_s;
    height = new_t;
  }

  // Make enough room
  image3d->allocateImage( width, height, images.size(), pixelFormat, images.front()->getDataType());

  // Add each image to the 3d image
  for( ImageList::iterator i = images.begin(); i != images.end(); ++i )
  {
    // Do we need to scale the image for textures
    if( ensureProperTextureSize )
    {
      if( new_s != (*i)->s() || new_t != (*i)->t() )
      {
        (*i)->scaleImage( new_s, new_t, 1 );
      }
    }

    // Copy the image to our 3d image
    image3d->copySubImage ( 0, 0, ( i - images.begin() ), i->get() );
  }

  image3d->setInternalTextureFormat( front->getInternalTextureFormat() );

  // Return the image
  return image3d.release();
}