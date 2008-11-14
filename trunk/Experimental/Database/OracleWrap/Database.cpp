
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

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"

#include "ocilib.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace OracleWrap;


///////////////////////////////////////////////////////////////////////////////
//
//  Get the error message.
//
///////////////////////////////////////////////////////////////////////////////

namespace OracleWrap
{
  namespace Detail
  {
    std::string getErrorMessage ( OCI_Error *error )
    {
      std::ostringstream out;

      const char *text ( ::OCI_ErrorGetString ( error ) );
      if ( 0x0 != text )
      {
        out << "Message: " << text;
      }

      if ( OCI_ERR_ORACLE == ::OCI_ErrorGetType ( error ) )
      {
        const char *sql ( ::OCI_GetSql ( ::OCI_ErrorGetStatement ( error ) ) );
        if ( 0x0 != sql )
        {
          out << ", SQL: " << sql;
        }
      }

      return out.str();
    } 
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last error message.
//
///////////////////////////////////////////////////////////////////////////////

namespace OracleWrap
{
  namespace Detail
  {
    std::string getLastError()
    {
      return OracleWrap::Detail::getErrorMessage ( ::OCI_GetLastError() );
    } 
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Error handler function.
//
///////////////////////////////////////////////////////////////////////////////

namespace OracleWrap
{
  namespace Detail
  {
    void printError ( OCI_Error *error )
    {
      std::cout << OracleWrap::Detail::getErrorMessage ( error ) << std::endl;
    } 
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize and clean up the library.
//
///////////////////////////////////////////////////////////////////////////////

namespace OracleWrap
{
  namespace Detail
  {
    struct Life
    {
      Life() : _isInitialized ( false )
      {
        _isInitialized = ( TRUE == ::OCI_Initialize ( &OracleWrap::Detail::printError, 0x0, OCI_ENV_CONTEXT ) );
      }
      ~Life()
      {
        if ( true == _isInitialized )
        {
          ::OCI_Cleanup();
        }
      }
      bool isInitialized() const
      {
        return _isInitialized;
      }
    private:
      bool _isInitialized;
    } ociLib;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Database::Database ( const std::string &database, const std::string &user, const std::string &password ) : BaseClass(),
  _connection ( 0x0 ),
  _statement ( 0x0 )
{
  // Check library state.
  if ( false == Detail::ociLib.isInitialized() )
    throw std::runtime_error ( "Error 3788882072: OCI wrapper not initialized properly" );

  // Try to connect.
  _connection = ::OCI_ConnectionCreate ( database.c_str(), user.c_str(), password.c_str(), OCI_SESSION_DEFAULT );
  if ( 0x0 == _connection )
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 1602442055: Failed to connect to database. ", OracleWrap::Detail::getLastError() ) );

  // Create the statement object.
  _statement = ::OCI_StatementCreate ( _connection );
  if ( 0x0 == _statement )
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3170960199: Failed to create statement. ", OracleWrap::Detail::getLastError() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Database::~Database()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Database::_destroy ), "1823607323" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Database::_destroy()
{
  if ( 0x0 != _statement )
  {
    if ( FALSE == ::OCI_StatementFree ( _statement ) )
    {
      std::cout << "Error 9523651570: Failed to delete statement. " << OracleWrap::Detail::getLastError() << std::endl;
    }
    _statement = 0x0;
  }

  if ( 0x0 != _connection )
  {
    if ( FALSE == ::OCI_ConnectionFree ( _connection ) )
    {
      std::cout << "Error 2330026500: Failed to free connection. " << OracleWrap::Detail::getLastError() << std::endl;
    }
    _connection = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the given SQL string.
//
///////////////////////////////////////////////////////////////////////////////

OracleWrap::Result::RefPtr Database::execute ( const std::string &sql )
{
  Guard guard ( this );

  // Empty string just means no result.
  if ( true == sql.empty() )
    return Result::RefPtr ( new Result ( 0x0, this->mutex() ) );

  // Check state.
  if ( 0x0 == _statement )
    throw std::runtime_error ( "Error 4136575298: Null statement pointer" );

  // Execute the statement.
  if ( FALSE == ::OCI_ExecuteStmt ( _statement, sql.c_str() ) )
    throw std::runtime_error ( Usul::Strings::format 
    ( "Error 2396100199: Statement failed. ", OracleWrap::Detail::getLastError() ) );

  // Return the result. We pass the mutex so that no further executions 
  // can happen until the result is deleted.
  return Result::RefPtr ( new Result ( _statement, this->mutex() ) );
}
