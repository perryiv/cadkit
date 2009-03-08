
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for executing oracle sql commands.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/Oracle/Execute.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"

#include <fstream>
#include <limits>

using namespace CadKit::Database::Oracle;


///////////////////////////////////////////////////////////////////////////////
//
//  Constants used in this file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Constants
{
  const std::string DATABASE_URL        ( "database_url" );
  const std::string DATABASE_USER       ( "database_user" );
  const std::string DATABASE_PASSWORD   ( "database_password" );
  const std::string SQL_COMMAND         ( "sql_command" );
  const std::string OUTPUT_FILE         ( "output_file" );
  const std::string ERROR_FILE          ( "error_file" );
  const std::string NUM_ROWS_PREFETCH   ( "num_rows_prefetch" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Execute::Execute ( int argc, char **argv ) :
  _input(),
  _commands(),
  _streams()
{
  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Initialize input with empty strings.
  _input[Constants::DATABASE_URL] = "";
  _input[Constants::DATABASE_USER] = "";
  _input[Constants::DATABASE_PASSWORD] = "";
  _input[Constants::SQL_COMMAND] = "";
  _input[Constants::ERROR_FILE] = "";
  _input[Constants::OUTPUT_FILE] = "";
  _input[Constants::NUM_ROWS_PREFETCH] = "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Execute::~Execute()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the stream.
//
///////////////////////////////////////////////////////////////////////////////

std::ostream &Execute::_getStream ( const std::string &name )
{
  OutputStreamPtr stream ( _streams[name] );
  if ( 0x0 == stream.get() )
  {
    const std::string file ( _input[name] );
    if ( false == file.empty() )
    {
      OutputStreamPtr out ( new std::ofstream ( file.c_str() ) );
      if ( true == out->is_open() )
      {
        _streams[name] = out;
        stream = out;
      }
    }
  }
  return ( ( 0x0 == stream.get() ) ? std::cout : *stream );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rows to prefetch.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Execute::_getNumRowsToPrefetch() const
{
  Input::const_iterator i ( _input.find ( Constants::NUM_ROWS_PREFETCH ) );
  const unsigned int num ( ( _input.end() == i ) ? 1000 : Usul::Convert::Type<std::string,unsigned int>::convert ( i->second ) );
  return Usul::Math::maximum ( num, static_cast < unsigned int > ( std::numeric_limits<unsigned short>::max() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Put the command-line arguments in a map.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::_processCommandLine()
{
  // Copy command-line arguments to the map.
  typedef Usul::CommandLine::Arguments Arguments;
  typedef Arguments::Args Args;
  typedef std::vector<std::string> StringList;
  Args args ( Arguments::instance().args() );

  // If there are not enough arguments...
  if ( args.size() < 2 )
  {
    throw Usage();
  }

  // The first argument is the program name.
  args.erase ( args.begin() );

  // Loop through command-line arguments.
  for ( Args::const_iterator a = args.begin(); a != args.end(); ++a )
  {
    const std::string arg ( *a );
    std::string::const_iterator c ( std::find ( arg.begin(), arg.end(), '=' ) );

    // If there is a delimiter...
    if ( arg.end() != c )
    {
      const std::string name  ( arg.begin(), c );
      const std::string value ( ++c, arg.end() );

      if ( ( false == name.empty() ) && ( false == value.empty() ) )
      {
        // The sql commands go in a separate container.
        if ( Constants::SQL_COMMAND == name )
        {
          _commands.push_back ( value );
        }

        // Other flags get replaced if they are already there.
        else
        {
          _input[name] = value;
        }
      }
      else
      {
        throw std::invalid_argument ( Usul::Strings::format 
          ( "Error 2163851239: Argument did not split into name=value pair:\n", arg ) );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the input.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::_checkInput()
{
  // Get the database name.
  const std::string database ( _input[Constants::DATABASE_URL] );
  if ( true == database.empty() )
  {
    throw Usul::Exceptions::Exception ( "Error 4187684402: No database URL given" );
  }

  // Get the database user.
  const std::string user ( _input[Constants::DATABASE_USER] );
  if ( true == user.empty() )
  {
    throw Usul::Exceptions::Exception ( "Error 3764694510: No database user given" );
  }

  // Get the database password.
  const std::string password ( _input[Constants::DATABASE_PASSWORD] );
  if ( true == password.empty() )
  {
    throw Usul::Exceptions::Exception ( "Error 3924495829: No database password given" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the commands.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::_executeCommands()
{
  // Get connection data.
  const std::string database   ( _input[Constants::DATABASE_URL] );
  const std::string user       ( _input[Constants::DATABASE_USER] );
  const std::string password   ( _input[Constants::DATABASE_PASSWORD] );

  // Make the connection.
  Connection::RefPtr connection ( new Connection ( database, user, password ) );
  if ( false == connection.valid() )
    throw Usul::Exceptions::Exception ( Usul::Strings::format 
      ( "Error 2232730731: Failed to connect to database '", database, "'" ) );

  // Use the correct stream.
  std::ostream &out ( this->_getStream ( Constants::OUTPUT_FILE ) );

  // Need local scope because the connection has to live longer than the roll-back object.
  {
    // Make sure we always roll back.
    Usul::Scope::Caller::RefPtr rollback ( Usul::Scope::makeCaller 
      ( Usul::Adaptors::memberFunction ( connection.get(), &Connection::rollback ) ) );

    // Execute the commands.
    this->_executeCommands ( connection, out );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the commands.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::_executeCommands ( Connection::RefPtr connection, std::ostream &out )
{
  // Check input.
  if ( false == connection.valid() )
    throw Usul::Exceptions::Exception ( "Error 4281592813: Invalid connection given" );

  // Loop through the commands.
  for ( Commands::const_iterator i = _commands.begin(); i != _commands.end(); ++i )
  {
    const std::string command ( *i );
    Result::RefPtr result ( this->_executeCommand ( connection, command ) );
    this->_writeResult ( result, out );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command.
//
///////////////////////////////////////////////////////////////////////////////

Execute::Result::RefPtr Execute::_executeCommand ( Connection::RefPtr connection, const std::string &sql )
{
  // Check input.
  if ( false == connection.valid() )
    throw Usul::Exceptions::Exception ( "Error 4640977390: Invalid connection given" );

  // Handle empty commands.
  if ( true == sql.empty() )
    return Result::RefPtr ( 0x0 );

  // Get the number of rows to pre-fetch.
  const unsigned int numRowsToPreFetch ( this->_getNumRowsToPrefetch() );

  // Execute the command.
  Result::RefPtr result ( connection->execute ( sql, numRowsToPreFetch ) );

  // Return the result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the error.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::writeError ( const std::string &s )
{
  std::ostream &out ( this->_getStream ( Constants::ERROR_FILE ) );
  out << s << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the usage string.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::writeUsage() const
{
  std::cout << Usul::CommandLine::Arguments::instance().program() << ' ';
  for ( Input::const_iterator i = _input.begin(); i != _input.end(); ++i )
  {
    std::cout << i->first << "=<value> ";
  }
  std::cout << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the result.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::_writeResult ( Result::RefPtr result, std::ostream &out )
{
  // Handle no result.
  if ( false == result.valid() )
    return;

  // Get the number of columns.
  const unsigned int numColumns ( result->numColumns() );
  if ( 0 == numColumns )
    return;

  // Loop until there are no more rows.
  while ( result->prepareNextRow() )
  {
    for ( unsigned int i = 0; i < numColumns; ++i )
    {
      std::string value;
      *result >> value;
      out << value << ' ';
    }
    out << '\n';
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Execute::run()
{
  this->_processCommandLine();
  this->_checkInput();
  this->_executeCommands();
}
