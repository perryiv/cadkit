
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
#include "Usul/System/LastError.h"
#include "Usul/Strings/Format.h"

#ifdef _MSC_VER
# define NOMINMAX
# include <windows.h>
# include <shlobj.h>
# include "direct.h"
#endif

#include <stdexcept>
#include <iostream>

using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current working directory.
//
///////////////////////////////////////////////////////////////////////////////

void Directory::cwd ( const std::string& directory )
{
  // Initialize last error.
  Usul::System::LastError::init();

  // Set new working directory.
#ifdef _MSC_VER
  const int result ( ::_chdir( directory.c_str() ) );
#else
  const int result ( ::chdir ( directory.c_str() ) );
#endif

  // Check result.
  if ( -1 == result )
  {
    throw std::runtime_error ( Usul::Strings::format ( "Error: 2118168350, Failed to set current working directory to '", directory, "', System error: ", Usul::System::LastError::message() ) );
  }

#if 1
  std::cout << "Setting Current Working Directory: " << directory << std::endl;
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
#if 1
  std::cout << "Getting Current Working Directory: " << directory << std::endl;
#endif
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
