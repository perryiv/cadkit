
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
  this->priority ( level );
  Usul::Pointers::reference ( _raster );
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
  Guard guard ( this );

  // Have we been cancelled?
  if ( true == this->canceled() )
    this->cancel();

  // Handle no raster layer.
  if ( 0x0 == _raster )
    return;

  // Get the image.
  const unsigned int width  ( 256 );
  const unsigned int height ( 256 );

  // Request the image.
  _image = _raster->texture ( _extents, width, height, _level );

  // Create the texture.
  _texture = new osg::Texture2D;
  _texture->setImage( _image.get() );

  _texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
  _texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
  _texture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
  _texture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
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
