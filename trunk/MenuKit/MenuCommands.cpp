
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

MenuCommands::MenuCommands()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands::MenuCommands ( const MenuCommands &i )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands::~MenuCommands()
{
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
//  Find a command with name <name>.
//
///////////////////////////////////////////////////////////////////////////////

MenuCommands::CommandPtr MenuCommands::find( const std::string& name )
{
  //Guard guard ( _mutex );
  return _commandMap[name];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a command.
//
///////////////////////////////////////////////////////////////////////////////

void MenuCommands::add( const std::string& name, CommandPtr command )
{
  //Guard guard ( _mutex );
  _commandMap[name] = command;
}