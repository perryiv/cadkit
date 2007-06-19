
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Directory.h"

#ifdef _MSC_VER
#  include "direct.h"
#endif

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
