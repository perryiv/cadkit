
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
  // Get the application-data directory.
  static std::string      appData ( bool wantSlash = false, bool throwIfFail = true );

  // Get user's documents directory. If environment variable 'HOME' is set, then that is 
  // returned. On Windows, if there is no 'HOME' environment variable, then 
  // the 'My Documents' directory in the user's profile is returned. 
  static std::string      documents ( bool wantSlash = false, bool throwIfFail = true );

  // Get the directory in which the vendor can store persistent data. Throws if it fails.
  static std::string      vendor ( const std::string &vendorName, bool wantSlash = false );
  static std::string      vendor ( bool wantSlash = false );

  // Get the directory in which the program can store persistent data. Throws if it fails.
  static std::string      program ( const std::string &vendorName, const std::string &programName, bool wantSlash = false );
  static std::string      program ( bool wantSlash = false );
};


} // namespace User
} // namespace Usul


#endif // _USUL_USER_DIRECTORY_CLASS_H_
