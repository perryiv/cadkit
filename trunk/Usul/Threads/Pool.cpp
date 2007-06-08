
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread pool class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Pool.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/System/Clock.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <limits>

using namespace Usul::Threads;

USUL_IMPLEMENT_TYPE_ID ( Pool );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Pool, Pool::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Pool::Pool ( unsigned int numThreads ) : BaseClass(),
  _mutex      (),
  _pool       (),
  _queued     (),
  _tasks      (),
  _nextTaskId ( 0 ),
  _thread     ( Usul::Threads::Manager::instance().create() ),
  _sleep      ( 100 )
{
  USUL_TRACE_SCOPE;

  // Populate pool.
  _pool.reserve ( numThreads );
  for ( unsigned int i = 0; i < numThreads; ++i )
  {
    Usul::Threads::Thread::RefPtr thread ( Usul::Threads::Manager::instance().create() );
    _pool.push_back ( thread );
  }

  // Set callbacks for the internal thread.
  _thread->started   ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_threadStarted   ) ) );
  _thread->cancelled ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_threadCancelled ) ) );
  _thread->error     ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_threadError     ) ) );
  _thread->finished  ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_threadFinished  ) ) );

  // Start the internal thread.
  _thread->start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Pool::~Pool()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Pool::_destroy ), "4142774520" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the members.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_destroy()
{
  USUL_TRACE_SCOPE;

  // Do not lock mutex up here.

  // Cancel all the tasks.
  this->cancel();

  // Wait for threads to finish.
  this->_waitForThreads ( 100000 );

  // Now delete internal thread.
  if ( 0x0 != _thread && false == _thread->isIdle() )
  {
    _thread->cancel();
    while ( false == _thread->isIdle() )
    {
      Usul::System::Sleep::milliseconds ( 100 );
    }
  }

  // Don't lock mutex until we get here.
  {
    Guard guard ( this->mutex() );

    // Destroy members.
    _thread = 0x0;
    _pool.clear();
    _queued.clear();
    _tasks.clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Pool::Mutex &Pool::mutex() const
{
  USUL_TRACE_SCOPE;
  return _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a task.
//
///////////////////////////////////////////////////////////////////////////////

Pool::TaskHandle Pool::addTask ( Callback *started, Callback *finished, Callback *cancelled, Callback *error, Callback *destroyed )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Task::RefPtr task ( new Task ( _nextTaskId++, started, finished, cancelled, error, destroyed ) );
  _queued.push_back ( task );
  _tasks[task->id()] = task;

  return task->id();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the pool have the task.
//
///////////////////////////////////////////////////////////////////////////////

bool Pool::hasTask ( TaskHandle id ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  AllTasks::const_iterator i ( _tasks.find ( id ) );
  return ( _tasks.end() != i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of threads in the pool.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numThreads() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _pool.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of threads that are idle.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numThreadsIdle() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  unsigned int count ( static_cast<unsigned int> ( std::count_if ( _pool.begin(), _pool.end(), 
    std::mem_fun ( &Usul::Threads::Thread::isIdle ) ) ) );

  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of tasks that are waiting for idle threads.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numTasksQueued() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _queued.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the task.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::remove ( TaskHandle id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  AllTasks::iterator i ( _tasks.find ( id ) );
  if ( _tasks.end() != i )
  {
    Task::RefPtr task ( i->second );
    _tasks.erase ( i );
    _queued.remove ( task );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the internal thread is running.
//
///////////////////////////////////////////////////////////////////////////////

bool Pool::_isRunning() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 != _thread ) ? ( Usul::Threads::Thread::RUNNING == _thread->state() ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the internal thread starts.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_threadStarted ( Usul::Threads::Thread * )
{
  USUL_TRACE_SCOPE;
  // Do not lock mutex here!

  // Check thread.
  if ( Usul::Threads::currentThreadId() != _thread->systemId() )
    throw std::runtime_error ( "Error 2565542508: wrong thread in start function" );

  // Loop until this thread is cancelled or this instance is deleted.
  while ( this->_isRunning() )
  {
    // Sleep so that this thread doesn't take over.
    Usul::System::Sleep::milliseconds ( this->sleepDuration() );

    // Process any queued tasks.
    this->_threadProcessTasks();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the internal thread starts.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_threadProcessTasks()
{
  USUL_TRACE_SCOPE;
  // Do not lock mutex here!

  // Check thread.
  if ( Usul::Threads::currentThreadId() != _thread->systemId() )
    throw std::runtime_error ( "Error 1154876700: wrong thread in task-processing function" );

  // Check to see if we've been cancelled.
  if ( Thread::CANCELLED == _thread->result() )
    throw Usul::Exceptions::Cancelled();

  // If there are any queued tasks...
  const unsigned int queued ( this->numTasksQueued() );
  if ( queued > 0 )
  {
    // Are there any idle threads?
    const unsigned int idle ( this->numThreadsIdle() );
    if ( idle > 0 )
    {
      Guard guard ( this->mutex() );

      // Get next available thread.
      Usul::Threads::Thread::RefPtr thread ( this->_firstAvailableThread() );
      if ( true == thread.valid() )
      {
        // Get the next task.
        Task::RefPtr task ( this->_nextTask() );
        if ( true == task.valid() )
        {
          // Set callbacks.
          thread->started   ( task->startedCB()   );
          thread->finished  ( task->finishedCB()  );
          thread->cancelled ( task->cancelledCB() );
          thread->error     ( task->errorCB()     );
          thread->destroyed ( task->destroyedCB() );

          // Set the task id.
          thread->task ( task->id() );

          // Start the thread.
          thread->start();
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called if the internal thread is cancelled.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_threadCancelled ( Usul::Threads::Thread * )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called if the internal thread has an error.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_threadError ( Usul::Threads::Thread * )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the internal thread finishes.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_threadFinished ( Usul::Threads::Thread * )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the next task.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Task *Pool::_nextTask()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Task::RefPtr task ( 0x0 );

  if ( false == _queued.empty() )
  {
    task = _queued.front();
    _queued.pop_front();
  }

  return task.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the first available thread.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Thread *Pool::_firstAvailableThread()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  ThreadPool::iterator i ( std::find_if ( _pool.begin(), _pool.end(), std::mem_fun ( &Thread::isIdle ) ) );
  return ( ( _pool.end() == i ) ? 0x0 : i->get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the sleep duration.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Pool::sleepDuration() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _sleep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the sleep duration.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::sleepDuration ( unsigned long duration )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _sleep = duration;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel all tasks.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::cancel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Loop through theads.
  for ( ThreadPool::iterator i = _pool.begin(); i != _pool.end(); ++i )
  {
    Thread::RefPtr thread ( *i );
    if ( true == thread.valid() )
    {
      if ( Thread::RUNNING == thread->state() )
      {
        thread->cancel();
      }
    }
  }

  // Clear the tasks.
  _queued.clear();
  _tasks.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all tasks to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::wait ( unsigned long timeout )
{
  USUL_TRACE_SCOPE;

  // Save current time.
  const unsigned long start ( static_cast<unsigned long> ( Usul::System::Clock::milliseconds() ) );

  // While there are tasks...
  while ( this->numTasksQueued() )
  {
    // Sleep some so that we don't spike the cpu.
    Usul::System::Sleep::milliseconds ( 100 );

    // Check the time.
    const unsigned long now ( static_cast<unsigned long> ( Usul::System::Clock::milliseconds() ) );
    if ( now - start > timeout )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3360169001: exceeded allowable time period of ", 
          static_cast<double> ( timeout ) / 1000.0, 
          " seconds while waiting for tasks" );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all tasks to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::wait()
{
  USUL_TRACE_SCOPE;
  this->wait ( std::numeric_limits<unsigned long>::max() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all threads to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_waitForThreads ( unsigned long timeout )
{
  USUL_TRACE_SCOPE;

  // Save current time.
  const unsigned long start ( static_cast<unsigned long> ( Usul::System::Clock::milliseconds() ) );

  // Copy the pool.
  typedef std::list<Thread::RefPtr> ThreadList;
  ThreadList pool;
  {
    Guard guard ( this->mutex() );
    pool.assign ( _pool.begin(), _pool.end() );
  }

  // While there are threads...
  while ( false == pool.empty() )
  {
    // Remove the ones that are idle.
    pool.remove_if ( std::mem_fun ( &Thread::isIdle ) );

    // Sleep some so that we don't spike the cpu.
    Usul::System::Sleep::milliseconds ( 100 );

    // Check the time.
    const unsigned long now ( static_cast<unsigned long> ( Usul::System::Clock::milliseconds() ) );
    if ( now - start > timeout )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3331992936: exceeded allowable time period of ", 
          static_cast<double> ( timeout ) / 1000.0, 
          " seconds while waiting for threads" );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Pool::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

	switch ( iid )
	{
	case Usul::Interfaces::IUnknown::IID:
	case Usul::Interfaces::IThreadPoolAddTask::IID:
		return static_cast < Usul::Interfaces::IThreadPoolAddTask* > ( this );
	default:
		return 0x0;
	}
}
