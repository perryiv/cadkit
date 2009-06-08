
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

#ifndef _USUL_THREADS_MUTEX_CLASSES_H_
#define _USUL_THREADS_MUTEX_CLASSES_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

#ifdef _WIN32
# define USUL_WINDOWS
#endif

#ifdef _WIN64
# define USUL_WINDOWS
#endif

#ifdef USUL_WINDOWS
# ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0400 // Needed for TryEnterCriticalSection.  See http://msdn.microsoft.com/en-us/library/ms686857(VS.85).aspx
# endif
# define WIN32_LEAN_AND_MEAN

# ifndef NOMINMAX
#  define NOMINMAX
# endif

# include <windows.h>
#endif

#ifdef __GNUC__
# include <pthread.h>
#endif

namespace Usul {
namespace Threads {


///////////////////////////////////////////////////////////////////////////////
//
//  Base mutex class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Mutex
{
public:

  // Construction/Destruction.
  Mutex();
  ~Mutex();

  // Create a mutex.
  static Mutex *  create();

  // Lock the mutex.
  void            lock();
  void            lock ( Usul::Types::Uint64 timeout, unsigned long millisecondPause = 500 );

  // Unlock the mutex.
  void            unlock();

  // Try to lock the mutex.  Will return true if the lock has been acquired.
  bool            trylock();

private:

  Mutex ( const Mutex & );             // No copying
  Mutex &operator = ( const Mutex & ); // No assignment

#ifdef USUL_WINDOWS
  CRITICAL_SECTION _cs;
#endif

#ifdef __GNUC__
  pthread_mutex_t _mutex;
#endif
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_MUTEX_CLASSES_H_
