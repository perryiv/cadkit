
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

#include "Usul/Threads/Mutex.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Errors/Assert.h"

#include <stdexcept>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mutex::Mutex()
{
  USUL_TRACE_SCOPE;

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
  USUL_TRACE_SCOPE;

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
  USUL_TRACE_SCOPE_STATIC;

  return new Mutex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void Mutex::lock()
{
  USUL_TRACE_SCOPE;

#ifdef USUL_WINDOWS
  ::EnterCriticalSection( &_cs );
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
  USUL_TRACE_SCOPE;

#ifdef USUL_WINDOWS
  ::LeaveCriticalSection( &_cs );
#endif

#ifdef __GNUC__
  ::pthread_mutex_unlock ( &_mutex );
#endif

}