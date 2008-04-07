
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlThreads: Thread stuff.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_THREADS_H_
#define _CADKIT_STANDARD_LIBRARY_THREADS_H_

#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# ifdef _WIN32
#  include "windows.h"
# endif // _WIN32
#endif // _CADKIT_USE_PRECOMPILED_HEADERS


namespace CadKit 
{
//////////////////////////////////////////////////////////////////////////
//
// Get the current thread id.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline T getCurrentThreadId()
{
#ifdef _WIN32
  return ( static_cast<T> ( ::GetCurrentThreadId() ) );
#else
  TODO
#endif
}


//////////////////////////////////////////////////////////////////////////
//
//  Thread safe increment.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void threadSafeIncrement ( T &num )
{
#ifdef _WIN32
  SL_ASSERT ( sizeof ( long ) == sizeof ( T ) );
  ::InterlockedIncrement ( (long *) (&num) );
#elif _LINUX
  Make this static so that we only have one that gets reused.
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock ( &mutex );
  ++num;
  pthread_mutex_unlock ( &mutex );
#else
TODO // This may be the same as _LINUX, have to check...
#endif
}


//////////////////////////////////////////////////////////////////////////
//
//  Thread safe decrement.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void threadSafeDecrement ( T &num )
{
#ifdef _WIN32
  SL_ASSERT ( sizeof ( long ) == sizeof ( T ) );
  ::InterlockedDecrement ( (long *) (&num) );
#elif _LINUX
  Make this static so that we only have one that gets reused.
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock ( &mutex );
  --num;
  pthread_mutex_unlock ( &mutex );
#else
TODO // This may be the same as _LINUX, have to check...
#endif
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_THREADS_H_