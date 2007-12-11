
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/RasterGroup.h"

#include "Usul/File/Make.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

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
  BaseClass (),
  _layers   (),
  _cache    (),
  _useCache ( false )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::~RasterGroup()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the layer.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::append ( RasterLayer* layer )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != layer )
  {
    Guard guard ( this );
    _layers.push_back ( layer );
    this->_updateExtents ( *layer );

    // Clear the cache because some or all of the images are now incorrect.
    _cache.clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the extents.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::_updateExtents ( const RasterLayer& layer  )
{
  USUL_TRACE_SCOPE;
  Extents e ( this->extents() );
  e.expand ( layer.extents() );
  this->extents ( e );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* RasterGroup::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this );

  // Have we been cancelled?
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();

  // See if it's in the cache.
  osg::ref_ptr < osg::Image > result ( this->_cacheFind ( extents, width, height ) );
  if ( true == result.valid() )
    return result.release();

  // Get copy of the layers.
  Layers layers ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );

  // If there are no layers...
  if ( true == layers.empty() )
    return 0x0;

  // Loop through each layer.
  for ( Layers::iterator iter = layers.begin(); iter != layers.end(); ++iter )
  {
    // Have we been cancelled?
    if ( ( 0x0 != job ) && ( true == job->canceled() ) )
      job->cancel();

    // Get the layer.
    Layers::value_type layer ( *iter );
    if ( true == layer.valid() )
    {
      if ( extents.intersects ( layer->extents() ) )
      {
        // Get the image for the layer.
        osg::ref_ptr < osg::Image > image ( layer->texture ( extents, width, height, level, job ) );

        if ( image.valid() )
        {
          // Is this the first image?
          if ( false == result.valid() )
          {
            // We always make an image and composite to handle formats other than GL_RGBA.
            result = this->_createBlankImage( width, height );
          }

          // Composite the images.
          this->_compositeImages ( *result, *image, layer->alphas(), job );

          // Cache the result.
          this->_cacheAdd ( extents, width, height, result.get() );
        }
      }
    }
  }

#if 0
  Extents::Vertex min ( extents.minimum() );
  Extents::Vertex max ( extents.maximum() );

  std::ostringstream os;
  os << "C:/adam/temp/debug/level_" << level << "/";
  
  Usul::File::make ( os.str() );

  os << min[0] << "_" << min[1] << "_" << max[0] << "_" << max[1] << "_" << width << "_" << height << ".bmp";
  osgDB::writeImageFile ( *result, os.str() );
#endif

  // Return the result.
  return result.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Composite the two images.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::_compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, Usul::Jobs::Job *job )
{
  USUL_TRACE_SCOPE;

  // We only handle these cases.
  const GLenum format ( image.getPixelFormat() );
  if ( GL_RGBA != format && GL_RGB != format )
    return;

  unsigned char       *dst ( result.data() );
  const unsigned char *src ( image.data()  );

  const unsigned int width  ( result.s() );
  const unsigned int height ( result.t() );

  // We only composite images of the same size.
  if ( ( width != image.s() ) || ( height != image.t() ) )
    return;

  const unsigned int size ( width * height );
  const bool alphaMapEmpty ( alphas.empty() );
  bool hasExtraAlpha ( false );
  Alphas::const_iterator iter ( alphas.end() );

  const bool hasAlpha ( GL_RGBA == format );
  const unsigned int offset ( ( hasAlpha ) ? 4 : 3 );

  for ( unsigned int i = 0; i < size; ++i )
  {
    // Have we been cancelled?
    if ( 0 == ( i % 100 ) )
    {
      if ( ( 0x0 != job ) && ( true == job->canceled() ) )
        job->cancel();
    }

    // Copy the color channels.
    unsigned char r ( src[0] );
    unsigned char g ( src[1] );
    unsigned char b ( src[2] );

    // Is the color in the alpha table?
    if ( false == alphaMapEmpty )
    {
      iter = alphas.find ( Color ( RedGreen ( r, g ), b ) );
      hasExtraAlpha = ( alphas.end() != iter );
    }

    // If the image is competely opaque then set the values.
    if ( ( false == hasAlpha ) && ( false == hasExtraAlpha ) )
    {
      dst[0] = r;
      dst[1] = g;
      dst[2] = b;
    }
    else
    {
      // Get correct alpha.
      const unsigned char useThisAlpha ( ( hasExtraAlpha ) ? iter->second : src[3] );

      // Normalize between zero and one.
      const float a ( static_cast < float > ( useThisAlpha ) / 255.5f );

      // Composite.
      dst[0] = static_cast < unsigned char > ( dst[0] * ( 1 - a ) + ( r * a ) );
      dst[1] = static_cast < unsigned char > ( dst[1] * ( 1 - a ) + ( g * a ) );
      dst[2] = static_cast < unsigned char > ( dst[2] * ( 1 - a ) + ( b * a ) );
    }

    dst[3] = 255;

    dst += 4;
    src += offset;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the image to the cache.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::_cacheAdd ( const Extents& extents, unsigned int width, unsigned int height, osg::Image *image )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _useCache )
  {
    const ImageKey key ( RasterGroup::_makeKey ( extents, width, height ) );
    _cache[key] = image;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Look for the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image *RasterGroup::_cacheFind ( const Extents& extents, unsigned int width, unsigned int height ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  const ImageKey key ( RasterGroup::_makeKey ( extents, width, height ) );
  ImageCache::const_iterator i ( _cache.find ( key ) );
  return ( ( _cache.end() == i ) ? 0x0 : i->second.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the key.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::ImageKey RasterGroup::_makeKey ( const Extents& extents, unsigned int width, unsigned int height )
{
  USUL_TRACE_SCOPE_STATIC;

  KeySize size ( width, height );
  KeyRange lon ( extents.minimum()[0], extents.maximum()[0] );
  KeyRange lat ( extents.minimum()[1], extents.maximum()[1] );
  KeyBounds bounds ( lon, lat );
  ImageKey key ( size, bounds );

  return key;
}
