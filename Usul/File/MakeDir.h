
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


namespace Usul {
namespace File {


struct USUL_EXPORT MakeDir
{
  // Make directory if it does not already exist.
  static bool       make ( const std::string & );
};


}; // namespace File
}; // namespace Usul


#endif // _USUL_FILE_MAKE_DIRECTORY_H_
