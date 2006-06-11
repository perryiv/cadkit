
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get the environment variable.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Environment.h"

#include <cstdlib>


///////////////////////////////////////////////////////////////////////////////
//
//  Return the environment variable.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::System::Environment::get ( const std::string &name ) 
{
  // Handle trivial case.
  if ( name.empty() )
    return std::string();

#if ( defined ( _MSC_VER ) && ( _MSC_VER >= 1400 ) )

  // Initialize.
  std::string temp;
  char *buffer = 0x0;

  // Look for the environment variable safely.
  if ( 0 != ::_dupenv_s ( &buffer, 0x0, name.c_str() ) )
  {
    // Note: a missing environment variable should not be an error.
    // If we get to here then something else is wrong.
    throw std::runtime_error ( "Error 2373003667: problem when looking for environment variable: " + name );
  }
  else
  {
    // A missing environment variable yield null.
    temp = ( ( 0x0 == buffer ) ? "" : buffer );

    // It's ok to free null.
    ::free ( buffer );
  }

  // Return what we have, which may be empty.
  return temp;

#else

  const char *temp ( ::getenv ( name.c_str() ) );
  return std::string ( ( 0x0 == temp ) ? "" : temp );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the environment variable exists.
//
///////////////////////////////////////////////////////////////////////////////

bool Usul::System::Environment::has ( const std::string &name ) 
{
  const std::string value ( Usul::System::Environment::get ( name ) );
  return ( false == value.empty() );
}
