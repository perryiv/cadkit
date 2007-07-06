
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_SYSTEM_DIRECTORY_H__
#define __USUL_SYSTEM_DIRECTORY_H__

#include "Usul/Export/Export.h"

#include <string>

namespace Usul {
namespace System {

class USUL_EXPORT Directory
{
public:

  /// Get/Set the current working directory.
  static void          cwd ( const std::string& directory );
  static std::string   cwd ();

  /// Helper struct the change and restore current working directory.
  struct ScopedCwd
  {
    ScopedCwd ( const std::string& directory ) :
      _oldCwd ( Directory::cwd() )
    {
      Directory::cwd ( directory );
    }
    ~ScopedCwd ()
    {
      Directory::cwd ( _oldCwd );
    }

  private:
    std::string _oldCwd;
  };

  // Get home directory. If environment variable 'HOME' is set, then that is 
  // returned. On Windows, if there is no 'HOME' environment variable, then 
  // the 'Application Data' directory in the user's profile is returned. 
  // Throws if it failed.
  static std::string      home ( bool wantSlash = false );
};

}
}


#endif // __USUL_SYSTEM_DIRECTORY_H__
