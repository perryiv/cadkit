
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/RasterGroup.h"

#include "osg/ref_ptr"
#include "osg/Image"

#include "osgDB/WriteFile"

using namespace StarSystem;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::RasterGroup() : 
  BaseClass(),
  _layers()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::~RasterGroup()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the layer.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::append ( RasterLayer* layer )
{
  if ( 0x0 != layer )
  {
    Guard guard ( this );
    _layers.push_back ( layer );
    this->_updateExtents ( *layer );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the extents.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::_updateExtents( const RasterLayer& layer  )
{
  Extents e ( this->extents() );
  e.expand ( layer.extents() );
  this->extents ( e );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* RasterGroup::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level )
{
  Guard guard ( this );

  // Create the answer.
  osg::ref_ptr < osg::Image > result ( new osg::Image );
  result->allocateImage ( width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE );
  ::memset ( result->data(), 0, result->getImageSizeInBytes() );

  // Loop through each layer.
  for ( Layers::iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
  {
    if ( extents.intersects ( (*iter)->extents() ) )
    {
      // Get the image for the layer.
      osg::ref_ptr < osg::Image > image ( (*iter)->texture ( extents, width, height, level ) );

      if ( image.valid() )
      {
        // Composite.
        this->_compositeImages ( *result, *image );
      }
    }
  }

#if 0
  Extents::Vertex min ( extents.min() );
  Extents::Vertex max ( extents.max() );

  std::ostringstream os;
  os << "C:/adam/temp/debug/" << min[0] << "_" << min[1] << "_" << max[0] << "_" << max[1] << "_" << width << "_" << height << "_" << level << ".bmp";
  osgDB::writeImageFile ( *result, os.str() );
#endif

  return result.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Composite the two images.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::_compositeImages ( osg::Image& result, const osg::Image& image ) const
{
  unsigned char       *dst ( result.data() );
  const unsigned char *src ( image.data()  );

  const unsigned int width  ( result.s() );
  const unsigned int height ( result.t() );

  const unsigned int size ( width * height );

  for ( unsigned int i = 0; i < size; ++i )
  {
    // If it's competely opaque, set the values.
    if ( 255 == src[3] )
    {
      dst[0] = src[0];
      dst[1] = src[1];
      dst[2] = src[2];
    }
    else
    {
      // Normalize between zero and one.
      float a ( static_cast < float > ( src[3] ) / 255.5 );

      // Composite.
      dst[0] = static_cast < unsigned char > ( dst[0] * ( 1 - a ) + ( src[0] * a ) );
      dst[1] = static_cast < unsigned char > ( dst[1] * ( 1 - a ) + ( src[1] * a ) );
      dst[2] = static_cast < unsigned char > ( dst[2] * ( 1 - a ) + ( src[2] * a ) );
    }

    dst[3] = 255;

    dst += 4;
    src += 4;
  }
}
