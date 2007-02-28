
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
//  Encapsulates all exposed SQLite database functionality.
//
///////////////////////////////////////////////////////////////////////////////

#include "SQLiteWrap/Database.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Functions/SafeCall.h"

#include "sqlite3.h"

using namespace SQLiteWrap;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Database::Database ( const std::string &file ) : BaseClass(),
  _file   ( file ),
  _db     ( 0x0 ),
  _mutex  ( Usul::Threads::Mutex::create() ),
  _sql    (),
  _cb     ( 0x0 )
{
  const int code ( ::sqlite3_open ( file.c_str(), &_db ) );
  if ( SQLITE_OK != code )
  {
    this->_close();
    throw Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 2047343298: Failed to open SQLite database.",
        "\n  File: ", file,
        "\n  Message: ", ::sqlite3_errmsg ( _db ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Database::~Database()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Database::_close ), "2313175981" );
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Database::_clean ), "1236310543" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Database::_clean()
{
  _file.clear();
  _db = 0x0;
  delete _mutex; _mutex = 0x0;
  _sql.clear();
  _cb = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the database.
//
///////////////////////////////////////////////////////////////////////////////

void Database::_close()
{
  Guard guard ( this->mutex() );
  ::sqlite3_close ( _db );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback function to catch the statement results.
//  http://souptonuts.sourceforge.net/readme_sqlite_tutorial.html
//
///////////////////////////////////////////////////////////////////////////////

int Database::_callback ( void *user, int argc, char **argv, char **columnName )
{
  Database *me ( reinterpret_cast < Database * > ( user ) );
  return ( ( 0x0 != me ) ? me->_callback ( argc, argv, columnName ) : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback function to catch the statement results.
//  http://souptonuts.sourceforge.net/readme_sqlite_tutorial.html
//
///////////////////////////////////////////////////////////////////////////////

int Database::_callback ( int argc, char **argv, char **columnName )
{
  try
  {
    if ( true == _cb.valid() )
    {
      for ( int i = 0; i < argc; ++i )
      {
        if ( 0x0 != columnName[i] )
        {
          const std::string name ( columnName[i] );
          const std::string value ( ( 0x0 == argv[i] ) ? "" : argv[i] );
          (*_cb) ( *this, name, value );
        }
      }
    }
    return SQLITE_OK;
  }
  catch ( std::exception &e )
  {
    std::cout << "Error 2794326376: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 2935405268: exception generated in SQLite callback" << std::endl;
  }
  return SQLITE_ABORT;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

void Database::execute ( const std::string &s )
{
  this->sql ( s );
  this->execute();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the cached SQL string.
//
///////////////////////////////////////////////////////////////////////////////

void Database::execute()
{
  Guard guard ( this->mutex() );

  const std::string sql ( _sql );
  _sql.clear();

  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Database::_start ), "8960825960" );

  char *errorMessage = 0x0;
  const int code ( ::sqlite3_exec ( _db, sql.c_str(), &Database::_callback, this, &errorMessage ) );

  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Database::_finish ), "1884181044" );

  if ( SQLITE_OK != code )
  {
    std::string message ( ( 0x0 == errorMessage ) ? "" : errorMessage );
    throw Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 4873607880: Failed to execute SQL statement.",
        "\n  SQL: ", sql,
        "\n  Error: ", message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  For use with the output stream operator.
//
///////////////////////////////////////////////////////////////////////////////

Database &Database::flush ( Database &db )
{
  db.execute();
  return db;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Database::file() const
{
  Guard guard ( this->mutex() );
  return std::string ( _file.begin(), _file.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Database::Mutex &Database::mutex() const
{
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the SQL string.
//
///////////////////////////////////////////////////////////////////////////////

void Database::sql ( const std::string &s )
{
  Guard guard ( this->mutex() );
  _sql.assign ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the SQL string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Database::sql() const
{
  Guard guard ( this->mutex() );
  return std::string ( _sql.begin(), _sql.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Database::callback ( Callback *cb )
{
  Guard guard ( this->mutex() );
  _cb = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Database::_start()
{
  Guard guard ( this->mutex() );
  if ( true == _cb.valid() )
    _cb->start ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Database::_finish()
{
  Guard guard ( this->mutex() );
  if ( true == _cb.valid() )
    _cb->finish ( *this );
}
