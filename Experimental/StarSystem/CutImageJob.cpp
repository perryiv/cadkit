
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

#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace StarSystem;

USUL_IMPLEMENT_TYPE_ID ( CutImageJob );


///////////////////////////////////////////////////////////////////////////////
//
//  Job to cut the texture to correct extents.
//
///////////////////////////////////////////////////////////////////////////////

CutImageJob::CutImageJob ( Tile::RefPtr tile ) : 
  BaseClass ( 0x0, false ),
  _tile ( tile )
{
  USUL_TRACE_SCOPE;
  
  if ( _tile.valid() )
  {
    const unsigned int level ( _tile->level() );
    this->priority ( -1 * static_cast<int> ( level ) );
  
    Tile::Extents extents ( _tile->extents() );
    this->name ( Usul::Strings::format ( "Extents: [", extents.minimum()[0], ", ", extents.minimum()[1], ", ", extents.maximum()[0], ", ", extents.maximum()[1], "], level: ", level ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CutImageJob::~CutImageJob()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the texture.
//
///////////////////////////////////////////////////////////////////////////////

void CutImageJob::_started()
{
  USUL_TRACE_SCOPE;

  // Make sure we have valid data.
  if ( false == _tile.valid() )
    return;
  
  // Have we been cancelled?
  if ( true == this->canceled() )
    this->cancel();
  
  // Ask the tile to split.
  _tile->buildRaster ( Usul::Jobs::Job::RefPtr ( this ) );
}
