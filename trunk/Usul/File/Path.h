
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
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
#ifdef _WIN32
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
//  Get the extension from the path.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string extension ( const std::string &path )
{
  typedef std::string::value_type Char;
  typedef std::string::const_reverse_iterator Itr;
  typedef std::equal_to<Char> IsEqual;
  typedef std::logical_or<bool> Or;

  // Look for the last period.
  Itr i ( std::find_if ( path.rbegin(), path.rend(), std::bind2nd ( IsEqual(), '.' ) ) );

  // Return the result.
  const std::string ext ( i.base(), path.end() );
  return ext;
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
