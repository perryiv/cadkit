
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Build four sub-tiles.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Jobs/BuildTiles.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Trace/Trace.h"

using namespace Minerva::Core::Jobs;

USUL_IMPLEMENT_TYPE_ID ( BuildTiles );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BuildTiles::BuildTiles ( Tile::RefPtr tile ) : BaseClass ( 0x0, false ),
  _tile ( tile )
{
  USUL_TRACE_SCOPE;

  if ( _tile.valid() )
  {
    const unsigned int level ( _tile->level() );
    this->priority ( -1 * static_cast<int> ( level ) );
  
    const Tile::Extents extents ( _tile->extents() );
    this->name ( Usul::Strings::format ( "BuildTiles, Extents: [", extents.minimum()[0], ", ", extents.minimum()[1], ", ", extents.maximum()[0], ", ", extents.maximum()[1], "], level: ", level ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BuildTiles::~BuildTiles()
{
  USUL_TRACE_SCOPE;
  _tile = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the sub-tiles.
//
///////////////////////////////////////////////////////////////////////////////

void BuildTiles::_started()
{
  USUL_TRACE_SCOPE;
  
  // Make sure we have valid data.
  if ( false == _tile.valid() )
    return;
  
  // Have we been cancelled?
  if ( true == this->canceled() )
    this->cancel();

  // Ask the tile to split.
  _tile->split ( Usul::Jobs::Job::RefPtr ( this ) );
}
