
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive mutex class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <stdexcept>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard < Usul::Threads::Mutex > MutexGuard;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RecursiveMutex::RecursiveMutex() :
  _mutex  ( Usul::Threads::Mutex::create() ),
  _local  ( Usul::Threads::Mutex::create() ),
  _count  ( 0 ),
  _thread ( 0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RecursiveMutex::~RecursiveMutex()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &RecursiveMutex::_destroy ), "8683423670" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this class.
//
///////////////////////////////////////////////////////////////////////////////

void RecursiveMutex::_destroy()
{
  USUL_TRACE_SCOPE;

  if ( 0 != _count )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 2732635427: deleting RecursiveMutex ", this, " with lock count ", _count );
  }

  if ( 0 != _thread )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1730106011: deleting RecursiveMutex ", this, " locked with thread ", _thread );
  }

  delete _local;
  delete _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void RecursiveMutex::lock()
{
  USUL_TRACE_SCOPE;

  // Get threads.
  const unsigned long id ( Usul::Threads::currentThreadId() );
  const unsigned long owner ( this->_getOwnerThread() );

  // Are we the owner?
  if ( id == owner )
  {
    ++_count;
  }

  // Otherwise, take ownership.
  else
  {
    // Lock first then set members.
    _mutex->lock();
    _thread = id;
    ++_count;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void RecursiveMutex::unlock()
{
  USUL_TRACE_SCOPE;

  // Get threads.
  const unsigned long id ( Usul::Threads::currentThreadId() );
  const unsigned long owner ( this->_getOwnerThread() );

  // We should only be in here if we own the mutex.
  if ( id != owner )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 3066735345: Thread ", id, " attempting to unlock recursive mutex owned by thread ", owner );
  }

  // Decrement the count.
  --_count;

  // Unlock mutex if we should.
  if ( 0 == _count )
  {
    _thread = 0;
    _mutex->unlock();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is it locked?
//
///////////////////////////////////////////////////////////////////////////////

unsigned long RecursiveMutex::numLocks() const
{
  USUL_TRACE_SCOPE;
  MutexGuard guard ( *_local );
  return _count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the owner thread.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long RecursiveMutex::_getOwnerThread() const
{
  USUL_TRACE_SCOPE;
  MutexGuard guard ( *_local );
  return _thread;
}
