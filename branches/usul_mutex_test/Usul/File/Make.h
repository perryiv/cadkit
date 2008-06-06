
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Make directory.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_MAKE_DIRECTORY_H_
#define _USUL_FILE_MAKE_DIRECTORY_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul
{
  namespace File
  {
    // Make directory or file if it does not already exist. Given string is 
    // assumed to be a directory if it has a trailing slash. 
    // Throws if it failed.
    USUL_EXPORT void make ( const std::string & );
  }
}


#endif // _USUL_FILE_MAKE_DIRECTORY_H_
