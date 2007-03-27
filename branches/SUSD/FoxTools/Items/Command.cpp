
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base command class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Items/Command.h"

using namespace FoxTools;
using namespace FoxTools::Items;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::Command ( unsigned int token ) : BaseClass(),
  _token       ( token ),
  _hotKeys     (),
  _description (),
  _target      ( 0x0 ),
  _selector    ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::Command ( const std::string &name, 
                   FX::FXObject *target, 
                   unsigned int selector, 
                   unsigned int token ) : 
  BaseClass ( name ),
  _token       ( token ),
  _hotKeys     (),
  _description (),
  _target      ( target ),
  _selector    ( selector )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::Command ( const std::string &name, 
                   const std::string &hotKeys, 
                   const std::string &description, 
                   FX::FXObject *target, 
                   unsigned int selector, 
                   unsigned int token ) : BaseClass ( name ),
  _token       ( token ),
  _hotKeys     ( hotKeys ),
  _description ( description ),
  _target      ( target ),
  _selector    ( selector )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::~Command()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot keys.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Command::hotKeys() const
{
  return _hotKeys;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hot keys.
//
///////////////////////////////////////////////////////////////////////////////

void Command::hotKeys ( const std::string &hot )
{
  _hotKeys = hot;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the description.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Command::description() const
{
  return _description;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the description.
//
///////////////////////////////////////////////////////////////////////////////

void Command::description ( const std::string &d )
{
  _description = d;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the target.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXObject *Command::target()
{
  return _target;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the target.
//
///////////////////////////////////////////////////////////////////////////////

const FX::FXObject *Command::target() const
{
  return _target;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the target.
//
///////////////////////////////////////////////////////////////////////////////

void Command::target ( FX::FXObject *t )
{
  _target = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the selector.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Command::selector() const
{
  return _selector;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the selector.
//
///////////////////////////////////////////////////////////////////////////////

void Command::selector ( unsigned int s )
{
  _selector = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the token.
//
///////////////////////////////////////////////////////////////////////////////

void Command::token ( unsigned int t )
{
  _token = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the token.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Command::token() const
{
  return _token;
}
