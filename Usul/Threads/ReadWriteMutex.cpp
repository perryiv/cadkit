
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive multiple-reader, single-writer mutex class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/ReadWriteMutex.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Threads/ThreadId.h"

#include <map>
#include <stdexcept>

using namespace Usul::Threads;
using namespace Usul::Threads::Detail;


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul { namespace Threads { namespace Detail {
struct USUL_EXPORT ReadWriteMutexImpl
{
  typedef Usul::Threads::Guard<Usul::Threads::RecursiveMutex> RecursiveGuard;
  typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;
  typedef std::map<unsigned long,unsigned long> RefCounts;

  ReadWriteMutexImpl();
  ~ReadWriteMutexImpl();

  // Lock/unlock for reading.
  void            readLock();
  void            readUnlock();

  // Lock/unlock for writing.
  void            writeLock();
  void            writeUnlock();

private:

  ReadWriteMutexImpl ( const ReadWriteMutexImpl & );             // No copying
  ReadWriteMutexImpl &operator = ( const ReadWriteMutexImpl & ); // No assignment

  void            _destroy();

  Usul::Threads::RecursiveMutex *_writeMutex;
  Usul::Threads::Mutex *_local;
  RefCounts _refCounts;
}; } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadWriteMutex::ReadWriteMutex() :
  _mutex ( new Usul::Threads::Detail::ReadWriteMutexImpl )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadWriteMutex::~ReadWriteMutex()
{
  delete _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex for reading.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutex::readLock()
{
  _mutex->readLock();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex for reading.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutex::readUnlock()
{
  _mutex->readUnlock();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex for writing.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutex::writeLock()
{
  _mutex->writeLock();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex for writing.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutex::writeUnlock()
{
  _mutex->writeUnlock();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadWriteMutexImpl::ReadWriteMutexImpl() :
  _writeMutex ( new Usul::Threads::RecursiveMutex ),
  _local ( new Usul::Threads::Mutex ),
  _refCounts()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadWriteMutexImpl::~ReadWriteMutexImpl()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ReadWriteMutexImpl::_destroy ), "3246582487" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the reference count for the given thread.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class MutexType, class RefCountContainer > 
  inline typename RefCountContainer::mapped_type &getRefCount ( MutexType &mutex, RefCountContainer &refCounts, unsigned long thread )
  {
    typedef Usul::Threads::Detail::ReadWriteMutexImpl Object;
    typedef typename Object::Guard Guard;
    typedef typename Object::RefCounts RefCounts;
    Guard guard ( mutex );
    RefCounts::mapped_type &count ( refCounts[thread] );
    return count;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if it is ok to take a write-lock.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class MutexType, class RefCountContainer > 
  inline bool canTakeWriteLock ( MutexType &mutex, RefCountContainer &refCounts )
  {
    typedef Usul::Threads::Detail::ReadWriteMutexImpl Object;
    typedef typename Object::Guard Guard;
    typedef typename Object::RefCounts RefCounts;

    Guard guard ( mutex );

    if ( true == refCounts.empty() )
    {
      // There are no read-locks.
      return true;
    }

    if ( 1 == refCounts.size() )
    {
      if ( Usul::Threads::currentThreadId() == refCounts.begin()->first )
      {
        // There is one read-lock and it is this thread.
        return true;
      }
      else
      {
        // There is one read-lock but it's not this thread.
        return false;
      }
    }

    // There are more than one read-locks.
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::_destroy()
{
  // Need local scope.
  {
    Guard local ( *_local );

    // Should be true.
    if ( false == _refCounts.empty() )
    {
      _refCounts.clear();
      throw std::runtime_error ( "Error 1777649577: Map of reference counts is not empty. Is the mutex still locked?" );
    }

    // Delete the these mutexes.
    delete _writeMutex; _writeMutex = 0x0;
  }

  // Done with the local mutex.
  delete _local; _local = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex for reading.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::readLock()
{
  // If there is a write-lock then this will block unless the 
  // thread that holds the write-lock is the calling thread 
  // (because _writeMutex is recursive). In either case, when the 
  // write-lock is aquired it is immediately released.
  RecursiveGuard write ( *_writeMutex );

  // We have one more read-lock for this thread. It's ok to operate directly 
  // on this reference because an std::map does not invalidate iterators
  // (or references to items).
  RefCounts::mapped_type &count ( Helper::getRefCount ( *_local, _refCounts, Usul::Threads::currentThreadId() ) );
  ++count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex for writing.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::writeLock()
{
  // A thread gets a write-lock if there are no locks or if the only 
  // existing lock (read or write) is held by the same thread.
  const unsigned long duration ( 100 );
  while ( true )
  {
    // Need local scope because we don't want this lock if we sleep.
    {
      // Try to get the write-lock.
      RecursiveGuard write ( *_writeMutex );

      // Are we allowed to have the write-lock?
      if ( true == Helper::canTakeWriteLock ( *_local, _refCounts ) )
      {
        write.release();
        return;
      }
    }

    #if 0
    const unsigned long thread ( Usul::Threads::currentThreadId() );
    std::ostringstream threads;
    const RefCounts refCounts ( Usul::Threads::Safe::get ( *_local, _refCounts ) );
    for ( RefCounts::const_iterator i = refCounts.begin(); i != refCounts.end(); ++i )
      threads << i->first << ' ';
    std::cout << Usul::Strings::format ( "Thread ", thread, " trying to take write-lock, waiting on read-locks held by threads: ", threads.str(), '\n' ) << std::flush;
    #endif

    // Sleep here to give other threads a chance to proceed.
    Usul::System::Sleep::milliseconds ( duration );
  }
}

#if 0

I'm concluding that this is a flawed idea. If n-number of threads each have 
read-locks, and their paths through the code require them to get write-locks, 
and, when they try to get write-locks they have to wait on the read-locks, 
then you create a "infinite wait" situation. Not exactly deadlock, but 
effectively the same thing.

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex for reading.
//
//  Note: taking advantage of the fact that std::map entries remain valid 
//  even when the structure of the map may change. In other words, if we 
//  get a reference to an item in the map, if another thread inserts or 
//  removes another item, our reference is still valid. This means we only 
//  have to guard reads and writes to _refCount and not its items.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::readUnlock()
{
  // This container should not be empty.
  {
    Guard local ( *_local );
    if ( true == _refCounts.empty() )
    {
      throw std::runtime_error ( "Error 1957225110: Attempting to release a read-lock when there are none" );
    }
  }

  // Get a reference to the reference count.
  const unsigned long thread ( Usul::Threads::currentThreadId() );
  RefCounts::mapped_type &count ( Helper::getRefCount ( *_local, _refCounts, thread ) );

  // There should be at least one read-lock for this thread
  if ( 0 == count )
  {
    throw std::runtime_error ( "Error 7469268180: This thread does not hold a read-lock" );
  }

  // We have one less read-lock for this thread.
  --count;

  // If we're now at zero then remove the entry.
  if ( 0 == count )
  {
    Guard local ( *_local );
    _refCounts.erase ( thread );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex for writing.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::writeUnlock()
{
  // No conditions required. Just unlock it.
  _writeMutex->unlock();
}
