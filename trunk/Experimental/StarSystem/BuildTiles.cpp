
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

#include "StarSystem/BuildTiles.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Trace/Trace.h"

using namespace StarSystem;

USUL_IMPLEMENT_TYPE_ID ( BuildTiles );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BuildTiles::BuildTiles ( Tile::RefPtr tile ) : BaseClass(),
  _tile ( tile )
{
  USUL_TRACE_SCOPE;

  if ( _tile.valid() )
  {
    this->priority ( -1 * static_cast<int> ( _tile->level() ) );
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
