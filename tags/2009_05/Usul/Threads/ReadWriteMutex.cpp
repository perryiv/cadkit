
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
#include "Usul/Threads/Safe.h"
#include "Usul/Threads/ThreadId.h"

#include <set>
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
  typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;
  typedef std::set<unsigned long> Threads;

  ReadWriteMutexImpl ( unsigned long writeLockWaitSleep );
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

  bool            _hasLocks() const;

  void            _threadAdd();
  void            _threadHasLock ( bool has );
  void            _threadHasWriteLock ( bool has );
  void            _threadRemove();
  Threads         _threadsGet() const;
  std::string     _threadsToString() const;

  void            _destroy();

  Usul::Threads::Mutex *_writeMutex;
  unsigned long _writeLockThread;
  Usul::Threads::Mutex *_local;
  Threads _threads;
  unsigned long _sleep;
}; } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadWriteMutex::ReadWriteMutex ( unsigned long writeLockWaitSleep ) :
  _mutex ( new Usul::Threads::Detail::ReadWriteMutexImpl ( writeLockWaitSleep ) )
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

ReadWriteMutexImpl::ReadWriteMutexImpl ( unsigned long writeLockWaitSleep ) :
  _writeMutex ( new Usul::Threads::Mutex ),
  _writeLockThread ( 0 ),
  _local ( new Usul::Threads::Mutex ),
  _threads(),
  _sleep ( writeLockWaitSleep )
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
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::_destroy()
{
  // Need local scope.
  {
    Guard local ( *_local );

    // Should be true.
    if ( false == _threads.empty() )
    {
      _threads.clear();
      throw std::runtime_error ( "Error 1777649577: Set of threads is not empty. Is the mutex still locked?" );
    }

    // Should be true.
    if ( 0 != _writeLockThread )
    {
      throw std::runtime_error ( Usul::Strings::format ( "Error 3757046843: Thread ", _writeLockThread, " still has the write-lock" ) );
    }

    // Delete the these mutexes.
    delete _writeMutex; _writeMutex = 0x0;
  }

  // Done with the local mutex.
  delete _local; _local = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if there are any locks.
//
///////////////////////////////////////////////////////////////////////////////

bool ReadWriteMutexImpl::_hasLocks() const
{
  Guard guard ( *_local );
  return ( false == _threads.empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Try to add the current thread.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::_threadAdd()
{
  Guard guard ( *_local );

  // Add this thread to the set.
  typedef std::pair<Threads::iterator,bool> Result;
  const unsigned long thread ( Usul::Threads::currentThreadId() );
  const Result result ( _threads.insert ( thread ) );

  // Was this thread already in the set?
  if ( false == result.second )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3705275995: thread ", thread, " already has a read-lock" ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Try to remove the current thread.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::_threadRemove()
{
  Guard guard ( *_local );

  // Look for the thread in the set.
  const unsigned long thread ( Usul::Threads::currentThreadId() );
  Threads::iterator i ( _threads.find ( thread ) );

  // Did we find it?
  if ( _threads.end() == i )
  {
    throw std::runtime_error ( Usul::Strings::format 
      ( "Error 3763892839: thread ", thread, " does not have a read-lock" ) );
  }

  // Erase the entry.
  _threads.erase ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the current thread is in the set.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::_threadHasLock ( bool has )
{
  Guard guard ( *_local );

  // Look for the thread in the set.
  const unsigned long thread ( Usul::Threads::currentThreadId() );
  Threads::iterator i ( _threads.find ( thread ) );

  if ( has )
  {
    if ( _threads.end() == i )
    {
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 3446629783: thread ", thread, " does not have a read-lock" ) );
    }
  }
  else
  {
    if ( _threads.end() != i )
    {
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 1144823296: thread ", thread, " already has a read-lock" ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the current thread has the write lock.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::_threadHasWriteLock ( bool has )
{
  Guard guard ( *_local );

  const unsigned long thread ( Usul::Threads::currentThreadId() );

  if ( has )
  {
    if ( thread != _writeLockThread )
    {
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 8090541550: thread ", thread, " does not have the write-lock, ", _writeLockThread, " does" ) );
    }
  }
  else
  {
    if ( thread == _writeLockThread )
    {
      throw std::runtime_error ( Usul::Strings::format 
        ( "Error 1220405437: thread ", thread, " already has the write-lock" ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a copy of the threads with locks.
//
///////////////////////////////////////////////////////////////////////////////

ReadWriteMutexImpl::Threads ReadWriteMutexImpl::_threadsGet() const
{
  Guard guard ( *_local );
  return _threads;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the threads to space-delimited string.
//
///////////////////////////////////////////////////////////////////////////////

std::string ReadWriteMutexImpl::_threadsToString() const
{
  Threads threads ( this->_threadsGet() );
  if ( true == threads.empty() )
    return std::string();

  std::ostringstream s;
  for ( Threads::const_iterator i = threads.begin(); i != threads.end(); ++i )
    s << *i << ' ';

  std::string answer ( s.str() );
  answer.erase ( answer.begin() + answer.size() - 1 );
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex for reading.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::readLock()
{
  // Make sure this thread does not already have a lock.
  this->_threadHasLock ( false );

  // Make sure this thread does not already have the write-lock.
  this->_threadHasWriteLock ( false );

  // Lock for writing and release when we return.
  Guard write ( *_writeMutex );

  // Add this thread to the set.
  this->_threadAdd();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex for writing.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::writeLock()
{
  // Make sure this thread does not already have a lock.
  this->_threadHasLock ( false );

  // Make sure this thread does not already have the write-lock.
  this->_threadHasWriteLock ( false );

  // Lock for writing.
  Guard write ( *_writeMutex );

  // Spin until there are no read-locks.
  const unsigned long thread ( Usul::Threads::currentThreadId() );
  while ( true == this->_hasLocks() )
  {
    #ifdef _DEBUG
    std::cout << Usul::Strings::format ( "Thread ", thread, " waiting for read-locks held by threads: ", this->_threadsToString(), '\n' ) << std::flush;
    #endif

    Usul::System::Sleep::milliseconds ( _sleep );
  }

  // Add this thread to the set.
  this->_threadAdd();

  // We have the write-lock.
  Usul::Threads::Safe::set ( *_local, Usul::Threads::currentThreadId(), _writeLockThread );

  // Hang on to the lock.
  write.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex for reading.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::readUnlock()
{
  // Make sure this thread has a lock.
  this->_threadHasLock ( true );

  // Should not have the write-lock.
  this->_threadHasWriteLock ( false );

  // Remove the read-lock.
  this->_threadRemove();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex for writing.
//
///////////////////////////////////////////////////////////////////////////////

void ReadWriteMutexImpl::writeUnlock()
{
  // Make sure this thread has a lock.
  this->_threadHasLock ( true );

  // Should own the write-lock.
  this->_threadHasWriteLock ( true );

  // Remove this thread from the set.
  this->_threadRemove();

  // Nobody has the write-lock.
  Usul::Threads::Safe::set ( *_local, 0, _writeLockThread );

  // Unlock the mutex.
  _writeMutex->unlock();
}
