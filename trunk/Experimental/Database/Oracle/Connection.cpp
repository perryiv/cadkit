
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
//  Encapsulates all database connectivity.
//
///////////////////////////////////////////////////////////////////////////////

#include "OracleWrap/Connection.h"
#include "OracleWrap/Environment.h"
#include "OracleWrap/Result.h"

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

using namespace CadKit::Databases::Oracle;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Connection::Connection ( const std::string &database, const std::string &user, const std::string &password ) : BaseClass(),
  _connection ( 0x0 ),
  _numPreFetchRows ( 10 )
{
  USUL_TRY_BLOCK
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

Connection::~Connection()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Connection::_destroy ), "9116646720" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_destroy()
{
  if ( 0x0 != _connection )
  {
    USUL_TRY_BLOCK
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
//  Terminate the statement.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_terminateStatement ( oracle::occi::Statement *s )
{
  Guard guard ( this );

  if ( ( 0x0 != _connection ) && ( 0x0 != s ) )
  {
    _connection->terminateStatement ( s );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

Result *Connection::execute ( const std::string &sql, unsigned int numRowsToPreFetch )
{
  USUL_TRY_BLOCK
  {
    return this->_execute ( sql, numRowsToPreFetch );
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
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

Result *Connection::_execute ( const std::string &sql, unsigned int numRowsToPreFetch )
{
  Guard guard ( this );

  // Empty string just means no result.
  if ( true == sql.empty() )
    return new Result ( 0x0, 0x0, 0x0 );

  // Check state.
  if ( 0x0 == _connection )
    throw std::runtime_error ( "Error 3741173076: Null connection pointer" );

  // Create the statement.
  oracle::occi::Statement *statement ( _connection->createStatement ( sql ) );
  if ( 0x0 == statement )
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3577957947: Failed to create statement. SQL: ", sql ) );

  statement->setPrefetchRowCount ( numRowsToPreFetch );

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
  return new Result ( this, statement, resultSet );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default number of rows to fetch at a time from the server.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::numPreFetchRows ( unsigned int num )
{
  Guard guard ( this );
  _numPreFetchRows = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default number of rows to fetch at a time from the server.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Connection::numPreFetchRows() const
{
  Guard guard ( this );
  return _numPreFetchRows;
}
