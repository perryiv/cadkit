
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
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <shlobj.h>
# include "direct.h"
#endif

#include <stdexcept>
#include <cstring>
#include <cstdlib>

using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current working directory.
//
///////////////////////////////////////////////////////////////////////////////

void Directory::cwd ( const std::string& directory, bool allowThrow )
{
  // Initialize last error.
  Usul::System::LastError::init();

  // Set new working directory.
#ifdef _MSC_VER
  const int result ( ::_chdir ( directory.c_str() ) );
#else
  const int result (  ::chdir ( directory.c_str() ) );
#endif

  // Check result.
  if ( ( -1 == result ) && ( true == allowThrow ) )
  {
    throw std::runtime_error ( Usul::Strings::format ( "Error: 2118168350, Failed to set current working directory to '", directory, "', System error: ", Usul::System::LastError::message() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current working directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Directory::cwd()
{
  std::string directory;
  char* buffer;

   // Get the current working directory:
#ifdef _MSC_VER
  if( ( buffer = ::_getcwd ( NULL, 0 ) ) != NULL )
#else
  if( ( buffer =  ::getcwd ( NULL, 0 ) ) != NULL )
#endif
   {
     directory.assign ( buffer, ::strlen ( buffer ) );
     ::free ( buffer );
   }

  return directory;
}

