
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Make directory or file if needed.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/Make.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Split.h"
#include "Usul/System/LastError.h"

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
# define TEST_ACCESS _access
#else
# include <sys/stat.h>
# include <unistd.h>
# define TEST_ACCESS access
#endif

#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to see if the directory or file exists.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool pathExists ( const std::string &path )
  {
    const int exists ( 0 );
    const bool result ( 0 == TEST_ACCESS ( path.c_str(), exists ) );
    return result;
  }
}

#ifdef _MSC_VER

///////////////////////////////////////////////////////////////////////////////
//
//  Win32 function to create a directory. See:
//  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/fs/createdirectory.asp
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void createDirectory ( const std::string &path )
  {
    // If it doesn't already exists...
    if ( false == Helper::pathExists ( path ) )
    {
      // Create the directory.
      if ( TRUE != ::CreateDirectory ( path.c_str(), 0x0 ) )
      {
        std::ostringstream out;
        out << "Error 2487206897: Failed to create directory '" + path + "'";
        if ( Usul::System::LastError::has() )
          out << ", " << Usul::System::LastError::message();
        throw std::runtime_error ( out.str() );
      }
    }
  }
}

#else

///////////////////////////////////////////////////////////////////////////////
//
//  POSIX function to create a directory.
//  See: http://www.opengroup.org/onlinepubs/009695399/functions/mkdir.html
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void createDirectory ( const std::string &path )
  {
    // If it doesn't already exists...
    if ( false == Helper::pathExists ( path ) )
    {
      // Create the directory.
      if ( 0 != ::mkdir ( path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) )
      {
        std::ostringstream out;
        out << "Error 8819370150: Failed to create directory '" + path + "'";
        if ( Usul::System::LastError::has() )
          out << ", " << Usul::System::LastError::message();
        throw std::runtime_error ( out.str() );
      }
    }
  }
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Make file if it does not already exist.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void createFile ( const std::string &path )
  {
    // Does the file exist?
    Usul::Predicates::FileExists exists;
    if ( false == exists ( path ) )
    {
      // Make the file.
      {
        std::ofstream out ( path.c_str() );
      }

      // Check again.
      if ( false == exists ( path ) )
      {
        std::ostringstream out;
        out << "Error 3914968472: Failed to create file '" + path + "'";
        if ( Usul::System::LastError::has() )
          out << ", " << Usul::System::LastError::message();
        throw std::runtime_error ( out.str() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make directory or file if it does not already exist.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void createFileOrDirectory ( const std::string &path )
  {
    // Handle empty path.
    if ( path.empty() )
      throw ( "Error 2735355448: Given path is empty" );

    // Is the last character a slash?
    const std::string::size_type last ( path.size() - 1 );
    const bool isDir ( '/' == path.at ( last ) || '\\' == path.at ( last ) );

    // Call appropriate function.
    if ( isDir )
      Helper::createDirectory ( path );
    else
      Helper::createFile ( path );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make directory or file if it does not already exist.
//
///////////////////////////////////////////////////////////////////////////////

void Usul::File::make ( const std::string &p )
{
  // Handle empty path.
  if ( p.empty() )
    throw ( "Error 1824260280: Given path is empty" );

  // Make copy.
  std::string path ( p );

  // Change all back-slashes to forward slashes because Windows can handle it.
  std::replace ( path.begin(), path.end(), '\\', '/' );

  // Split path into individual directories.
  typedef std::vector < std::string > Parts;
  Parts parts;
  Usul::Strings::split ( path, '/', false, parts );

  // We should have some parts.
  if ( parts.empty() )
    throw ( std::runtime_error ( "Error 4291918647: Failed to split path: " + path ) );

  // Initialize temporary path.
  std::string temp;

  // We don't want the drive letter.
  if ( std::string::npos != parts.front().find ( ':' ) )
  {
    temp.append ( parts.front() );
    temp.append ( "/" );
    parts.erase ( parts.begin() );
  }
  
  // On *NIX platorms, prepend a / to the beginning of the path.
  #ifndef _MSC_VER
  temp.insert ( temp.begin(), '/' );
  #endif

  // Loop through all but last part.
  const unsigned int num ( parts.size() );
  for ( unsigned int i = 0; i < num - 1; ++i )
  {
    temp.append ( parts.at ( i ) );
    temp.append ( "/" );
    Helper::createDirectory ( temp );
  }

  // Create last part.
  Helper::createFileOrDirectory ( path );
}
