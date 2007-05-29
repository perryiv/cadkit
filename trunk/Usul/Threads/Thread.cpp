
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Thread.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>
#include <iostream>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Thread::Thread() : BaseClass(),
  _state          ( Thread::NOT_RUNNING ),
  _result         ( Thread::NORMAL ),
  _mutex          (),
  _cancelledCB    ( 0x0 ),
  _destroyedCB    ( 0x0 ),
  _errorCB        ( 0x0 ),
  _finishedCB     ( 0x0 ),
  _startedCB      ( 0x0 ),
  _errorMessage   (),
  _id             ( Manager::instance().nextThreadId() ),
  _systemId       ( 0 ),
  _creationThread ( Usul::Threads::currentThreadId() ),
  _task           ( 0, false )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Thread::~Thread()
{
  USUL_TRACE_SCOPE;

  USUL_ERROR_STACK_CATCH_EXCEPTIONS_BEGIN;

  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Thread::_notifyDestroyed ), "2055113901" );

  if ( Thread::NOT_RUNNING != _state )
  {
    std::cout << Usul::Strings::format ( "Error 4212506063: deleting running thread: ", this, ", id: ", _id, ", system thread: ", _systemId,  '\n' );
    std::cout << std::flush;
    USUL_ASSERT ( false );
  }

  _cancelledCB = 0x0;
  _destroyedCB = 0x0;
  _errorCB = 0x0;
  _finishedCB = 0x0;
  _startedCB = 0x0;
  _errorMessage.clear();

  USUL_ERROR_STACK_CATCH_EXCEPTIONS_END(1177998049);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

