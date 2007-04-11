
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for working with a file path.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_PATH_H_
#define _USUL_FILE_PATH_H_

#include "Usul/Predicates/UnaryPair.h"

#include <string>
#include <algorithm>
#include <stdlib.h>
#include <iostream>


namespace Usul {
namespace File {


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory from the path.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string directory ( const std::string &path, bool wantSlash )
{
  typedef std::string::value_type Char;
  typedef std::string::const_reverse_iterator Itr;
  typedef std::string::size_type Size;
  typedef std::equal_to<Char> IsEqual;
  typedef std::logical_or<bool> Or;

  // Look for the last slash.
#ifdef _MSC_VER
  Itr i ( std::find_if ( path.rbegin(), path.rend(), Usul::Predicates::unaryPair ( std::bind2nd ( IsEqual(), '\\' ), Or(), std::bind2nd ( IsEqual(), '/' ) ) ) );
#else
  Itr i ( std::find_if ( path.rbegin(), path.rend(), std::bind2nd ( IsEqual(), '/' ) ) );
#endif

  // Make the string.
  std::string dir ( path.begin(), i.base() );

  // Return now if it's empty.
  if ( dir.empty() )
    return dir;

  // Drop the slash if we're supposed to.
  Size last ( dir.size() - 1 );
  if ( ( false == wantSlash ) && ( '\\' == dir[last] || '/' == dir[last] ) )
    dir.erase ( last );

  // Return the result.
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full path name.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string fullPath ( const std::string &file )
{
  typedef std::string::value_type Char;

  const size_t size ( 1024 );
  Char buffer[size];
  ::memset ( buffer, '\0', size - 1 );

#ifdef _MSC_VER

  ::_fullpath ( buffer, file.c_str(), size - 1 );

#else

  std::cout << "Usul/File/Path.h std::string fullPath ( const std::string &file ) "
    << " Is not implemented yet." << std::endl;

#endif

  return std::string ( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension from the path.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string extension ( const std::string &path )
{
  // Find the last dot.
  const std::string::size_type dotPos ( path.find_last_of ( '.' ) );

  // Check for not extension or just a trailing '.'
  if ( dotPos == std::string::npos || dotPos == path.size() - 1 ) 
    return std::string();
  else
    return path.substr ( dotPos + 1 ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the base file name from the path.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string base ( const std::string &path )
{
  // Handle empty strings.
  if ( path.empty() )
    return std::string();

  // Get the directory and extension.
  const std::string dir ( Usul::File::directory ( path, true ) );
  const std::string ext ( Usul::File::extension ( path ) );

  // Subtract them off.
  std::string base ( path );
  base.erase ( base.size() - ext.size() - 1 );
  base.erase ( 0, dir.size() );

  // Return the base name.
  return base;
}


}; // namespace File
}; // namespace Usul


#endif // _USUL_FILE_PATH_H_
