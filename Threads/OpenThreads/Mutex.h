
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
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_THREADS_EXTENSIONS_MUTEX_H_
#define _OPEN_THREADS_EXTENSIONS_MUTEX_H_

#include "Threads/OpenThreads/Export.h"

#include "Usul/Threads/Mutex.h"

namespace OpenThreads { class Mutex; };


namespace Threads {
namespace OT {


class OPEN_THREADS_EXTENSIONS_EXPORT Mutex : public Usul::Threads::Mutex
{
public:

  typedef Usul::Threads::Mutex BaseClass;

  Mutex();
  virtual ~Mutex();

  // Lock/unlock the mutex.
  virtual void            lock();
  virtual void            unlock();

protected:

  void                    _check();

private:

  Mutex ( const Mutex & );             // No copying
  Mutex &operator = ( const Mutex & ); // No assignment

  OpenThreads::Mutex *_mutex;
  unsigned long _threadId;
};


// Creation function.
OPEN_THREADS_EXTENSIONS_EXPORT Usul::Threads::Mutex *newOpenThreadsMutex();


}; // namespace OT
}; // namespace Threads


#endif // _OPEN_THREADS_EXTENSIONS_MUTEX_H_