Thread::Mutex &Thread::mutex() const
{
  USUL_TRACE_SCOPE;
  return _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::cancelled ( Callback *cb )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _cancelledCB = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::destroyed ( Callback *cb )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _destroyedCB = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::error ( Callback *cb )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _errorCB = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::finished ( Callback *cb )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _finishedCB = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::started ( Callback *cb )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _startedCB = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_setState ( Thread::State state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _state = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the result.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_setResult ( Thread::Result result )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _result = result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the result.
//
///////////////////////////////////////////////////////////////////////////////

Thread::Result Thread::result() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state.
//
///////////////////////////////////////////////////////////////////////////////

Thread::State Thread::state() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::cancel()
{
  USUL_TRACE_SCOPE;

  // Set the flag.
  this->_setResult ( Thread::CANCELLED );

  // Get the thread ids.
  const unsigned long currentThread ( Usul::Threads::currentThreadId() );
  const unsigned long systemThread  ( this->systemId() );

  // If the current thread is the system thread that we created...
  if ( systemThread == currentThread )
  {
    throw Usul::Exceptions::Cancelled();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Schedules the thread and immediately returns.
//
//  TODO: Need mutex for this function or the "running" state. No other way 
//  to guard this function against re-entrance.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::start()
{
  USUL_TRACE_SCOPE;

  // We should not be running.
  if ( Thread::NOT_RUNNING != this->state() )
  {
    throw Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 4293851626: thread ", this->id(), " is already running or scheduled to run" );
  }

  // Using scope to release lock before we start thread.
  {
    // Lock access and set all flags together.
    Guard guard ( this->mutex() );

    // Initialize the error.
    this->_setError ( "" );

    // Set the state and result.
    this->_setState ( Thread::SCHEDULED );
    this->_setResult ( Thread::NORMAL );
  }

  // Start the thread.
  this->_start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the thread.
//
//  TODO: Need mutex for this function or the "running" state. No other way 
//  to guard this function against re-entrance.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_execute()
{
  USUL_TRACE_SCOPE;

  try
  {
    // Initialize.
    bool hasError ( false );

    // This thread should not be the one that created us.
    if ( Usul::Threads::currentThreadId() == this->_getCreationThread() )
    {
      this->_reportErrorNoThrow ( Usul::Strings::format ( "Error 1746088330: calling thread ", this->id(), "'s _execute() from the system thread that created it" ) );
      hasError = true;
    }

    // Are we already running?
    else if ( Thread::RUNNING == this->state() )
    {
      this->_reportErrorNoThrow ( Usul::Strings::format ( "Error 2551583580: thread ", this->id(), " is already running" ) );
      hasError = true;
    }

    // We should be scheduled to run.
    else if ( Thread::SCHEDULED != this->state() )
    {
      this->_reportErrorNoThrow ( Usul::Strings::format ( "Error 3288388842: thread ", this->id(), " is not scheduled to run" ) );
      hasError = true;
    }

    // Is there an error?
    if ( hasError )
    {
      this->_setResult ( Thread::ERROR_RESULT );
      this->_notifyError();
    }

    // Otherwise, no error yet...
    else
    {
      // Set the system thread ID.
      this->_setSystemThreadId ( Usul::Threads::currentThreadId() );

      // Set the state and result.
      this->_setState  ( Thread::RUNNING );
      this->_setResult ( Thread::NORMAL  );

      // Notify start callback.
      this->_notifyStarted();

      // Notify finished callback.
      this->_notifyFinished();

      // Set the state and result.
      this->_setResult ( Thread::NORMAL );
    }
  }

  // Handle special exception.
  catch ( const Usul::Exceptions::Cancelled & )
  {
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Thread::_setResult ), Thread::CANCELLED, "9390472160" );
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Thread::_notifyCancelled ), "1672077110" );
  }

  // Handle standard exceptions.
  catch ( const std::exception &e )
  {
    this->_reportErrorNoThrow ( Usul::Strings::format ( "Error 3474512825: standard exception caught while running thread ", this->id(), ", ", e.what() ) );
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Thread::_setResult ), Thread::ERROR_RESULT, "1486236410" );
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Thread::_notifyError ), "2480505155" );
  }

  // Handle unknown exceptions.
  catch ( ... )
  {
    this->_reportErrorNoThrow ( Usul::Strings::format ( "Error 1668279636: unknown exception caught while running thread ", this->id() ) );
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Thread::_setResult ), Thread::ERROR_RESULT, "4044605042" );
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Thread::_notifyError ), "2253082170" );
  }

  // Always reset this, and in this order.
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Thread::_setSystemThreadId ), 0, "3250307774" );
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Thread::_setState ), Thread::NOT_RUNNING, "1283621892" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to call callbacks.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void notify ( Thread *thread, Thread::Callback::RefPtr &cb )
  {
    USUL_TRACE_SCOPE_STATIC;

    // Handle bad input.
    if ( 0x0 == thread )
    {
      return;
    }

    // Get a copy of the callback.
    Callback::RefPtr copy ( 0x0 );
    {
      // Don't lock the mutex for long.
      Thread::Guard guard ( thread->mutex() );
      copy = cb;
    }

    // If the callback is valid then call it.
    if ( true == copy.valid() )
    {
      (*copy) ( thread );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyCancelled()
{
  USUL_TRACE_SCOPE;
  Helper::notify ( this, _cancelledCB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyDestroyed()
{
  USUL_TRACE_SCOPE;
  Helper::notify ( this, _destroyedCB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyError()
{
  USUL_TRACE_SCOPE;
  Helper::notify ( this, _errorCB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyFinished()
{
  USUL_TRACE_SCOPE;
  Helper::notify ( this, _finishedCB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyStarted()
{
  USUL_TRACE_SCOPE;
  Helper::notify ( this, _startedCB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the thread's ID. This is not the system's ID for the thread.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Thread::id() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the error string.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_setError ( const std::string &s )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _errorMessage = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  For reporting errors when you cannot throw.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_reportErrorNoThrow ( const std::string &s )
{
  USUL_TRACE_SCOPE;
  try
  {
    if ( false == s.empty() )
    {
      this->_setError ( s );
      std::cout << ( ( '\n' != s.at ( s.size() - 1 ) ) ? ( s + "\n" ) : s ) << std::endl;
      Usul::Errors::Stack::instance().push ( s );
    }
  }
  catch ( ... )
  {
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the system thread ID that created this instance.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Thread::_getCreationThread() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _creationThread;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the system thread ID for the thread that this instance encapsulates.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_setSystemThreadId ( unsigned long id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _systemId = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the system thread ID for the thread that this instance encapsulates.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Thread::systemId() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _systemId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the thread have the state?
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::hasState ( State s ) const
{
  USUL_TRACE_SCOPE;
  return ( this->state() == s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the thread have the result?
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::hasResult ( Result r ) const
{
  USUL_TRACE_SCOPE;
  return ( this->result() == r );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if it's ok to delete this instance.
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::isIdle() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  const bool notRunning ( Thread::NOT_RUNNING == this->state() );
  const bool zeroSystemThread ( 0 == this->systemId() );

  return ( notRunning && zeroSystemThread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the task id.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::task ( unsigned long task )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _task.first = task;
  _task.second = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the task id.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Thread::task() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _task.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this thread executing the thread-pool task?
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::isTask() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _task.second;
}
