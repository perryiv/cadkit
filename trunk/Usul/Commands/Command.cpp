
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all commands. A command is instantiated and executed 
//  inside of an event handler (action).
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Commands/Command.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Commands;

USUL_IMPLEMENT_TYPE_ID ( Command );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Command, Command::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::Command ( IUnknown *caller ) : BaseClass(),
  _caller ( caller ),
  _text(),
  _toolTip(),
  _statusTip(),
  _shortcut(),
  _iconPath()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::~Command()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Command::_destroy ), "3176019710" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Command::_destroy()
{
  USUL_TRACE_SCOPE;
  _caller = 0x0;
  _text.clear();
  _toolTip.clear();
  _statusTip.clear();
  _shortcut.clear();
  _iconPath.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Command::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  // No need to guard, should be re-entrant.

  switch ( iid )
  {
  case Usul::Interfaces::ICommand::IID:
    return static_cast<Usul::Interfaces::ICommand*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<Usul::Interfaces::ICommand*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Command::execute ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  // No need to guard, should be re-entrant.

  // If we don't already have a caller, and we are given a valid one, set our internal caller.
  if( false == _caller.valid() && 0x0 != caller )
    _caller = caller;
  
  // Execute the command.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Command::_execute ), "3084410573" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the caller.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::IUnknown *Command::caller() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _caller.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the caller.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Command::caller()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _caller.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void Command::text ( const std::string& value )
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	_text = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text.
//
///////////////////////////////////////////////////////////////////////////////

std::string Command::text() const
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return std::string ( _text.begin(), _text.end() ); // More thread-safe?
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Set the tooltip.
//
///////////////////////////////////////////////////////////////////////////////

void Command::toolTip ( const std::string& value )
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	_toolTip = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tooltip.
//
///////////////////////////////////////////////////////////////////////////////

std::string Command::toolTip() const
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return std::string ( _toolTip.begin(), _toolTip.end() ); // More thread-safe?
}
 

///////////////////////////////////////////////////////////////////////////////
//
//  Set the tooltip.
//
///////////////////////////////////////////////////////////////////////////////

void Command::statusTip ( const std::string& value )
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	_statusTip = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tooltip.
//
///////////////////////////////////////////////////////////////////////////////

std::string Command::statusTip() const
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return std::string ( _statusTip.begin(), _statusTip.end() ); // More thread-safe?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shortcut.
//
///////////////////////////////////////////////////////////////////////////////

void Command::shortcut ( const std::string& value )
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	_shortcut = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shortcut.
//
///////////////////////////////////////////////////////////////////////////////

std::string Command::shortcut() const
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return std::string ( _shortcut.begin(), _shortcut.end() ); // More thread-safe?
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon path.
//
///////////////////////////////////////////////////////////////////////////////

void Command::iconPath ( const std::string& value )
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	_iconPath = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon path.
//
///////////////////////////////////////////////////////////////////////////////

std::string Command::iconPath() const
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return std::string ( _iconPath.begin(), _iconPath.end() ); // More thread-safe?
}
