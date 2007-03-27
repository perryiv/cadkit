
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User's directories.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_USER_DIRECTORY_CLASS_H_
#define _USUL_USER_DIRECTORY_CLASS_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace User {


struct USUL_EXPORT Directory
{
  // Get home directory. If environment variable 'HOME' is set, then that is 
  // returned. On Windows, if there is no 'HOME' environment variable, then 
  // the 'Application Data' directory in the user's profile is returned. 
  // Throws if it failed.
  static std::string      home ( bool wantSlash = false );

  // Get the directory in which the vendor can store persistent data. Throws if
  // it failed. If the user has HOME set then we follow the unix convention of 
  // making a "dot" directory.
  static std::string      vendor ( const std::string &vendorName, bool wantSlash = false );
};


} // namespace User
} // namespace Usul


#endif // _USUL_USER_DIRECTORY_CLASS_H_
