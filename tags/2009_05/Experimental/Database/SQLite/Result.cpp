
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"

#include "sqlite3.h"

#include "boost/bind.hpp"

#include <limits>
#include <stdexcept>

using namespace CadKit::Database::SQLite;

USUL_IMPLEMENT_TYPE_ID ( Result );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Result::Result ( const std::string &sql, ::sqlite3 *db, ::sqlite3_stmt *s ) : BaseClass(),
  _database ( db ),
  _statement ( s ),
  _currentColumn ( 0 ),
  _sql ( sql )
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
  this->finalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize the statement now.
//
///////////////////////////////////////////////////////////////////////////////

void Result::finalize()
{
  Guard guard ( this );

  // Copy the statement and set member to null.
  ::sqlite3_stmt *statement ( _statement );
  _statement = 0x0;

  // Is the statement valid?
  if ( 0x0 == statement )
    return;

  // Try to finalize the statement.
  Helper::call ( boost::bind<int> ( ::sqlite3_finalize, statement ), 
    "Error 1333604206: Failed to finalize statement", _sql, _database );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepare the next row for data retrieval. 
//  Returns false if there are no more rows or if there was an error.
//
///////////////////////////////////////////////////////////////////////////////

bool Result::prepareNextRow()
{
  Guard guard ( this );

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
  Guard guard ( this );

  if ( 0x0 == _statement )
    return 0;

  const int num ( ::sqlite3_column_count ( _statement ) );
  return ( ( num < 0 ) ? 0 : static_cast<unsigned int> ( num ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the column name or empty string if index is out of range.
//
///////////////////////////////////////////////////////////////////////////////

std::string Result::columnName ( unsigned int which ) const
{
  Guard guard ( this );

  std::string name;

  // Handle bad statement.
  if ( 0x0 == _statement )
    return name;

  // Handle out of range index.
  if ( which >= this->numColumns() )
    return name;

  // Set the name and return it.
  name = ::sqlite3_column_name ( _statement, which );
  return name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( std::string &value )
{
  Guard guard ( this );
  const unsigned char *temp ( 0x0 );
  Helper::getValue ( _statement, ::sqlite3_column_text, temp, _currentColumn );
  value = ( ( 0x0 == temp ) ? std::string() : reinterpret_cast<const char *> ( temp ) );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( double &value )
{
  Guard guard ( this );
  Helper::getValue ( _statement, ::sqlite3_column_double, value, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( float &value )
{
  Guard guard ( this );
  double temp ( 0 );
  *this >> temp;
  if ( ( temp < static_cast<double> ( - std::numeric_limits<float>::max() ) ) ||
       ( temp > static_cast<double> (   std::numeric_limits<float>::max() ) ) )
  {
    throw Usul::Exceptions::Exception ( Usul::Strings::format 
      ( "Error 2911940382: ", sizeof ( double ), " byte float ", temp, 
        " will not fit in a ", sizeof ( float ), " byte float" ) );
  }
  value = static_cast<float> ( temp );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( int &value )
{
  Guard guard ( this );
  Helper::getValue ( _statement, ::sqlite3_column_int, value, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( unsigned int &value )
{
  Guard guard ( this );
  int temp ( 0 );
  *this >> temp;
  if ( temp < 0 )
  {
    throw Usul::Exceptions::Exception ( Usul::Strings::format 
      ( "Error 3361492869: Negative ", sizeof ( int ), "-byte integer ", temp, 
        " will not convert to ", sizeof ( unsigned int ), "-byte unsigned integer" ) );
  }
  value = static_cast<unsigned int> ( temp );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( Usul::Types::Int64 &value )
{
  Guard guard ( this );
  Helper::getValue ( _statement, ::sqlite3_column_int64, value, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( Usul::Types::Uint64 &value )
{
  Guard guard ( this );
  Usul::Types::Int64 temp ( 0 );
  *this >> temp;
  if ( temp < 0 )
  {
    throw Usul::Exceptions::Exception ( Usul::Strings::format 
      ( "Error 3361492869: Negative ", sizeof ( Usul::Types::Int64 ), "-byte integer ", temp, 
        " will not convert to ", sizeof ( Usul::Types::Uint64 ), "-byte unsigned integer" ) );
  }
  value = static_cast<Usul::Types::Uint64> ( temp );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( Blob &value )
{
  Guard guard ( this );

  // According to http://www.sqlite.org/c3ref/column_blob.html, we should 
  // get the data before we ask how big it is.
  const void *temp ( 0x0 );
  Helper::getValue ( _statement, ::sqlite3_column_blob, temp, _currentColumn );

  // Have to rewind.
  --_currentColumn;

  int numBytes ( 0 );
  Helper::getValue ( _statement, ::sqlite3_column_bytes, numBytes, _currentColumn );

  if ( ( 0x0 != temp ) && ( numBytes > 0 ) )
  {
    const unsigned char *start ( reinterpret_cast < const unsigned char * > ( temp ) );
    value.assign ( start, start + numBytes );
  }

  return *this;
}
