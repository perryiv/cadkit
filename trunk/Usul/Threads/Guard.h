
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread-guard class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_GUARD_CLASS_H_
#define _USUL_THREADS_GUARD_CLASS_H_


namespace Usul {
namespace Threads {


template < class MutexType > class Guard
{
public:

  // Constructor. Locks the mutex.
  Guard ( MutexType &m ) : _mutex ( m )
  {
    _mutex.lock();
  }

  // Destructor. Unlocks the mutex.
  ~Guard()
  {
    _mutex.unlock();
  }

protected:

  MutexType &_mutex;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_GUARD_CLASS_H_
