
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlFileStats.h: Functions to get file statistics.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_FILE_STATS_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_FILE_STATS_FUNCTIONS_H_

#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# ifdef _WIN32
#  include <sys/types.h>
#  include <sys/stat.h>
# else
   TODO
# endif
# include <string>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Get the file size.
//
///////////////////////////////////////////////////////////////////////////////

bool getFileSize ( const std::basic_string<char> &filename, unsigned int &size )
{
  SL_ASSERT ( false == filename.empty() );

#ifdef _WIN32

  // Get the file statistics.
  struct _stat buf;
  int result = ::_stat ( filename.c_str(), &buf );
  if ( 0 != result )
    return false;

  // Set the size.
  size = buf.st_size;

  // It worked.
  return true;

#else

  TODO

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file size.
//
///////////////////////////////////////////////////////////////////////////////

bool getFileSize ( const std::basic_string<wchar_t> &filename, unsigned int &size )
{
  SL_ASSERT ( false == filename.empty() );

#ifdef _WIN32

  // Get the file statistics.
  struct _stat buf;
  int result = ::_wstat ( filename.c_str(), &buf );
  if ( 0 != result )
    return false;

  // Set the size.
  size = buf.st_size;

  // It worked.
  return true;

#else

  TODO

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file size.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int getFileSize ( const std::basic_string<char> &filename )
{
  unsigned int size;
  SL_VERIFY ( true == CadKit::getFileSize ( filename, size ) );
  return size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file size.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int getFileSize ( const std::basic_string<wchar_t> &filename )
{
  unsigned int size;
  SL_VERIFY ( true == CadKit::getFileSize ( filename, size ) );
  return size;
}


}; // End of namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_FILE_STATS_FUNCTIONS_H_
