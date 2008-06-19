
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/RasterGroup.h"
#include "Minerva/Core/Algorithms/Composite.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Make.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/ref_ptr"
#include "osg/Image"

using namespace Minerva::Core::Layers;

USUL_FACTORY_REGISTER_CREATOR ( RasterGroup );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS( RasterGroup, RasterGroup::BaseClass );

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
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::~RasterGroup()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * RasterGroup::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Minerva::Interfaces::IAddLayer::IID:
    return static_cast<Minerva::Interfaces::IAddLayer* > ( this );
  case Minerva::Interfaces::IRemoveLayer::IID:
    return static_cast < Minerva::Interfaces::IRemoveLayer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
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
  USUL_ASSERT ( true == this->logGet().valid() );

  if ( 0x0 != layer )
  {
    Guard guard ( this );
    _layers.push_back ( layer );
    this->_updateExtents ( layer );

    // Clear the cache because some or all of the images are now incorrect.
    _cache.clear();

    // Set the log file.
    RasterLayer::RefPtr rl ( dynamic_cast < RasterLayer * > ( layer ) );
    if ( true == rl.valid() )
    {
      rl->logSet ( this->logGet() );
    }
    else
    {
      USUL_ASSERT ( 0 ); // FYI...
    }
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
    Layers::value_type raster ( *iter );
    if ( true == raster.valid() )
    {
      // Query for needed interfaces.
      Usul::Interfaces::ILayer::QueryPtr layer ( raster );
      Usul::Interfaces::ILayerExtents::QueryPtr le ( layer );
      
      // Get the extents.
      const double minLon ( le.valid() ? le->minLon() : -180.0 );
      const double minLat ( le.valid() ? le->minLat() :  -90.0 );
      const double maxLon ( le.valid() ? le->maxLon() :  180.0 );
      const double maxLat ( le.valid() ? le->maxLat() :   90.0 );
      
      Extents e ( minLon, minLat, maxLon, maxLat );
      
      // Should the layer be shown?
      const bool shown ( layer.valid() ? layer->showLayer() : true );
      
      if ( shown && extents.intersects ( e ) )
      {
        // Get the image for the layer.
        osg::ref_ptr < osg::Image > image ( raster->texture ( extents, width, height, level, job, caller ) );

        if ( image.valid() )
        {
          // Is this the first image?
          if ( false == result.valid() )
          {
            // We always make an image and composite to handle formats other than GL_RGBA.
            result = this->_createBlankImage ( width, height );
          }
          
          // See if it has raster alpha data.
          Usul::Interfaces::IRasterAlphas::QueryPtr ra ( layer );

          // Copy the alphas.
          Alphas alphas;
          float alpha ( 1.0f );
          if ( true == ra.valid() )
          {
            alphas = ra->alphas();
            alpha = ra->alpha();
          }

          // Composite the images.
          this->_compositeImages ( *result, *image, alphas, alpha, job );

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

void RasterGroup::_compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, float alpha, Usul::Jobs::Job *job )
{
  USUL_TRACE_SCOPE;

  // Composite.
  Minerva::Core::Algorithms::Composite::raster ( result, image, alphas, alpha, 1.0f, job );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raster layer.
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::ILayer* RasterGroup::layer ( unsigned int i )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( i < _layers.size() ) ? ILayer::QueryPtr ( _layers.at ( i ) ).get() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of raster layers.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int RasterGroup::size() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _layers.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a copy of the layers.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::layers ( Layers& layers ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  layers.resize ( _layers.size() );
  std::copy ( _layers.begin(), _layers.end(), layers.begin() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int RasterGroup::getNumChildNodes() const
{
  USUL_TRACE_SCOPE;
  return this->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child node (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode * RasterGroup::getChildNode ( unsigned int which )
{
  USUL_TRACE_SCOPE;
  return Usul::Interfaces::ITreeNode::QueryPtr ( this->layer ( which ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::setTreeNodeName ( const std::string & s )
{
  USUL_TRACE_SCOPE;
  this->name( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string RasterGroup::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return this->name();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer (IAddLayer).
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::addLayer ( Usul::Interfaces::ILayer *layer )
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
  if ( rl.valid() )
  {
    this->append ( rl.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer (IRemoveLayer).
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::removeLayer ( Usul::Interfaces::ILayer * layer )
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
  if ( rl.valid() )
  {
    this->remove ( rl.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the log.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::logSet ( LogPtr lp )
{
  USUL_TRACE_SCOPE;

  // Set our member.
  BaseClass::logSet ( lp );

  // Get copy of the layers.
  Layers layers ( Usul::Threads::Safe::get ( this->mutex(), _layers ) );

  // Loop through each layer and set its log.
  for ( Layers::iterator i = layers.begin(); i != layers.end(); ++i )
  {
    RasterLayer::RefPtr raster ( dynamic_cast < RasterLayer * > ( i->get() ) );
    if ( true == raster.valid() )
    {
      raster->logSet ( lp );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the log. Without this function this statement doesn't compile:
//  this->log()
//
///////////////////////////////////////////////////////////////////////////////

RasterGroup::LogPtr RasterGroup::logGet()
{
  USUL_TRACE_SCOPE;
  return BaseClass::logGet();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void RasterGroup::deserialize ( const XmlTree::Node &node )
{
  Guard guard ( this->mutex() );

  // Call the base class.
  BaseClass::deserialize ( node );

  // Clear the extents.
  this->extents ( Extents() );

  // Update the extents.
  for ( Layers::const_iterator iter = _layers.begin(); iter != _layers.end(); ++iter )
    this->_updateExtents ( (*iter).get() );
}
