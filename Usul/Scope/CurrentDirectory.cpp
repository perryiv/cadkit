
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/System/Directory.h"
#include "Usul/Threads/RecursiveMutex.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize global members.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Scope
  {
    namespace Detail
    {
      Usul::Threads::RecursiveMutex _mutex;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Scope::CurrentDirectory::CurrentDirectory ( const std::string& directory, bool allowThrow ) :
  _oldCwd ( Usul::System::Directory::cwd() ),
  _allowThrow ( allowThrow )
{
  Usul::Scope::Detail::_mutex.lock();
  Usul::System::Directory::cwd ( directory, _allowThrow );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Scope::CurrentDirectory::~CurrentDirectory()
{
  Usul::System::Directory::cwd ( _oldCwd, _allowThrow );
  Usul::Scope::Detail::_mutex.unlock();
}
