
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Encapsulates all exposed SQLite database functionality.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/SQLite/Connection.h"
#include "Database/SQLite/Internal.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"

#include "sqlite3.h"
#include "spatialite.h"

using namespace CadKit::Database::SQLite;

USUL_IMPLEMENT_TYPE_ID ( Connection );


///////////////////////////////////////////////////////////////////////////////
//
//  Small helper class to initialize and shutdown the sqlite library.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Life
  {
    Life()
    {
      ::sqlite3_initialize();
      ::spatialite_init();
    }
    ~Life()
    {
      ::sqlite3_shutdown();
    }
  } life;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Connection::Connection ( const std::string &file ) : BaseClass(),
  _file   ( file ),
  _db     ( 0x0 )
{
  // Open the database.
  {
    const int code ( ::sqlite3_open ( file.c_str(), &_db ) );
    if ( SQLITE_OK != code )
    {
      throw Usul::Exceptions::Exception ( Usul::Strings::format
        ( "Error 2047343298: Failed to open SQLite database.",
          " File: '", file, "', ",
          " Message: '", Helper::errorMessage ( _db ), "'" ) );
    }
  }

  // Register a busy handler.
  {
    const int code ( ::sqlite3_busy_handler ( _db, &Connection::_busyHandler, this ) );
    if ( SQLITE_OK != code )
    {
      throw Usul::Exceptions::Exception ( Usul::Strings::format
        ( "Error 5642419090: Failed to set busy-handler.",
          " File: '", file, "', ",
          " Message: '", Helper::errorMessage ( _db ), "'" ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Connection::~Connection()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Connection::_destroy ), "1236310543" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::_destroy()
{
  Guard guard ( this );

  if ( 0x0 != _db )
  {
    Usul::Functions::safeCallV1 ( ::sqlite3_close, _db, "6855261040" );
    _db = 0x0;
  }
  _file.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

Result::RefPtr Connection::execute ( const std::string &sql )
{
  return this->_execute ( sql, Binders() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

Result::RefPtr Connection::_execute ( const std::string &sql, const Binders &binders )
{
  Guard guard ( this );

  // Handle bad state.
  if ( 0x0 == _db )
    throw Usul::Exceptions::Exception ( "Error 3868552584: null database" );

  // Local scope.
  {
    // Lock the database. Using sqlite3_exec() as a guide.
    Helper::Guard dbGuard ( _db );

    // Initialize.
    int resultCode ( SQLITE_OK );
    ::sqlite3_stmt *statement ( 0x0 );
    const char *leftOver ( 0x0 );

    // Prepare the statement.
    resultCode = ::sqlite3_prepare_v2 ( _db, sql.c_str(), -1, &statement, &leftOver );
    if ( SQLITE_OK != resultCode )
    {
      throw Usul::Exceptions::Exception ( Usul::Strings::format
        ( "Error 1353976135: Result Code: ", resultCode, 
          ", Message: '", Helper::errorMessage ( _db ), "'",
          ", SQL: ", sql ) );
    }

    // Execute the binders.
    for ( unsigned int i = 0; i < binders.size(); ++i )
    {
      Binder::RefPtr binder ( binders.at ( i ) );
      if ( true == binder.valid() )
      {
        binder->bind ( sql, i, statement, _db );
      }
    }

    // Prepare the result.
    Result::RefPtr result ( new Result ( sql, _db, statement ) );

    // If the statement is not a select...
    if ( false == Helper::isSelectStatement ( sql ) )
    {
      // This will actually execute the statement. Since it's not a select 
      // statement the caller is likely to ignore the returned object.
      result->prepareNextRow();
      result->finalize();
    }

    // Return the result.
    return result;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Connection::file() const
{
  Guard guard ( this );
  return _file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The sqlite busy-handler. This gets called when there is a lock on the 
//  database because another thread or process is writing to it. We return 
//  non-zero to have sqlite continue to try.
//  See http://www.sqlite.org/c3ref/busy_handler.html
//
///////////////////////////////////////////////////////////////////////////////

int Connection::_busyHandler ( void *, int )
{
  std::cout << "Database is busy \n" << std::flush;
  return 1;
}
