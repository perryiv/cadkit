
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
//  Callbacks that determine tile splitting behavior.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/TileEngine/SplitCallbacks.h"
#include "Minerva/Core/TileEngine/Tile.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"

using namespace Minerva::Core::TileEngine::Callbacks;

USUL_FACTORY_REGISTER_CREATOR ( PassThrough );
USUL_FACTORY_REGISTER_CREATOR ( SplitToLevel );
USUL_FACTORY_REGISTER_CREATOR ( SplitIfLess );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SplitCallback::SplitCallback() : BaseClass(),
  SERIALIZE_XML_INITIALIZER_LIST
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SplitCallback::~SplitCallback()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PassThrough::PassThrough() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PassThrough::~PassThrough()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if the tile should split.
//
///////////////////////////////////////////////////////////////////////////////

bool PassThrough::shouldSplit ( bool suggestion, Tile * )
{
  return suggestion;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SplitToLevel::SplitToLevel ( unsigned int maxLevel ) : BaseClass(), 
  _maxLevel ( maxLevel )
{
  USUL_TRACE_SCOPE;

  // Serialization glue.
  this->_addMember ( "max_level", _maxLevel );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SplitToLevel::~SplitToLevel()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if the tile's level is less than the max.
//
///////////////////////////////////////////////////////////////////////////////

bool SplitToLevel::shouldSplit ( bool suggestion, Tile *tile )
{
  return ( ( 0x0 == tile ) ? suggestion : ( tile->level() < _maxLevel ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SplitIfLess::SplitIfLess ( unsigned int maxLevel ) : BaseClass(), 
  _maxLevel ( maxLevel )
{
  USUL_TRACE_SCOPE;

  // Serialization glue.
  this->_addMember ( "max_level", _maxLevel );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SplitIfLess::~SplitIfLess()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if the tile's level is less than the max.
//
///////////////////////////////////////////////////////////////////////////////

bool SplitIfLess::shouldSplit ( bool suggestion, Tile *tile )
{
  return ( ( 0x0 == tile ) ? suggestion : ( ( true == suggestion ) && ( tile->level() < _maxLevel ) ) );
}
