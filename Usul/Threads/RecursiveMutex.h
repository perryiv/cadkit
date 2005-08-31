
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive mutex class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_RECURSIVE_MUTEX_CLASS_H_
#define _USUL_THREADS_RECURSIVE_MUTEX_CLASS_H_

#include "Usul/Export/Export.h"


namespace Usul {
namespace Threads {

class Mutex;

class USUL_EXPORT RecursiveMutex
{
public:

  RecursiveMutex();
  ~RecursiveMutex();

  // Return the number of times this mutex has been locked.
  unsigned int            count() const;

  // Lock/unlock the mutex.
  void                    lock();
  void                    unlock();

private:

  RecursiveMutex ( const RecursiveMutex & );             // No copying
  RecursiveMutex &operator = ( const RecursiveMutex & ); // No assignment

  mutable Usul::Threads::Mutex *_mutex;
  Usul::Threads::Mutex *_local;
  unsigned int _count;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_RECURSIVE_MUTEX_CLASS_H_
