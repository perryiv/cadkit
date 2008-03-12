
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Mutex classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Threads/OpenThreads/Mutex.h"

#include "OpenThreads/Mutex"

#include "Usul/Threads/ThreadId.h"
#include "Usul/Exceptions/Thrower.h"

using namespace Threads;
using namespace Threads::OT;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mutex::Mutex() : _mutex ( new OpenThreads::Mutex ), _threadId ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Mutex::~Mutex()
{
  delete _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the state of the mutex,
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::_check()
{
  // See if this thread has the lock.
  if ( Usul::Threads::currentThreadId() == _threadId )
  {
    // Bad news.
    throw std::runtime_error ( "Error 3756599590, attempting to recursively lock a mutex" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::lock()
{
  // See if we can lock.
  this->_check();

  // Lock the mutex.
  if ( 0 != _mutex->lock() )
  {
    // Bad news.
    throw std::runtime_error ( "Error 1939779417, failed to lock mutex" );
  }

  // Get thread id.
  _threadId = Usul::Threads::currentThreadId();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::unlock()
{
  // Lock the mutex.
  if ( 0 != _mutex->unlock() )
  {
    // Bad news.
    throw std::runtime_error ( "Error 2622139462, failed to unlock mutex" );
  }

  // Reset the id
  _threadId = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Creation function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Threads
{
  namespace OT
  {
    Usul::Threads::Mutex *newMutex()
    {
      return new Mutex();
    }
  };
};