
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

  // Lock/unlock the mutex.
  void                    lock();
  void                    unlock();

  // How many locks are there?
  unsigned long           numLocks() const;

private:

  RecursiveMutex ( const RecursiveMutex & );             // No copying
  RecursiveMutex &operator = ( const RecursiveMutex & ); // No assignment

  unsigned long           _getOwnerThread() const;

  void                    _destroy();

  Usul::Threads::Mutex *_mutex;
  mutable Usul::Threads::Mutex *_local;
  unsigned long _count;
  unsigned long _thread;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_RECURSIVE_MUTEX_CLASS_H_
