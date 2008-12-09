
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

#include "Database/Oracle/Connection.h"
#include "Database/Oracle/Environment.h"
#include "Database/Oracle/Result.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "occi.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace CadKit::Database::Oracle;

USUL_IMPLEMENT_TYPE_ID ( Connection );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Connection::Connection ( const std::string &database, const std::string &user, const std::string &password ) : BaseClass(),
  _connection ( 0x0 ),
  _executing ( 0x0 ),
  _numPreFetchRows ( 10 )
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Connection::_destroy ), "9116646720" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_destroy()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _connection )
  {
    USUL_TRY_BLOCK
    {
      // Always set to null.
      oracle::occi::Statement *s ( _executing );
      oracle::occi::Connection *c ( _connection );
      _executing = 0x0;
      _connection = 0x0;

      // First stop the executing statement if it exists.
      if ( 0x0 != s )
      {
        c->terminateStatement ( s );
      }

      // Terminate connection on client side.
      Environment::instance().env()->terminateConnection ( c );
    }
    catch ( const oracle::occi::SQLException &e )
    {
      std::cout << "Error 1403031671: Failed to terminate connection. " << ( ( 0x0 != e.what() ) ? e.what() : "" ) << std::endl;
    }
    USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2924598246" )
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Terminate the statement.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_terminateStatement ( oracle::occi::Statement *s )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != s )
  {
    oracle::occi::Connection *c ( Usul::Threads::Safe::get ( this->mutex(), _connection ) );
    if ( 0x0 != c )
    {
      c->terminateStatement ( s );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the string a select statement?
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool isSelectStatement ( const std::string &sql )
  {
    USUL_TRACE_SCOPE_STATIC;

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

Result *Connection::execute ( const std::string &sql, unsigned int numRowsToPreFetch )
{
  USUL_TRACE_SCOPE;

  USUL_TRY_BLOCK
  {
    return this->_execute ( sql, numRowsToPreFetch );
  }
  catch ( const oracle::occi::SQLException &e )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3257568799: Failed to execute statement. ",
        ( ( 0x0 != e.what() ) ? e.what() : "" ),
        ", SQL = ", sql ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

Result *Connection::_execute ( const std::string &sql, unsigned int numRowsToPreFetch )
{
  USUL_TRACE_SCOPE;

  // Empty string just means no result.
  if ( true == sql.empty() )
    return new Result ( 0x0, 0x0, 0x0 );

  // Check state.
  oracle::occi::Connection *c ( Usul::Threads::Safe::get ( this->mutex(), _connection ) );
  if ( 0x0 == c )
    throw std::runtime_error ( "Error 3741173076: Null connection pointer" );

  // Create the statement.
  oracle::occi::Statement *statement ( c->createStatement ( sql ) );
  if ( 0x0 == statement )
    throw std::runtime_error ( "Error 3577957947: Failed to create statement" );

  // Set the number of rows to bring back at once.
  statement->setPrefetchRowCount ( numRowsToPreFetch );

  // Return the result.
  const bool select ( true == Helper::isSelectStatement ( sql ) );
  return this->_execute ( c, statement, select );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given statement.
//
///////////////////////////////////////////////////////////////////////////////

Result *Connection::_execute ( oracle::occi::Connection *c, oracle::occi::Statement *s, bool select )
{
  USUL_TRACE_SCOPE;

  // Null input means no result, not an error.
  if ( ( 0x0 == c ) || ( 0x0 == s ) )
    return new Result ( 0x0, 0x0, 0x0 );

  // This will translate Windows structured exceptions into C++ exceptions.
  USUL_TRY_BLOCK
  {
    // Always unset after query.
    Usul::Scope::Caller::RefPtr reset ( Usul::Scope::makeCaller 
      ( Usul::Adaptors::bind1 ( static_cast < oracle::occi::Statement * > ( 0x0 ), 
      Usul::Adaptors::memberFunction ( this, &Connection::_setExecutingStatement ) ) ) );

    // Set member now.
    this->_setExecutingStatement ( s );

    // Execute the statement.
    if ( true == select )
    {
      oracle::occi::ResultSet *r ( s->executeQuery() );
      return new Result ( this, s, r );
    }
    else
    {
      s->executeUpdate();
      return new Result ( this, s, 0x0 );
    }
  }
  catch ( ... )
  {
    throw;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Roll back any changes made since previous commit.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::rollback()
{
  USUL_TRACE_SCOPE;

  oracle::occi::Connection *c ( Usul::Threads::Safe::get ( this->mutex(), _connection ) );
  if ( 0x0 != c )
  {
    c->rollback();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Commit any changes made since previous commit.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::commit()
{
  USUL_TRACE_SCOPE;

  oracle::occi::Connection *c ( Usul::Threads::Safe::get ( this->mutex(), _connection ) );
  if ( 0x0 != c )
  {
    c->commit();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel any running queries for this connection.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::sendCancelRequestToServer()
{
  USUL_TRACE_SCOPE;

  oracle::occi::Connection *c ( Usul::Threads::Safe::get ( this->mutex(), _connection ) );
  if ( 0x0 == c )
    return;

  // Cancel the server side.
  OCISvcCtx *server ( c->getOCIServiceContext() );
  if ( 0x0 == server )
    return;

  // See http://www.orafaq.com/forum/t/52133/0/
  // See http://forums.oracle.com/forums/thread.jspa?threadID=192292
  OCIError *errorCallback ( static_cast < OCIError * > ( 0x0 ) );
  Usul::Functions::safeCallV1V2 ( ::OCIBreak, server, errorCallback, "2257760186" );
  Usul::Functions::safeCallV1V2 ( ::OCIReset, server, errorCallback, "3100516377" );
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Cancel a running statement. This has no effect if there is no running 
//  statement. This is meant to be safe to call from any thread.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::cancelRunningStatement()
{
  USUL_TRACE_SCOPE;

  oracle::occi::Connection *c ( 0x0 );
  oracle::occi::Statement *s ( 0x0 );
  {
    Guard guard ( this );
    c = _connection;
    s = _executing;
  }

  if ( ( 0x0 == c ) || ( 0x0 == s ) )
    return;

  USUL_TRY_BLOCK
  {
    c->terminateStatement ( s );
  }
  catch ( const oracle::occi::SQLException &e )
  {
    throw std::runtime_error ( Usul::Strings::format ( 
      "Error 3905936610: Failed to terminate statement. ",
      ( ( 0x0 != e.what() ) ? e.what() : "" ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the member that indicates there is a running query.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_setExecutingStatement ( oracle::occi::Statement *s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _executing = s;
}
