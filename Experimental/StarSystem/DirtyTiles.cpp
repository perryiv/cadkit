
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
//  Helper functor for making the tiles dirty.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/DirtyTiles.h"
#include "StarSystem/Tile.h"

#include "Usul/Trace/Trace.h"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DirtyTiles::DirtyTiles ( bool state, unsigned int flags ) :
  _state ( state ),
  _flags ( flags ),
  _extents(),
  _useExtents ( false )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DirtyTiles::DirtyTiles ( bool state, unsigned int flags, const Tile::Extents& extents ) :
  _state ( state ),
  _flags ( flags ),
  _extents ( extents ),
  _useExtents ( true )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

  DirtyTiles::DirtyTiles ( const DirtyTiles &d ) :
  _state ( d._state ),
  _flags ( d._flags ),
  _extents ( d._extents ),
  _useExtents ( d._useExtents )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called for every group. Need to determine if it's a tile.
//
///////////////////////////////////////////////////////////////////////////////

void DirtyTiles::operator () ( osg::Group *group )
{
  USUL_TRACE_SCOPE;
  Tile::RefPtr tile ( dynamic_cast < Tile * > ( group ) );
  if ( true == tile.valid() )
  {
    if ( true == _useExtents )
    {
      tile->dirty ( _state, _flags, false, _extents );
    }
    else
    {
      tile->dirty ( _state, _flags, false );
    }
  }
}
