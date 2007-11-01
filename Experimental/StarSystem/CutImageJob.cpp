
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

CutImageJob::CutImageJob ( Tile* tile, RasterLayer *raster ) : 
  BaseClass ( 0x0, false ),
  _tile    ( tile ),
  _raster  ( raster )
{
  USUL_TRACE_SCOPE;

  if ( _tile.valid () )
    this->priority ( _tile->level() * -1 );

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
  if ( _tile.valid() )
    _tile->image ( _raster->texture ( _tile->extents(), width, height, _tile->level() )  );

  // Set the new starting texture coordinates.
  if ( _tile.valid() )
    _tile->texCoords ( Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ) );
}
