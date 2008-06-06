
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command history.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Commands/History.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ICanRedo.h"
#include "Usul/Interfaces/ICanUndo.h"
#include "Usul/Interfaces/IRedo.h"
#include "Usul/Interfaces/IUndo.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Commands;

USUL_IMPLEMENT_TYPE_ID ( History );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

History::History() : BaseClass(),
  _done(),
  _undone()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

History::~History()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &History::_destroy ), "5159896700" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to clear the stack.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void clear ( History::CommandStack &s )
  {
    USUL_TRACE_SCOPE_STATIC;
    while ( false == s.empty() ) 
    {
      s.pop();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the stack.
//
///////////////////////////////////////////////////////////////////////////////

void History::_destroy()
{
  USUL_TRACE_SCOPE;
  Helper::clear ( _done );
  Helper::clear ( _undone );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the recently executed command.
//
///////////////////////////////////////////////////////////////////////////////

void History::add ( ICommand *command )
{
  if ( 0x0 != command )
  {
	  Guard guard ( this->mutex() );
    _done.push ( ICommand::RefPtr ( command ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can the command be redone?
//
///////////////////////////////////////////////////////////////////////////////

bool History::canRedo() const
{
  Guard guard ( this->mutex() );
	
	Usul::Interfaces::IUnknown::QueryPtr unknown ( ( false == _undone.empty() ) ? _undone.top().get() : 0x0 ); 
  Usul::Interfaces::ICanRedo::QueryPtr redo ( unknown.get () );
  return ( ( true == redo.valid() ) ? redo->canRedo() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can the command be undone?
//
///////////////////////////////////////////////////////////////////////////////

bool History::canUndo() const
{
	Guard guard ( this->mutex() );
	
	Usul::Interfaces::IUnknown::QueryPtr unknown ( ( false == _done.empty() ) ? _done.top().get() : 0x0 );
  Usul::Interfaces::ICanUndo::QueryPtr undo ( unknown.get () );
  return ( ( true == undo.valid() ) ? undo->canUndo() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the history.
//
///////////////////////////////////////////////////////////////////////////////

void History::clear()
{
	Guard guard ( this->mutex() );
  Helper::clear ( _done );
  Helper::clear ( _undone );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Redo the command that was last undone.
//
///////////////////////////////////////////////////////////////////////////////

void History::redo()
{
  if ( true == this->canRedo() )
  {
    // Move the command over to the "done" stack before we execute.
    Usul::Interfaces::IRedo::QueryPtr command;
    {
	    Guard guard ( this->mutex() );
      command = _undone.top();
      _undone.pop();
    }
    if ( true == command.valid() )
    {
      {
	      Guard guard ( this->mutex() );
        _done.push ( ICommand::QueryPtr ( command.get() ) );
      }

      // Redo the command. This may throw.
      command->redo();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Undo the last command.
//
///////////////////////////////////////////////////////////////////////////////

void History::undo()
{
  if ( true == this->canUndo() )
  {
    // Move the command over to the "undone" stack before we execute.
    Usul::Interfaces::IUndo::QueryPtr command;
    {
	    Guard guard ( this->mutex() );
      command = _done.top();
      _done.pop();
    }
    if ( true == command.valid() )
    {
      {
	      Guard guard ( this->mutex() );
        _undone.push ( ICommand::QueryPtr ( command.get() ) );
      }

      // Undo the command. This may throw.
      command->undo();
    }
  }
}
