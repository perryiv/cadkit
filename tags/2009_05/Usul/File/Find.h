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

#include "Usul/File/Slash.h"

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include "windows.h"
#else
#include <sys/types.h>
#include <dirent.h>
#include "Usul/File/Path.h"
#endif

#include <string>


namespace Usul {
namespace File {


///////////////////////////////////////////////////////////////////////////////
//
//  Find all the files in the directory that match the pattern.
//
///////////////////////////////////////////////////////////////////////////////

template < class Names >
inline void find ( const std::string &d, const std::string &ext, Names &names )
{
#if _WIN32

  // Initialize.
  HANDLE handle = 0x0;

  // Safely...
  try
  {
    // For convenience.
    const std::string dir ( d + Usul::File::slash() );
    const std::string path ( dir + "*." + ext );

    // Get the first file in the directory.
    WIN32_FIND_DATAA data;
    handle = ::FindFirstFileA ( path.c_str(), &data );
    if ( INVALID_HANDLE_VALUE == handle )
      return;

    // Push the first file onto the list.
    names.insert ( names.end(), dir + data.cFileName );

    // Push any additional files.
    while ( ::FindNextFileA ( handle, &data ) )
      names.insert ( names.end(), dir + data.cFileName );
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

  struct dirent *dp;

  // Get directory stream pointer
  if ( DIR * dirp = opendir ( d.c_str() ) )
  {
    // Loop over directory entries
    struct dirent dirresult;
    
    while ( !readdir_r ( dirp, &dirresult, &dp ) && dp )
    {
      // Get name
      std::string name ( dp->d_name );

      const std::string base ( Usul::File::base ( name ) );
      if ( ( "." != base ) && ( ".." != base ) )
      {
        if ( ( "*" == ext ) || ( Usul::File::extension ( name ) == ext ) )
        {
          // Add to list
          names.insert ( names.end(), name );
        }
      }
    }
    closedir ( dirp );
  }

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a list of all files in the given directories that match the extension.
//
///////////////////////////////////////////////////////////////////////////////

template < class Dirs, class Names >
inline void find ( const Dirs &dirs, const std::string &ext, Names &fl )
{
  // Loop through the directories.
  for ( typename Dirs::const_iterator i = dirs.begin(); i != dirs.end(); ++i )
  {
    // Append the files.
    Usul::File::find ( *i, ext, fl );
  }
}


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_FIND_FILES_H_
