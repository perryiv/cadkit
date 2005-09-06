
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
  virtual ~RecursiveMutex();

  // Lock/unlock the mutex.
  virtual void            lock() = 0;
  virtual void            unlock() = 0;

private:

  RecursiveMutex ( const RecursiveMutex & );             // No copying
  RecursiveMutex &operator = ( const RecursiveMutex & ); // No assignment

  Usul::Threads::Mutex *_mutex;
  Usul::Threads::Mutex *_local;
  unsigned int _count;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_RECURSIVE_MUTEX_CLASS_H_
