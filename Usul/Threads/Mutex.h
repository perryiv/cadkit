
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

#ifdef _WIN32
# define USUL_WINDOWS
#endif

#ifdef _WIN64
# define USUL_WINDOWS
#endif

#ifdef USUL_WINDOWS
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

  virtual ~Mutex();

  // Create a mutex. Uses the registered create-function.
  static Mutex *          create();

  // Lock/unlock the mutex.
  void            lock();
  void            unlock();

protected:

  Mutex();

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
