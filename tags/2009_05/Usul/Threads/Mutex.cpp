
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Mutex class.
//
//  Should this file be split into 2 implementation files?
//  MutexWin32.cpp and MutexPThreads.cpp?
//
//  For critical section objects see:
//  http://msdn.microsoft.com/en-us/library/ms682530.aspx
//
//  For pthreads see:
//  https://computing.llnl.gov/tutorials/pthreads/
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Mutex.h"
#include "Usul/Exceptions/TimedOut.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Clock.h"
#include "Usul/System/Sleep.h"

#include <stdexcept>

#ifdef __GNUC__
# include <errno.h>
#endif

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mutex::Mutex() :
#ifdef USUL_WINDOWS
  _cs()
#endif
#ifdef __GNUC__
  _mutex()
#endif
{
#ifdef USUL_WINDOWS
  ::InitializeCriticalSection( &_cs );
#endif

#ifdef __GNUC__
  // This function may return an error.  Should we throw?
  ::pthread_mutex_init ( &_mutex, 0x0 );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Mutex::~Mutex()
{
#ifdef USUL_WINDOWS
  ::DeleteCriticalSection( &_cs );
#endif

#ifdef __GNUC__
  ::pthread_mutex_destroy ( &_mutex );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Mutex *Mutex::create()
{
  return new Mutex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::lock()
{
#ifdef USUL_WINDOWS
  ::EnterCriticalSection ( &_cs );
#endif

#ifdef __GNUC__
  ::pthread_mutex_lock ( &_mutex );
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::unlock()
{
#ifdef USUL_WINDOWS
  ::LeaveCriticalSection ( &_cs );
#endif

#ifdef __GNUC__
  ::pthread_mutex_unlock ( &_mutex );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Try to lock the mutex.  Will return true if the lock has been acquired.
//
///////////////////////////////////////////////////////////////////////////////

bool Mutex::trylock()
{
#ifdef USUL_WINDOWS

  return ( 0 != ::TryEnterCriticalSection ( &_cs ) );

#endif

#ifdef __GNUC__

  const int error ( ::pthread_mutex_trylock ( &_mutex ) );

  // Check for unintilaized mutex and invalid pointer.
  if ( EINVAL == error || EFAULT == error )
    throw std::runtime_error ( "Error 4580724080: Mutex is not initialized properly." );

  return ( EBUSY != error );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::lock ( Usul::Types::Uint64 timeout, unsigned long pause )
{
  // Get the current time.
  const Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );

  // Try to acquire the lock...
  while ( false == this->trylock() )
  {
    // Calculate how long have we been trying.
    const Usul::Types::Uint64 duration ( Usul::System::Clock::milliseconds() - start );

    // If the time is greater than the timeout, throw an exception.
    if ( duration > timeout )
    {
      throw Usul::Exceptions::TimedOut::AcquireLock ( Usul::Strings::format 
        ( "Warning 1373313702: Could not acquire lock within ", timeout, " milliseconds." ) );
    }

    // Try again in "pause" milliseconds.
    Usul::System::Sleep::milliseconds ( pause );
  }
}
