
#include <functional>


///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group of commands.
//
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>

#include "Helios/Qt/Commands/Group.h"

#include "MenuKit/MenuCommands.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/trim.hpp"

#include <algorithm>
#include <fstream>
#include <functional>
#include <list>
#include <vector>

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( Group );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group ( Usul::Interfaces::IUnknown *caller ) : BaseClass ( caller ),
  _commands()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group ( const Group & c ) : BaseClass ( c ),
  _commands ( c._commands )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Group& Group::operator = ( const Group & c )
{
  USUL_TRACE_SCOPE;
  BaseClass::operator = ( c );
  Usul::Threads::Safe::set ( c.mutex(), c._commands, _commands );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Group::_destroy ), "1123964160" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_destroy()
{
  USUL_TRACE_SCOPE;
  _commands.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_execute()
{
  USUL_TRACE_SCOPE;
  CommandList commands ( Usul::Threads::Safe::get ( this->mutex(), _commands ) );
  std::for_each ( commands.begin(), commands.end(), std::bind2nd ( std::mem_fun ( &BaseClass::execute ), this->caller() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group empty?
//
///////////////////////////////////////////////////////////////////////////////

bool Group::empty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _commands.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the macro from file.
//
///////////////////////////////////////////////////////////////////////////////

void Group::readMacro ( const std::string &file )
{
  USUL_TRACE_SCOPE;

  // Open the file.
  std::ifstream in ( file.c_str() );
  if ( false == in.is_open() )
    return;

  // Used in the loop.
  typedef std::vector<char> Chars;
  Chars buffer ( 1024 );

  // Used in the loop.
  typedef std::vector<std::string> Strings;
  Strings parts;

  // Used in the loop.
  CommandList commands;

  // The first line is the name of this command.
  {
    std::fill ( buffer.begin(), buffer.end(), '\0' );
    in.getline ( &buffer[0], buffer.size() - 1 );
    const std::string name ( boost::trim_copy ( std::string ( &buffer[0] ) ) );
    if ( true == name.empty() )
      return;
    this->text ( name );
    this->name ( name );
  }

  // Read each line.
  while ( EOF != in.peek() )
  {
    // Get the line.
    std::fill ( buffer.begin(), buffer.end(), '\0' );
    in.getline ( &buffer[0], buffer.size() - 1 );
    std::string line ( boost::trim_copy ( std::string ( &buffer[0] ) ) );

    // If we have a word...
    if ( false == line.empty() )
    {
      // Make sure there is a leading '|' character.
      if ( '|' != line.at ( 0 ) )
      {
        line.insert ( line.begin(), '|' );
      }

      // Trim white space around the pipes.
      boost::replace_all ( line, " |", "|" );
      boost::replace_all ( line, "| ", "|" );

      // Is there a command?
      Usul::Commands::Command::RefPtr command ( MenuKit::MenuCommands::instance().find ( line ) );
      if ( true == command.valid() )
      {
        // Add the command.
        commands.push_back ( command );
      }
    }
  }

  // Replace the commands.
  Usul::Threads::Safe::set ( this->mutex(), commands, _commands );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return copy of commands.
//
///////////////////////////////////////////////////////////////////////////////

Group::CommandList Group::commands() const
{
  USUL_TRACE_SCOPE;
  CommandList commands ( Usul::Threads::Safe::get ( this->mutex(), _commands ) );
  return commands;
}
