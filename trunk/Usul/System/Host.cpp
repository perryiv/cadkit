
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents the local host machine.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Host.h"
#include "Usul/Exceptions/Thrower.h"

// Disable deprecated warning in Visual Studio 8 for getenv
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#ifdef _WIN32
# include <windows.h> // For GetComputerName()
#endif
#include <algorithm>
#include <stdexcept>

using namespace Usul;
using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Get the machine name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Host::name()
{
  // Declare up here.
  const char *name = 0x0;

#if _WIN32

  // Ask Windows for the computer name.
  const DWORD  size ( 2048 );
  DWORD  used ( size - 1 );
  char buffer[size];
  if ( ::GetComputerNameA ( buffer, &used ) )
    name = buffer;

#endif

  // If we are on unix, or if the above did not work, then try to get the 
  // environment variable.
  if ( !name )
    name = ::getenv ( "HOST" );

  // If we still do not have the name then punt.
  if ( !name )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1988678328, environment variable 'HOST' is not defined." );
  }

  // Lose any domain info. We just want the machine name.
  std::string host ( name );
  std::string::size_type pos ( host.find ( '.' ) );
  if ( std::string::npos != pos )
    host.erase ( pos );

  // Make sure it is all small letters.
  std::transform ( host.begin(), host.end(), host.begin(), ::tolower );

  // Return the machine name.
  return host;
}
