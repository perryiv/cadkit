
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to fetch an image.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/CutImageJob.h"
#include "StarSystem/RasterLayer.h"

#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Job to cut the texture to correct extents.
//
///////////////////////////////////////////////////////////////////////////////

CutImageJob::CutImageJob ( const Extents &extents, unsigned int level, RasterLayer *raster ) : 
  BaseClass ( 0x0, false ),
  _extents ( extents ),
  _level   ( level ),
  _raster  ( raster ),
  _image   ( 0x0 ),
  _texture ( 0x0 )
{
  USUL_TRACE_SCOPE;
  this->priority ( -1 * static_cast<int> ( level ) );
  Usul::Pointers::reference ( _raster );
  this->name ( Usul::Strings::format ( "Extents: [", extents.minimum()[0], ", ", extents.minimum()[1], ", ", extents.maximum()[0], ", ", extents.maximum()[1], "], level: ", level ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CutImageJob::~CutImageJob()
{
  USUL_TRACE_SCOPE;
  Usul::Pointers::unreference ( _raster );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the texture.
//
///////////////////////////////////////////////////////////////////////////////

void CutImageJob::_started()
{
  USUL_TRACE_SCOPE;

  // Have we been cancelled?
  if ( true == this->canceled() )
    this->cancel();

  // Handle no raster layer.
  RasterLayer::RefPtr raster ( Usul::Threads::Safe::get ( this->mutex(), _raster ) );
  if ( false == raster.valid() )
    return;

  // Get the image.
  const unsigned int width  ( 512 ); // WMS server doesn't like 256...
  const unsigned int height ( 512 );

  // Request the image.
  osg::ref_ptr<osg::Image> image ( raster->texture ( _extents, width, height, _level, this ) );

  // Have we been cancelled?
  if ( true == this->canceled() )
    this->cancel();

  // See if there is an image.
  if ( false == image.valid() )
    return;

  // Create the texture.
  osg::ref_ptr<osg::Texture2D> texture ( new osg::Texture2D );

  // Set the texture's state.
  texture->setImage ( image.get() );
  texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
  texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
  texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
  texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );

  // Have we been cancelled?
  if ( true == this->canceled() )
    this->cancel();

  // Set our data members.
  {
    Guard guard ( this );
    _image = image;
    _texture = texture;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image *CutImageJob::image()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _image.get();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Return the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Texture2D *CutImageJob::texture()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _texture.get();
}
