
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  File statistics.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_STATISTICS_H_
#define _USUL_FILE_STATISTICS_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#ifdef _WIN32
# define STAT _stat
#else
# define STAT stat
#endif


namespace Usul {
namespace File {


///////////////////////////////////////////////////////////////////////////////
//
//  Get the file size.
//
///////////////////////////////////////////////////////////////////////////////

inline unsigned int size ( const std::string &filename )
{
  struct STAT filebuf;
  int result ( _stat ( filename.c_str(), &filebuf ) );
  return ( ( 0 == result ) ? filebuf.st_size : 0 );
}


}; // namespace File
}; // namespace Usul


#endif // _USUL_FILE_STATISTICS_H_
