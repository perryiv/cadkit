
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Find the contents of a directory.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_FIND_FILES_H_
#define _USUL_FILE_FIND_FILES_H_

#include <string>
#if _WIN32
# include <windows.h>
#else
  TODO
#endif


namespace Usul {
namespace File {


///////////////////////////////////////////////////////////////////////////////
//
//  Find all the files in the directory that match the pattern.
//
///////////////////////////////////////////////////////////////////////////////

template < class NameContainer >
inline void find ( const std::string &pattern, NameContainer &names )
{
#if _WIN32

  // Initialize.
  HANDLE handle = 0x0;

  // Safely...
  try
  {
    // Get the first file in the directory.
    WIN32_FIND_DATA data;
    handle = ::FindFirstFile ( pattern.c_str(), &data );
    if ( INVALID_HANDLE_VALUE == handle )
      return;

    // Push the first file onto the list.
    names.insert ( names.end(), data.cFileName );

    // Push any additional files.
    while ( ::FindNextFile ( handle, &data ) )
      names.insert ( names.end(), data.cFileName );
  }

  // Catch all exceptions.
  catch ( ... )
  {
    // Close the directory handle and re-throw.
    if ( handle )
      ::FindClose ( handle );
    throw;
  }

  // Close the directory handle.
  if ( handle )
    ::FindClose ( handle );

#else

  TODO

#endif
}


}; // namespace File
}; // namespace Usul


#endif // _USUL_FILE_FIND_FILES_H_
