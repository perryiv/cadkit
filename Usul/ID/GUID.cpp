
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a globally unique identifier.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/ID/GUID.h"

#include "Usul/Exceptions/Thrower.h"

#include <stdexcept>
#include <cstdlib>

using namespace Usul;
using namespace Usul::ID;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

GUID::GUID() : 
  _data0 ( 0 ),
  _data1 ( 0 ),
  _data2 ( 0 ),
  _data3 ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Construct from the integers.
//
///////////////////////////////////////////////////////////////////////////////

GUID::GUID ( Types::Uint32 data0, 
             Types::Uint32 data1, 
             Types::Uint32 data2, 
             Types::Uint32 data3 ) : 
  _data0 ( data0 ),
  _data1 ( data1 ),
  _data2 ( data2 ),
  _data3 ( data3 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Common error string used below.
//
///////////////////////////////////////////////////////////////////////////////

#define COMMON_ERROR_STRING \
  "\n\tFailed to convert string '", \
  guid, \
  "' to a GUID.", \
  "\n\tString must be 32 characters after removal of '-', '{', and '}'."


///////////////////////////////////////////////////////////////////////////////
//
//  Construct from a string.
//
///////////////////////////////////////////////////////////////////////////////

GUID::GUID ( const std::string &g ) : 
  _data0 ( 0 ),
  _data1 ( 0 ),
  _data2 ( 0 ),
  _data3 ( 0 )
{
  // Make a copy because we change it.
  std::string guid ( g );

  // Remove any dashes.
  std::string::size_type pos ( std::string::npos );
  while ( std::string::npos != ( pos = guid.find ( '-' ) ) )
    guid.erase ( pos );

  // The string should be at least 32 characters long.
  if ( guid.size() < 32 )
    Exceptions::Thrower<std::invalid_argument> 
      ( "Error: ", 1075270993, COMMON_ERROR_STRING );

  // Remove any brackets at either end.
  if ( '{' == guid[0] )
    guid.erase ( 0, 1 );
  std::string::size_type last ( guid.size() - 1 );
  if ( '}' == guid[last] )
    guid.erase ( 0, 1 );

  // Must be exactly 32 characters now.
  if ( 32 != guid.size() )
    Exceptions::Thrower<std::invalid_argument> 
      ( "Error: ", 1075270994, COMMON_ERROR_STRING );

  // The first 8 characters are the first 32-bit integer, and so on.
  std::string data0 ( guid.begin() +  0, guid.begin() +  8 );
  std::string data1 ( guid.begin() +  8, guid.begin() + 16 );
  std::string data2 ( guid.begin() + 16, guid.begin() + 24 );
  std::string data3 ( guid.begin() + 24, guid.end() );

  // Convert to the integers.
  _data0 = ::atoi ( data0.c_str() );
  _data1 = ::atoi ( data1.c_str() );
  _data2 = ::atoi ( data2.c_str() );
  _data3 = ::atoi ( data3.c_str() );
}
