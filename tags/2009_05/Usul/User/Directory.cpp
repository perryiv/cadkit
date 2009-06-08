
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
#include "Usul/App/Application.h"
#include "Usul/System/Environment.h"

#ifdef _MSC_VER // Visual C++
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <shlobj.h>
#endif

#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check if this is Windows.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool isWindows()
  {
    #ifdef _MSC_VER
    return true;
    #else
    return false;
    #endif
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to fix the slash.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void fixSlash ( std::string &s, bool wantSlash )
  {
    // Skip empty strings.
    if ( true == s.empty() )
      return;

    // See if there is a slash.
    const unsigned int last ( s.size() - 1 );
    const bool hasSlash ( '\\' == s.at ( last ) || '/' == s.at ( last ) );

    // Drop slash if we are supposed to.
    if ( true == hasSlash && false == wantSlash )
      s.resize ( last );

    // Add slash if we are supposed to.
    if ( false == hasSlash && true == wantSlash )
      s.append ( "/" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's document directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::documents ( bool wantSlash, bool throwIfFail )
{
  // Always look for environment variable.
  std::string dir ( Usul::System::Environment::get ( "HOME" ) );

#ifdef _MSC_VER

  // Try 'My Documents' directory. Note: CSIDL_MYDOCUMENTS returns an empty string.
  if ( true == dir.empty() )
  {
    char path [ MAX_PATH * 2 ];
    if ( ( FALSE == SHGetSpecialFolderPath ( 0x0, path, CSIDL_PERSONAL, TRUE ) ) && ( true == throwIfFail ) )
      throw std::runtime_error ( "Error 2904267615: Failed to find 'My Documents' directory" );
    dir = path;
  }

#endif

  // Punt if the string is empty.
  if ( ( true == dir.empty() ) && ( true == throwIfFail ) )
    throw std::runtime_error ( "Error 2322383190: Failed to find user's document directory" );

  // Fix the slash.
  Helper::fixSlash ( dir, wantSlash );

  // Return string.
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's application-data directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::appData ( bool wantSlash, bool throwIfFail )
{
  // Look for environment variable on non-windows systems.
  std::string dir ( ( false == Helper::isWindows() ) ? Usul::System::Environment::get ( "HOME" ) : "" );

#ifdef _MSC_VER

  // Try window's directory next.
  if ( dir.empty() )
  {
    char path [ MAX_PATH * 2 ];
    if ( ( FALSE == SHGetSpecialFolderPath ( 0x0, path, CSIDL_APPDATA, TRUE ) ) && ( true == throwIfFail ) )
      throw std::runtime_error ( "Error 2792381829: Failed to find 'Application Data' directory" );
    dir = path;
  }

#endif

  // Punt if the string is empty.
  if ( ( true == dir.empty() ) && ( true == throwIfFail ) )
    throw std::runtime_error ( "Error 3888703160: Failed to find user's application-data directory" );

  // Fix the slash.
  Helper::fixSlash ( dir, wantSlash );

  // Return string.
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's vendor directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::vendor ( const std::string &vendor, bool wantSlash )
{
  // Handle bad input.
  if ( vendor.empty() )
    throw std::runtime_error ( "Error 2017220875: Empty vendor name given." );

  // Get the application-data directory with the slash.
  const std::string data ( Usul::User::Directory::appData ( true ) );

  // Append this string to path.
  const std::string append ( ( wantSlash ) ? ( vendor + '/' ) : ( vendor ) );

  // Make the path. Note: if this is not Windows then we follow the unix 
  // convention of making a "dot" directory.
  const std::string path ( ( false == Helper::isWindows() ) ? ( data + '.' + append ) : ( data + append ) );

  // Return string.
  return path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's program directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::program ( const std::string &vendor, const std::string &program, bool wantSlash )
{
  // Handle bad input.
  if ( program.empty() )
    throw std::runtime_error ( "Error 3166677210: Empty program name given." );

  // Make the slash.
  const std::string slash ( ( wantSlash ) ? "/" : "" );

  // Make the path.
  const std::string path ( Usul::User::Directory::vendor ( vendor, true ) + program + slash );

  // Return string.
  return path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's program directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::program ( bool wantSlash )
{
  const std::string v ( Usul::App::Application::instance().vendor() );
  const std::string p ( Usul::App::Application::instance().program() );
  return Usul::User::Directory::program ( v, p, wantSlash );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get user's vendor directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::User::Directory::vendor ( bool wantSlash )
{
  const std::string v ( Usul::App::Application::instance().vendor() );
  return Usul::User::Directory::vendor ( v, wantSlash );
}
