
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Function for getting the file size
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FILE_SIZE_H__
#define __USUL_FILE_SIZE_H__

#ifdef _WIN32
# define STAT _stat
#else
# define STAT stat
#endif

#include <sys/stat.h>
#include <string>

namespace Usul {
namespace File {

int filesize( const std::string& filename )
{
  struct STAT filebuf;
  int result ( _stat ( filename.c_str(), &filebuf ) );
  return filebuf.st_size;
}

};
};

#endif // __USUL_FILE_SIZE_H__