
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get user's home directory.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Home.h"

#ifdef _MSC_VER // Visual C++
#include "windows.h"
#include "shlobj.h"
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's home directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::System::Home::dir ( bool wantSlash )
{
  // Look for environment variable.
  const char *temp ( ::getenv ( "HOME" ) );
  std::string home ( ( temp ) ? temp : "" );

#ifdef _MSC_VER

  // Try window's directory next.
  if ( home.empty() )
  {
    char path [ MAX_PATH * 2  ];
    const BOOL result ( SHGetSpecialFolderPath ( 0x0, path, CSIDL_APPDATA, TRUE ) );
    if ( result )
      home = path;
  }

#endif

  // Drop slash if we are supposed to.
  if ( false == home.empty() && false == wantSlash )
  {
    const unsigned int last ( home.size() - 1 );
    if ( '\\' == home.at ( last ) || '/' == home.at ( last ) )
      home.resize ( last );
  }

  // Return string.
  return home;
}
