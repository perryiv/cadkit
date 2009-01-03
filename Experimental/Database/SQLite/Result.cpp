
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
//  Result for SQLite database.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/SQLite/Result.h"
#include "Database/SQLite/Internal.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"

#include "sqlite3.h"

#include <stdexcept>

using namespace CadKit::Database::SQLite;

USUL_IMPLEMENT_TYPE_ID ( Result );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Result::Result ( ::sqlite3 *db, ::sqlite3_stmt *s ) : BaseClass(),
  _database ( db ),
  _statement ( s ),
  _currentColumn ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Result::~Result()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Result::_destroy ), "4030095801" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Result::_destroy()
{
  // We do not own _database!

  if ( 0x0 != _statement )
  {
    if ( SQLITE_OK != ::sqlite3_finalize ( _statement ) )
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 1333604206: Failed to finalize statement, ", Helper::errorMessage ( _database ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepare the next row for data retrieval. 
//  Returns false if there are no more rows or if there was an error.
//
///////////////////////////////////////////////////////////////////////////////

bool Result::prepareNextRow()
{
  _currentColumn = 0;

  if ( 0x0 == _statement )
    return false;

  const int result ( ::sqlite3_step ( _statement ) );
  const bool answer ( SQLITE_ROW == result );
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Result::numColumns() const
{
  if ( 0x0 == _statement )
    return 0;

  const int num ( ::sqlite3_column_count ( _statement ) );
  return ( ( num < 0 ) ? 0 : static_cast<unsigned int> ( num ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( std::string &value )
{
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( double &value )
{
  Helper::getValue ( _statement, ::sqlite3_column_double, value, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( int &value )
{
  Helper::getValue ( _statement, ::sqlite3_column_int, value, _currentColumn );
  return *this;
}
