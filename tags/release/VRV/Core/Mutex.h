
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

#ifndef _VIRTUAL_REALITY_VIEWER_THREADS_MUTEX_CLASS_H_
#define _VIRTUAL_REALITY_VIEWER_THREADS_MUTEX_CLASS_H_

#include "VRV/Core/Export.h"

#include "Usul/Threads/Mutex.h"

#include "vpr/Sync/Mutex.h"


namespace VRV {
namespace Threads {


///////////////////////////////////////////////////////////////////////////////
//
//  Base mutex class.
//
///////////////////////////////////////////////////////////////////////////////

class VRV_EXPORT Mutex : public Usul::Threads::Mutex
{
public:

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

  vpr::Mutex _mutex;
  unsigned long _threadId;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Creation function.
//
///////////////////////////////////////////////////////////////////////////////

VRV_EXPORT Usul::Threads::Mutex *newMutex();


}; // namespace Threads
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_THREADS_MUTEX_CLASS_H_
