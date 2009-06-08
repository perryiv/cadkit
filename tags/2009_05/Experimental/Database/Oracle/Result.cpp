
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
//  Encapsulates the result of a query.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/Oracle/Result.h"
#include "Database/Oracle/Connection.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Pointers/Functions.h"
#include "Usul/Strings/Format.h"

#include "occi.h"

#include <stdexcept>

using namespace CadKit::Database::Oracle;

USUL_IMPLEMENT_TYPE_ID ( Result );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Result::Result ( Connection *c, oracle::occi::Statement *s, oracle::occi::ResultSet *r ) : BaseClass(),
  _connection ( c ),
  _statement ( s ),
  _result ( r ),
  _currentColumn ( 0 )
{
  Usul::Pointers::reference ( _connection );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Result::~Result()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Result::_destroy ), "1225065417" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean this instance. Should only be called from the destructor.
//
///////////////////////////////////////////////////////////////////////////////

void Result::_destroy()
{
  if ( ( 0x0 != _statement ) && ( 0x0 != _result ) )
  {
    USUL_TRY_BLOCK
    {
      _statement->closeResultSet ( _result );
    }
    catch ( const oracle::occi::SQLException &e )
    {
      std::cout << "Error 5436080370: Failed to close result set. " << ( ( 0x0 != e.what() ) ? e.what() : "" ) << std::endl;
    }
  }

  if ( ( 0x0 != _statement ) && ( 0x0 !=_connection ) )
  {
    USUL_TRY_BLOCK
    {
      _connection->_terminateStatement ( _statement );
    }
    catch ( const oracle::occi::SQLException &e )
    {
      std::cout << "Error 3433775247: Failed to terminate statement. " << ( ( 0x0 != e.what() ) ? e.what() : "" ) << std::endl;
    }
  }
  Usul::Pointers::unreference ( _connection );
  _connection = 0x0;
  _statement = 0x0;
  _result = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepare the next row for data retrieval. 
//  Returns false if there are no more rows.
//
///////////////////////////////////////////////////////////////////////////////

bool Result::prepareNextRow()
{
  _currentColumn = 1;

  if ( 0x0 == _result )
    return false;

  oracle::occi::ResultSet::Status status ( _result->status() );
  if ( oracle::occi::ResultSet::END_OF_FETCH == status )
    return false;

  return ( oracle::occi::ResultSet::END_OF_FETCH != _result->next() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Result::numColumns() const
{
  if ( 0x0 == _result )
    return 0;

  oracle::occi::ResultSet::Status status ( _result->status() );
  if ( oracle::occi::ResultSet::END_OF_FETCH == status )
    return 0;

  std::vector<oracle::occi::MetaData> meta = _result->getColumnListMetaData();
  return ( ( true == meta.empty() ) ? 0 : meta.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the value.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Function, class OutputType > 
  inline void getValue ( oracle::occi::ResultSet *result, Function fun, OutputType &value, unsigned int &column )
  {
    if ( 0x0 == result )
      throw std::runtime_error ( "Error 1644654670: Null result pointer" );

    value = ( ( *result ).*fun ) ( column++ );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( std::string &value )
{
  Helper::getValue ( _result, &oracle::occi::ResultSet::getString, value, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( double &value )
{
  Helper::getValue ( _result, &oracle::occi::ResultSet::getDouble, value, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( unsigned int &value )
{
  Helper::getValue ( _result, &oracle::occi::ResultSet::getUInt, value, _currentColumn );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Result &Result::operator >> ( int &value )
{
  Helper::getValue ( _result, &oracle::occi::ResultSet::getInt, value, _currentColumn );
  return *this;
}
