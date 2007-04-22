
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
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

Pool *Pool::_instance = 0x0;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Pool::Pool() :
  _mutex      (),
  _pool       (),
  _queued     (),
  _tasks      (),
  _nextTaskId ( 0 ),
  _thread     ( Thread::create() )
{
  USUL_TRACE_SCOPE;

  // Set callbacks for the internal thread.
  _thread->started   ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_started   ) ) );
  _thread->cancelled ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_cancelled ) ) );
  _thread->error     ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_error     ) ) );
  _thread->finished  ( newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_finished  ) ) );

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
  USUL_ERROR_STACK_CATCH_EXCEPTIONS_BEGIN;

  _pool.clear();
  _queued.clear();
  _tasks.clear();
  _thread = 0x0;

  USUL_ERROR_STACK_CATCH_EXCEPTIONS_END(4272162595);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton construction.
//
///////////////////////////////////////////////////////////////////////////////

Pool &Pool::instance()
{
  USUL_TRACE_SCOPE;
  if ( 0x0 == _instance )
  {
    _instance = new Pool;
  }
  return *_instance;
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

Pool::TaskHandle Pool::add ( Callback *started, Callback *finished, Callback *cancelled, Callback *error )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Task::RefPtr task ( new Task ( _nextTaskId++, started, finished, cancelled, error ) );
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
//  Get the number of threads with the given state.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numThreads ( const Usul::Threads::Thread::State &state ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  unsigned int count ( static_cast<unsigned int> ( std::count_if ( _pool.begin(), _pool.end(), 
    std::bind2nd ( std::mem_fun ( &Usul::Threads::Thread::hasState ), state ) ) ) );

  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of threads with the given result.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numThreads ( const Usul::Threads::Thread::Result &result ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  unsigned int count ( static_cast<unsigned int> ( std::count_if ( _pool.begin(), _pool.end(), 
    std::bind2nd ( std::mem_fun ( &Usul::Threads::Thread::hasResult ), result ) ) ) );

  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of threads.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::numThreads ( unsigned int request, bool allowThrow )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Determine number of non-idle (active) threads.
  unsigned int idle ( this->numThreads ( Thread::NOT_RUNNING ) );
  unsigned int size ( this->numThreads() );
  USUL_ASSERT ( idle <= size );
  unsigned int active ( size - idle );

  // If the new size will hold all the non-idle threads...
  if ( request >= active )
  {
    // Put all active threads at the beginning of the sequence.
    ThreadPool::iterator end ( std::remove_if ( _pool.begin(), _pool.end(), 
      std::bind2nd ( std::mem_fun ( &Usul::Threads::Thread::hasState ), Thread::NOT_RUNNING ) ) );

    // Should be true.
    USUL_ASSERT ( static_cast < unsigned int > ( std::distance ( _pool.begin(), end ) ) <= request );

    // Trim to the new size.
    _pool.resize ( request );
  }

  // Otherwise...
  else
  {
    if ( true == allowThrow )
    {
      throw std::runtime_error ( "Error 4967564190: Number of non-idle threads exceeds the requested thread-pool size" );
    }
  }
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
//  Called when the internal thread starts.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_started ( Usul::Threads::Thread * )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called if the internal thread is cancelled.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_cancelled ( Usul::Threads::Thread * )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called if the internal thread has an error.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_error ( Usul::Threads::Thread * )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the internal thread finishes.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_finished ( Usul::Threads::Thread * )
{
}
