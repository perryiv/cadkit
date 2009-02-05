
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
#include "Usul/Threads/Safe.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <ctime>

using namespace Usul::Threads;

USUL_IMPLEMENT_TYPE_ID ( Pool );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Pool::Pool ( const std::string &n, unsigned int numThreads, bool lazyStart ) : BaseClass(),
  _pool       (),
  _queue      (),
  _executing  (),
  _nextTaskId ( 0 ),
  _sleep      ( 10 ), // Was 100. Rational Quantify said it was a bottleneck.
  _runThreads ( true ),
  _started    ( false ),
  _log        ( 0x0 )
{
  USUL_TRACE_SCOPE;

  // Set the name.
  this->name ( n );

  // Populate pool. The threads are not started until the first task is added.
  _pool.reserve ( numThreads );
  for ( unsigned int i = 0; i < numThreads; ++i )
  {
    // Make thread.
    const std::string base ( ( true == n.empty() ) ? Usul::Strings::format ( "Usul::Thread::Pool ", this ) : n );
    const std::string name ( Usul::Strings::format ( base, ' ', i ) );
    Usul::Threads::Thread::RefPtr thread ( Usul::Threads::Manager::instance().create ( name ) );

    // Add to our pool.
    _pool.push_back ( thread );

    // Set callback.
    thread->started ( Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Pool::_threadStarted ) ) );
  }

  // Start threads now if we're supposed to.
  if ( false == lazyStart )
  {
    this->_startThreads();
  }
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
  // Do not lock mutex up here! Threads waiting for this mutex will never finish.

  // Turn off the switch.
  Usul::Threads::Safe::set ( this->mutex(), false, _runThreads );

  // Clear all queued tasks and cancel running threads.
  this->cancel();

  // Wait for threads to finish.
  this->_waitForThreads();

  // Should be true.
  USUL_ASSERT ( true == _queue.empty() );
  USUL_ASSERT ( true == _executing.empty() );

  // Remove the threads from the pool.
  {
    Guard guard ( this ); // Needed?
    _pool.clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a task.
//
///////////////////////////////////////////////////////////////////////////////

Pool::TaskHandle Pool::addTask ( int priority, Task *task )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Check input.
  if ( 0x0 == task )
    throw std::invalid_argument ( "Error 3285076575: null task given" );

  // Make handle.
  TaskHandle key ( priority, task->id() );

  // Add task. Local reference may help with stability.
  _queue[key] = Task::RefPtr ( task );

  // Make sure the threads are started.
  this->_startThreads();

  // Return key.
  return key;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a task.
//
///////////////////////////////////////////////////////////////////////////////

Pool::TaskHandle Pool::addTask ( int priority, Callback *started, Callback *finished, Callback *cancelled, Callback *error )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return this->addTask ( priority, new Task ( this->nextTaskId(), started, finished, cancelled, error ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the queued tasks and cancel the running threads.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::cancel()
{
  USUL_TRACE_SCOPE;

  // Clear all queued tasks. Has no effect on running threads. 
  // Do this before cancelling the threads.
  this->clearQueuedTasks();

  // Cancel all the threads.
  this->_cancelThreads();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the pool have the task.
//
///////////////////////////////////////////////////////////////////////////////

bool Pool::hasQueuedTask ( TaskHandle id ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  TaskMap::const_iterator i ( _queue.find ( id ) );
  return ( _queue.end() != i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of threads in the pool.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numThreads() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _pool.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of tasks that are waiting for idle threads.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numTasksQueued() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _queue.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of tasks that are executing.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numTasksExecuting() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _executing.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the names of the executing tasks.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::executingNames ( Strings &names ) const
{
  USUL_TRACE_SCOPE;
  const TaskSet executing ( Usul::Threads::Safe::get ( this->mutex(), _executing ) );
  names.clear();
  names.reserve ( executing.size() );
  for ( TaskSet::const_iterator i = executing.begin(); i != executing.end(); ++i )
  {
    Task::RefPtr task ( *i );
    names.push_back ( task->name() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of tasks.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Pool::numTasks() const
{
  USUL_TRACE_SCOPE;

  // Lock so that the answers are in sync.
  Guard guard ( this );

  // Get the numbers.
  const unsigned int queued ( this->numTasksQueued() );
  const unsigned int executing ( this->numTasksExecuting() );

  // Return the sum.
  return ( queued + executing );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the queued task.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::removeQueuedTask ( TaskHandle id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  TaskMap::iterator i ( _queue.find ( id ) );
  if ( _queue.end() != i )
  {
    Task::RefPtr task ( i->second );
    USUL_TRACE_6 ( "Task: priority = ", id.first, ", id = ", id.second, ( ( false == task->name().empty() ) ? ( ", name = '" + task->name() + "'" ) : "" ), " <-- Removed\n" );
    _queue.erase ( i );
  }
  else
  {
    USUL_TRACE_5 ( "Task: priority = ", id.first, ", id = ", id.second, " <-- Failed to remove" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the internal thread starts.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_threadStarted ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;
  // Do not lock mutex here!

  // Handle bad input.
  if ( 0x0 == thread )
    throw std::invalid_argument ( "Error 3376049975: null thread argument in start function" );

  // Check thread.
  if ( Usul::Threads::currentThreadId() != thread->systemId() )
    throw std::invalid_argument ( "Error 2565542508: wrong thread in start function" );

  // Loop until told otherwise.
  while ( true == Usul::Threads::Safe::get ( this->mutex(), _runThreads ) )
  {
    // Get the next task.
    Task::RefPtr task ( this->_nextTask() );
    if ( true == task.valid() )
    {
      USUL_TRACE_3 ( "Starting task: id = ", task->id(), ( ( false == task->name().empty() ) ? ( ", name = '" + task->name() + "'" ) : "" ) );

      // Process any queued tasks. Catch and eat all exceptions.
      Usul::Functions::safeCallV1V2 ( Usul::Adaptors::memberFunction ( this, &Usul::Threads::Pool::_threadProcessTasks ), task.get(), thread, "1232600780" );

      // Always decrement.
      { 
        Guard guard ( this );
        USUL_ASSERT ( false == _executing.empty() );
        _executing.erase ( task );
      }
      
      task = 0x0;
    }
    
    // Only sleep when the queue is empty (Task is null).
    // This vasty improves performace when processing tasks.
    // (From 10 to 30 times faster in some of the tests I ran.)
    else
    {
      Usul::System::Sleep::milliseconds ( this->sleepDuration() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called in the task processing loop.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_threadProcessTasks ( Usul::Threads::Task *task, Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;
  // Do not lock mutex here!

  // Handle bad input.
  if ( ( 0x0 == task ) || ( 0x0 == thread ) )
    return;

  // Get the callbacks.
  Usul::Threads::Callback::RefPtr started   ( task->startedCB()   );
  Usul::Threads::Callback::RefPtr finished  ( task->finishedCB()  );
  Usul::Threads::Callback::RefPtr cancelled ( task->cancelledCB() );
  Usul::Threads::Callback::RefPtr error     ( task->errorCB()     );

  // Safely...
  USUL_TRY_BLOCK
  {
    // Call the start callback.
    if ( true == started.valid() )
      (*started) ( thread );

    // Call the finished callback.
    if ( true == finished.valid() )
      (*finished) ( thread );
  }

  // Handle special exception.
  catch ( const Usul::Exceptions::Cancelled & )
  {
    // Call the cancelled callback.
    if ( true == cancelled.valid() )
      (*cancelled) ( thread );
  }

  // Handle standard exceptions.
  catch ( const std::exception &e )
  {
    // Feedback.
    std::cout << Usul::Strings::format ( "Error 3878127704: standard exception caught while running thread ", thread->id(), ", ", e.what(), '\n' ) << std::flush;

    // Call the error callback.
    if ( true == error.valid() )
      (*error) ( thread );
  }

  // Handle all other exceptions.
  catch ( ... )
  {
    // Call the error callback.
    if ( true == error.valid() )
      (*error) ( thread );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the next task. Make sure you return a copy of the smart-pointer! 
//  Otherwise, with multiple threads running at once, the task could be 
//  decremented by a different thread but after it's released here.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Task::RefPtr Pool::_nextTask()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( false == _queue.empty() )
  {
    // Since the task id numbers always increase, the map acts like 
    // a queue if we always grab from the beginning.
    Task::RefPtr task ( _queue.begin()->second );

    // Remove the task from the queue.
    _queue.erase ( _queue.begin() );

    // While we have the lock...
    _executing.insert ( task );

    // Return task.
    return task;
  }
  else
  {
    return Usul::Threads::Task::RefPtr ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the sleep duration.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Pool::sleepDuration() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
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
  Guard guard ( this );
  _sleep = duration;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel all threads.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_cancelThreads()
{
  USUL_TRACE_SCOPE;

  // Copy the pool.
  ThreadPool pool ( Usul::Threads::Safe::get ( this->mutex(), _pool ) );

  // Loop through theads.
  for ( ThreadPool::iterator i = pool.begin(); i != pool.end(); ++i )
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all queued tasks.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::clearQueuedTasks()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _queue.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all tasks to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::waitForTasks()
{
  USUL_TRACE_SCOPE;

  // While there are tasks...
  unsigned int num ( this->numTasks() );
  while ( num > 0 )
  {
    USUL_TRACE_3 ( "Trace 3685583618: Waiting on ", num, " tasks\n" );

    // Sleep some so that we don't spike the cpu.
    Usul::System::Sleep::milliseconds ( this->sleepDuration() );

    // Get the number of tasks left.
    num = this->numTasks();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all threads in the pool to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_waitForThreads()
{
  USUL_TRACE_SCOPE;

  // Copy the pool.
  typedef std::list<Thread::RefPtr> ThreadList;
  ThreadList pool;
  {
    Guard guard ( this );
    pool.assign ( _pool.begin(), _pool.end() );
  }

  Strings names;

  // While there are threads...
  while ( false == pool.empty() )
  {
    // Print the names of the remaining threads.
    for ( ThreadList::const_iterator i = pool.begin(); i != pool.end(); ++i )
    {
      Thread::RefPtr thread ( *i );
      if ( true == thread.valid() )
      {
        std::cout << Usul::Strings::format ( "Waiting for thread: ", thread->name() ) << std::endl;
        this->_logEvent ( "Waiting for thread", thread );
      }
    }

    // Remove the ones that are idle.
    pool.remove_if ( std::mem_fun ( &Thread::isIdle ) );

    // Sleep some so that we don't spike the cpu.
    Usul::System::Sleep::milliseconds ( this->sleepDuration() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the next task id. This will also increment the internal counter.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Pool::nextTaskId()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  unsigned long id ( _nextTaskId++ );
  return id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the threads.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_startThreads()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Only start once.
  if ( false == _started )
  {
    std::for_each ( _pool.begin(), _pool.end(), std::mem_fun ( &Usul::Threads::Thread::start ) );
    _started = true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the log.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::logSet ( LogPtr lp )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _log = lp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the log.
//
///////////////////////////////////////////////////////////////////////////////

Pool::LogPtr Pool::logGet()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return LogPtr ( _log );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Log the event.
//
///////////////////////////////////////////////////////////////////////////////

void Pool::_logEvent ( const std::string &s, Thread::RefPtr thread, std::ostream *stream )
{
  USUL_TRACE_SCOPE;

  if ( true == s.empty() )
    return;
  
  LogPtr file ( this->logGet() );
  std::string message;

  if ( true == thread.valid() )
  {
    message = Usul::Strings::format ( "clock: ", ::clock(), ", system thread: ", Usul::Threads::currentThreadId(), ", id: ", thread->id(), ", thread system id: ", thread->systemId(), ", address: ", thread.get(), ", pool: ", this, ", event: ", s );
  }
  else
  {
    message = Usul::Strings::format ( "clock: ", ::clock(), ", system thread: ", Usul::Threads::currentThreadId(), ", event: ", s );
  }

  if ( 0x0 != stream )
  {
    (*stream) << message << std::endl;
  }

  if ( true == file.valid() ) 
  {
    file->write ( message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if a higher-priority task is waiting.
//
///////////////////////////////////////////////////////////////////////////////

bool Pool::isHigherPriorityTaskWaiting ( int priority ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( false == _queue.empty() )
  {
    const TaskHandle &taskHandle ( _queue.begin()->first );
    if ( taskHandle.first < priority )
    {
      return true;
    }
  }

  return false;
}
