
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlThreads: Thread stuff.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_THREADS_H_
#define _CADKIT_STANDARD_LIBRARY_THREADS_H_

#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# ifdef _WIN32 // PC
#  define NOMINMAX // Do not define min and max as macros.
#  include <windows.h>
# else // unix
#  include <pthread.h>
# endif // PC/unix
#endif // _CADKIT_USE_PRECOMPILED_HEADERS


namespace CadKit {
namespace Threads {

///////////////////////////////////////////////////////////////////////////////
//
// Get the current thread id.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T getCurrentThreadId()
{
#ifdef _WIN32
  return ( static_cast<T> ( ::GetCurrentThreadId() ) );
#else
  return ( static_cast<T> ( ::pthread_self() ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Thread-safe increment.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void safeIncrement ( T &num )
{
#ifdef _WIN32
  SL_ASSERT ( sizeof ( long ) == sizeof ( T ) );
  ::InterlockedIncrement ( (long *) (&num) );
#else
  // TODO, is this the correct way to do this?
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock ( &mutex );
  ++num;
  pthread_mutex_unlock ( &mutex );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Thread-safe decrement.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void safeDecrement ( T &num )
{
#ifdef _WIN32
  SL_ASSERT ( sizeof ( long ) == sizeof ( T ) );
  ::InterlockedDecrement ( (long *) (&num) );
#else
  // TODO, is this the correct way to do this?
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock ( &mutex );
  --num;
  pthread_mutex_unlock ( &mutex );
#endif
}


}; // namespace Threads
}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_THREADS_H_
