
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Locks for a read-write mutex.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_READ_WRITE_LOCK_CLASSES_H_
#define _USUL_THREADS_READ_WRITE_LOCK_CLASSES_H_


namespace Usul {
namespace Threads {


///////////////////////////////////////////////////////////////////////////////
//
//  Reader lock.
//
///////////////////////////////////////////////////////////////////////////////

template < class MutexType_ > struct ReadLock
{
  typedef MutexType_ MutexType;
  ReadLock ( MutexType &m ) : _mutex ( m )
  {
    _mutex.readLock();
  }
  template < class ObjectType > ReadLock ( ObjectType *object ) : _mutex ( object->mutex() )
  {
    _mutex.readLock();
  }
  ~ReadLock()
  {
    _mutex.readUnlock();
  }
private:
  MutexType &_mutex;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Writer lock.
//
///////////////////////////////////////////////////////////////////////////////

template < class MutexType_ > struct WriteLock
{
  typedef MutexType_ MutexType;
  WriteLock ( MutexType &m ) : _mutex ( m )
  {
    _mutex.writeLock();
  }
  template < class ObjectType > WriteLock ( ObjectType *object ) : _mutex ( object->mutex() )
  {
    _mutex.writeLock();
  }
  ~WriteLock()
  {
    _mutex.writeUnlock();
  }
private:
  MutexType &_mutex;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_READ_WRITE_LOCK_CLASSES_H_
