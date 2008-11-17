
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates all database functionality.
//
///////////////////////////////////////////////////////////////////////////////

#include "OracleWrap/Database.h"
#include "OracleWrap/Environment.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include "occi.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace OracleWrap;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Database::Database ( const std::string &database, const std::string &user, const std::string &password ) : BaseClass(),
  _connection ( 0x0 )
{
  try
  {
    _connection = Environment::instance().env()->createConnection ( user, password, database );
  }
  catch ( const oracle::occi::SQLException &e )
  {
    throw std::runtime_error ( Usul::Strings::format ( "Error 5737838080: Failed to create connection. ", ( ( 0x0 != e.what() ) ? e.what() : "" ) ) );
  }
  if ( 0x0 == _connection )
  {
    throw std::runtime_error ( "Error 2573165794: Failed to create connection" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Database::~Database()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Database::_destroy ), "9116646720" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Database::_destroy()
{
  if ( 0x0 != _connection )
  {
    try
    {
      Environment::instance().env()->terminateConnection ( _connection );
    }
    catch ( const oracle::occi::SQLException &e )
    {
      std::cout << "Error 1403031671: Failed to terminate connection. " << ( ( 0x0 != e.what() ) ? e.what() : "" ) << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

OracleWrap::Result::RefPtr Database::execute ( const std::string &sql )
{
  try
  {
    return this->_execute ( sql );
  }
  catch ( const oracle::occi::SQLException &e )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3257568799: Failed to execute statement. SQL: ", sql, ". ",
        ( ( 0x0 != e.what() ) ? e.what() : "" ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the string a select statement?
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool isSelectStatement ( const std::string &sql )
  {
    const std::string s ( Usul::Strings::lowerCase ( sql ) );
    if ( s.size() >= 6 )
    {
      return ( ( 's' == s[0] ) && 
               ( 'e' == s[1] ) && 
               ( 'l' == s[2] ) && 
               ( 'e' == s[3] ) && 
               ( 'c' == s[4] ) && 
               ( 't' == s[5] ) );
    }
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Terminate the statement.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void terminateStatement ( oracle::occi::Connection *c, oracle::occi::Statement *s )
  {
    if ( 0x0 != c )
    {
      if ( 0x0 != s )
      {
        c->terminateStatement ( s );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

OracleWrap::Result::RefPtr Database::_execute ( const std::string &sql )
{
  Guard guard ( this );

  // Empty string just means no result.
  if ( true == sql.empty() )
    return Result::RefPtr ( new Result ( 0x0, 0x0, 0x0, this->mutex() ) );

  // Check state.
  if ( 0x0 == _connection )
    throw std::runtime_error ( "Error 3741173076: Null connection pointer" );

  // Create the statement.
  oracle::occi::Statement *statement ( _connection->createStatement ( sql ) );
  if ( 0x0 == statement )
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3577957947: Failed to create statement. SQL: ", sql ) );

  // Initialize the result set.
  oracle::occi::ResultSet *resultSet ( 0x0 );

  // Execute the statement.
  if ( true == Helper::isSelectStatement ( sql ) )
  {
    resultSet = statement->executeQuery();
  }
  else
  {
    statement->executeUpdate();
  }

  // Return the result. We pass the mutex so that no further executions 
  // can happen until the result is deleted.
  return Result::RefPtr ( new Result ( _connection, statement, resultSet, this->mutex() ) );

#if 0
  -- Rename Database to Connection.
  -- Pass "Connection::RefPtr ( this )", in addition to the statement and result set, to the Result, which will hang on to it.
  -- No need to pass the mutex. Once the result is acquired and returned the same or any other thread can enter this function.
  -- Result::~Result can destroy the result-set and ask the connection to destroy the statement.
  -- Trap Windows structured exceptions here and re-throw as regular exceptions.
#endif
}
