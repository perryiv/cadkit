
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlFileVerify.h: Functions to verify file and directory properties.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_FILE_VERIFY_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_FILE_VERIFY_FUNCTIONS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
# include <io.h>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Make the directory.
//
///////////////////////////////////////////////////////////////////////////////

template < typename CharType > bool makeDirectory ( const std::basic_string<CharType> &name )
{
  if ( false == name.empty() )
  {
    SL_ASSERT ( 0 );
    return false;
  }

#ifdef _WIN32

  TODO

#else

  TODO

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the name a directory?
//
///////////////////////////////////////////////////////////////////////////////

bool isDirectory ( const std::basic_string<char> &name )
{
  SL_ASSERT ( false == name.empty() );

#ifdef _WIN32

  // Get the file attributes.
  DWORD attributes = ::GetFileAttributesA ( name.c_str() );

  // Did it work?
  if ( -1 == attributes )
    return false;

  // Is it a directory?
  return ( FILE_ATTRIBUTE_DIRECTORY == ( attributes & FILE_ATTRIBUTE_DIRECTORY ) );

  // Does it exist?
  return ( -1 != ::_access ( name.c_str(), 0 ) );

#else

  TODO

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the name a directory?
//
///////////////////////////////////////////////////////////////////////////////

bool isDirectory ( const std::basic_string<wchar_t> &name )
{
  SL_ASSERT ( false == name.empty() );

#ifdef _WIN32

  // Get the file attributes.
  DWORD attributes = ::GetFileAttributesW ( name.c_str() );

  // Did it work?
  if ( -1 == attributes )
    return false;

  // Is it a directory?
  if ( FILE_ATTRIBUTE_DIRECTORY != ( attributes & FILE_ATTRIBUTE_DIRECTORY ) )
    return false;

  // Does it exist?
  return ( -1 != ::_waccess ( name.c_str(), 0 ) );

#else

  TODO

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we write to the file or directory?
//
///////////////////////////////////////////////////////////////////////////////

bool canWriteTo ( const std::basic_string<char> &name )
{
  SL_ASSERT ( false == name.empty() );

#ifdef _WIN32

  // See if we can write to the given file or directory.
  return ( -1 != ::_access ( name.c_str(), 2 ) );

#else

  TODO

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we write to the file or directory?
//
///////////////////////////////////////////////////////////////////////////////

bool canWriteTo ( const std::basic_string<wchar_t> &name )
{
  SL_ASSERT ( false == name.empty() );

#ifdef _WIN32

  // See if we can write to the given file or directory.
  return ( -1 != ::_waccess ( name.c_str(), 2 ) );

#else

  TODO

#endif
}


}; // End of namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_FILE_VERIFY_FUNCTIONS_H_
