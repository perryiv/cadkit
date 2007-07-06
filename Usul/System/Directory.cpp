
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Directory.h"
#include "Usul/System/Environment.h"

#ifdef _MSC_VER
# define NOMINMAX
# include <windows.h>
# include <shlobj.h>
# include "direct.h"
#endif

#include <stdexcept>

using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current working directory.
//
///////////////////////////////////////////////////////////////////////////////

void Directory::cwd ( const std::string& directory )
{
#ifdef _MSC_VER
  ::_chdir( directory.c_str() );
#else
  ::chdir ( directory.c_str() );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current working directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Directory::cwd ()
{
  std::string directory;
  char* buffer;

   // Get the current working directory: 
#ifdef _MSC_VER
  if( (buffer = ::_getcwd( NULL, 0 )) != NULL )
#else
  if( (buffer = ::getcwd( NULL, 0 )) != NULL )
#endif
   {
     directory.assign( buffer, ::strlen( buffer ) );
     ::free( buffer );
   }

  return directory;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's home directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Directory::home ( bool wantSlash )
{
  // Look for environment variable.
  std::string dir ( Usul::System::Environment::get ( "HOME" ) );

#ifdef _MSC_VER

  // Try window's directory next.
  if ( dir.empty() )
  {
    char path [ MAX_PATH * 2  ];
    if ( FALSE == SHGetSpecialFolderPath ( 0x0, path, CSIDL_APPDATA, TRUE ) )
      throw std::runtime_error ( "Error 2904267615: Failed to find 'Application Data' directory" );
    dir = path;
  }

#endif

  // Punt if the string is empty.
  if ( dir.empty() )
    throw std::runtime_error ( "Error 2322383190: Failed to find home directory" );

  // See if there is a slash.
  const unsigned int last ( dir.size() - 1 );
  const bool hasSlash ( '\\' == dir.at ( last ) || '/' == dir.at ( last ) );

  // Drop slash if we are supposed to.
  if ( true == hasSlash && false == wantSlash )
    dir.resize ( last );

  // Add slash if we are supposed to.
  if ( false == hasSlash && true == wantSlash )
    dir.append ( "/" );

  // Return string.
  return dir;
}
