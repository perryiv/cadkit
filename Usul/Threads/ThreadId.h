
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Function for getting the thread-id. This is inline because I do not 
//  want Usul to depend on pthreads.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_THREAD_ID_CLASS_H_
#define _USUL_THREADS_THREAD_ID_CLASS_H_

#ifdef _WIN32
# define NOMINMAX
# include <windows.h>
#elif __GNUC__
# include "Usul/Cast/Cast.h"
# include <pthread.h>
#elif __sgi
# include <pthread.h>
#else
 TODO
#endif


namespace Usul {
namespace Threads {


inline unsigned long currentThreadId()
{
  #ifdef _WIN32
    return ::GetCurrentThreadId();
  #elif __GNUC__
    return USUL_UNSAFE_CAST ( unsigned long, ::pthread_self() ); // TODO, find low-level way.
  #elif __sgi
    return ::pthread_self(); // TODO, find low-level way.
  #else
    TODO;
  #endif
}


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_THREAD_ID_CLASS_H_
