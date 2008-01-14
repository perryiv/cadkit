
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/RasterGroup.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/Functions/Color.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

using namespace StarSystem;

USUL_FACTORY_REGISTER_CREATOR ( RasterGroup );


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

  // Serialization glue.
  this->_addMember ( "layers", _layers );
  this->_addMember ( "use_in_memory_image_cache", _useCache );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::RasterGroup ( const RasterGroup& rhs ) : 
  BaseClass ( rhs ),
  _layers ( rhs._layers ),
  _cache ( rhs._cache ),
  _useCache ( rhs._useCache )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup& RasterGroup::operator= ( const RasterGroup& rhs )
{
  BaseClass::operator= ( rhs );
  
  _layers = rhs._layers;
  _cache = rhs._cache;
  _useCache = rhs._useCache;
  
  return *this;
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
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RasterGroup::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr clone ( new RasterGroup ( *this ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the layer.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::append ( IRasterLayer* layer )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != layer )
  {
    Guard guard ( this );
    _layers.push_back ( layer );
    this->_updateExtents ( layer );

    // Clear the cache because some or all of the images are now incorrect.
    _cache.clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the layer.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::remove ( IRasterLayer* layer )
{
  USUL_TRACE_SCOPE;
  
  if ( 0x0 != layer )
  {
    Guard guard ( this );
    
    // Remove the layer.
    _layers.erase ( 
                   std::remove_if ( _layers.begin(), 
                                    _layers.end(), 
                                    Usul::Interfaces::IRasterLayer::QueryPtr::IsEqual ( layer ) ),
                   _layers.end() );
    
    // Clear the cache because some or all of the images are now incorrect.
    _cache.clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the extents.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::_updateExtents ( IRasterLayer* layer  )
{
  USUL_TRACE_SCOPE;
  
  Extents e ( this->extents() );
  
  Usul::Interfaces::ILayerExtents::QueryPtr le ( layer );
  
  const double minLon ( le.valid() ? le->minLon() : -180.0 );
  const double minLat ( le.valid() ? le->minLat() :  -90.0 );
  const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
  const double maxLat ( le.valid() ? le->maxLat() :   90.0 );
  
  e.expand ( Extents ( minLon, minLat, maxLon, maxLat ) );
  
  this->extents ( e );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::ImagePtr RasterGroup::texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *job, IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this );

  // Have we been cancelled?
  if ( ( 0x0 != job ) && ( true == job->canceled() ) )
    job->cancel();

  // See if it's in the cache.
  ImagePtr result ( this->_cacheFind ( extents, width, height ) );
  if ( true == result.valid() )
    return result;

  // Get copy of the layers.
  Layers layers ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );

  // If there are no layers...
  if ( true == layers.empty() )
    return ImagePtr ( 0x0 );

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
      Usul::Interfaces::ILayerExtents::QueryPtr le ( layer );
      
      const double minLon ( le.valid() ? le->minLon() : -180.0 );
      const double minLat ( le.valid() ? le->minLat() :  -90.0 );
      const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
      const double maxLat ( le.valid() ? le->maxLat() :   90.0 );
      
      Extents e ( minLon, minLat, maxLon, maxLat );
      
      if ( extents.intersects ( e ) )
      {
        // Get the image for the layer.
        osg::ref_ptr < osg::Image > image ( layer->texture ( extents, width, height, level, job, caller ) );

        if ( image.valid() )
        {
          // Is this the first image?
          if ( false == result.valid() )
          {
            // We always make an image and composite to handle formats other than GL_RGBA.
            result = this->_createBlankImage( width, height );
          }
          
          // See if it has raster alpha data.
          Usul::Interfaces::IRasterAlphas::QueryPtr ra ( layer );

          // Copy the alphas.
          Alphas alphas ( ( true == ra.valid() ) ? ra->alphas() : Alphas() );
          
          // Composite the images.
          this->_compositeImages ( *result, *image, alphas, job );

          // Cache the result.
          this->_cacheAdd ( extents, width, height, result.get() );
        }
      }
    }
  }

  // Return the result.
  return result;
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
  if ( ( static_cast<int> ( width ) != image.s() ) || ( static_cast<int> ( height ) != image.t() ) )
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
      iter = alphas.find ( Usul::Functions::Color::pack ( r, g, b, 0 ) );
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
      const unsigned char useThisAlpha ( ( hasExtraAlpha ) ? ( static_cast < unsigned char > ( iter->second ) ) : ( src[3] ) );

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

RasterGroup::ImagePtr RasterGroup::_cacheFind ( const Extents& extents, unsigned int width, unsigned int height ) const
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
