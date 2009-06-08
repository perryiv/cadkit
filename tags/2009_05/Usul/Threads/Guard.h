
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


template < class MutexType_ > class Guard
{
public:

  typedef MutexType_ MutexType;

  // Constructor. Locks the mutex.
  Guard ( MutexType &m ) : _mutex ( m ), _isLocked ( true )
  {
    _mutex.lock();
  }

  // Constructor. Locks the mutex.
  template < class ObjectType > Guard ( ObjectType *object ) : _mutex ( object->mutex() ), _isLocked ( true )
  {
    _mutex.lock();
  }

  // Destructor. Unlocks the mutex.
  ~Guard()
  {
    this->unlock();
  }

  // Call to explicitely unlock the mutex.
  void unlock()
  {
    if ( _isLocked )
    {
      _isLocked = false;
      _mutex.unlock();
    }
  }

  // Prevent the automatic unlocking of the mutex.
  void release()
  {
    _isLocked = false;
  }

protected:

  MutexType &_mutex;
  bool _isLocked;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_GUARD_CLASS_H_
