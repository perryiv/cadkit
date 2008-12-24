
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
#include "Usul/System/Environment.h"
#include "Usul/Exceptions/Thrower.h"

#ifdef _WIN32
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h> // For GetComputerName()
#else
#include <unistd.h>
#include <cstring>
#endif
#include <algorithm>
#include <stdexcept>
#include <vector>

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
  std::string machine;

#if _WIN32

  // Ask Windows for the computer name.
  const DWORD  size ( 2048 );
  DWORD used ( size - 1 );
  char buffer[size];
  if ( ::GetComputerNameA ( buffer, &used ) )
    machine = buffer;

#else
  
  // Get the host name.
  const unsigned int size ( 1023 );
  std::vector < char > chars ( size + 1, '\0' );
  if ( 0 == ::gethostname ( &chars[0], size ) )
    machine.assign( &chars[0], &chars[0] + ::strlen ( &chars[0] ) );
  
#endif
  
  // Ifthe above did not work, then try to get the environment variable.
  if ( machine.empty() )
    machine = Usul::System::Environment::get ( "HOST" );

  // If we still do not have the name then punt.
  if ( machine.empty() )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1988678328, environment variable 'HOST' is not defined." );
  }

  // Lose any domain info. We just want the machine name.
  std::string host ( machine );
  std::string::size_type pos ( host.find ( '.' ) );
  if ( std::string::npos != pos )
    host.erase ( pos );

  // Make sure it is all small letters.
  std::transform ( host.begin(), host.end(), host.begin(), ::tolower );

  // Return the machine name.
  return host;
}
