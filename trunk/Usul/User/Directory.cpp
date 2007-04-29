
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User's directories.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/User/Directory.h"
#include "Usul/System/Environment.h"

#ifdef _MSC_VER // Visual C++
# define NOMINMAX
# include <windows.h>
# include <shlobj.h>
#endif

#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's home directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::home ( bool wantSlash )
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


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's vendor directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::vendor ( const std::string &name, bool wantSlash )
{
  // Handle bad input.
  if ( name.empty() )
    throw std::runtime_error ( "Error 2017220875: Empty vendor name given." );

  // Get the home directory with the slash.
  const std::string home ( Usul::User::Directory::home ( true ) );

  // Does the user have HOME set?
  const bool hasHome ( Usul::System::Environment::has ( "HOME" ) );

  // Append this string to home path.
  const std::string append ( ( wantSlash ) ? ( name + '/' ) : ( name ) );

  // Make the path. Note: if the user has HOME set then we follow the unix 
  // convention of making a "dot" directory.
  const std::string path ( ( hasHome ) ? ( home + '.' + append ) : ( append ) );

  // Return string.
  return path;
}
