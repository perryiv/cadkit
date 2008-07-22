
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
#include "Usul/Threads/Mutex.h"

#include <string>

namespace Usul {
namespace System {

class USUL_EXPORT Directory
{
public:

  /// Get/Set the current working directory.
  static void          cwd ( const std::string& directory, bool allowThrow = true );
  static std::string   cwd ();
#if 0
  /// Helper struct the change and restore current working directory.
  struct USUL_EXPORT ScopedCwd
  {
    typedef Usul::Threads::Mutex Mutex;
    
    ScopedCwd ( const std::string& directory, bool allowThrow = true ) :
      _oldCwd ( Directory::cwd() ),
      _allowThrow ( allowThrow )
    {
      _mutex.lock();
      Directory::cwd ( directory, _allowThrow );
    }
    ~ScopedCwd()
    {
      Directory::cwd ( _oldCwd, _allowThrow );
      _mutex.unlock();
    }

  private:
    std::string _oldCwd;
    bool _allowThrow;
    static Mutex _mutex;
  };
#endif
};

}
}


#endif // __USUL_SYSTEM_DIRECTORY_H__
