
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu MenuCommandss.
//
///////////////////////////////////////////////////////////////////////////////

#include "MenuCommands.h"
#include "Visitor.h"
#include "Errors.h"

#include "Usul/Bits/Bits.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace MenuKit;



///////////////////////////////////////////////////////////////////////////////
//
//  Static instance.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands *MenuCommands::_instance ( 0x0 );

///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands::MenuCommands() : 
  _commandMap(),
  _mutex ( Mutex::create() )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands::~MenuCommands()
{
  delete _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands& MenuCommands::instance()
{
  if ( 0x0 == _instance )
    _instance = new MenuCommands;
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the internal map.
//
///////////////////////////////////////////////////////////////////////////////

void MenuCommands::clear()
{
  Guard guard ( this->mutex() );
  _commandMap.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find a command with name <name>.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands::CommandPtr MenuCommands::find ( const std::string& name ) const
{
  Guard guard ( this->mutex() );
  Commands::const_iterator i ( _commandMap.find ( name ) );
  return ( ( _commandMap.end() == i ) ? CommandPtr ( 0x0 ) : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a command.
//
///////////////////////////////////////////////////////////////////////////////

void MenuCommands::add ( const std::string& name, CommandPtr command )
{
  Guard guard ( this->mutex() );
  _commandMap[name] = command;
}
