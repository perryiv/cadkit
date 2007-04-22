
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
#include "Usul/Errors/Assert.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>
#include <iostream>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

Thread::StaticMutex *Thread::_staticMutex ( 0x0 );
Thread::FactoryFunction *Thread::_fun ( 0x0 ); // Client has to set.
unsigned long Thread::_guiThread ( 0 );        // Client has to set.
unsigned long Thread::_nextThreadId ( 0 );


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
  _errorCB        ( 0x0 ),
  _finishedCB     ( 0x0 ),
  _startedCB      ( 0x0 ),
  _errorMessage   (),
  _id             ( _nextThreadId++ ),
  _systemId       ( 0 ),
  _creationThread ( Usul::Threads::currentThreadId() )
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

  USUL_ASSERT ( Thread::NOT_RUNNING == _state );

  _cancelledCB = 0x0;
  _errorCB = 0x0;
  _finishedCB = 0x0;
  _startedCB = 0x0;
  _errorMessage.clear();

  USUL_ERROR_STACK_CATCH_EXCEPTIONS_END(3271694255);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the thread. Client needs to set the function.
//
///////////////////////////////////////////////////////////////////////////////

Thread *Thread::create()
{
  USUL_TRACE_SCOPE;
  StaticGuard guard ( Thread::_getStaticMutex() );

  if ( 0x0 == _fun )
  {
    USUL_ASSERT ( _fun ); // The client needs to set this.
    throw std::runtime_error ( "Error 1689981392: No thread factory set" );
  }

  return (*_fun)();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the factory-function. Return the previous one.
//
///////////////////////////////////////////////////////////////////////////////

Thread::FactoryFunction *Thread::createFunction ( Thread::FactoryFunction *fun )
{
  USUL_TRACE_SCOPE;
  StaticGuard guard ( Thread::_getStaticMutex() );

  FactoryFunction *original = _fun;
  _fun = fun;
  return original;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the factory-function.
//
///////////////////////////////////////////////////////////////////////////////

Thread::FactoryFunction *Thread::createFunction()
{
  USUL_TRACE_SCOPE;
  StaticGuard guard ( Thread::_getStaticMutex() );
  return _fun;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the class's mutex.
//
///////////////////////////////////////////////////////////////////////////////

Thread::StaticMutex &Thread::_getStaticMutex()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == _staticMutex )
  {
    _staticMutex = Thread::StaticMutex::create();
  }

  return *_staticMutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the GUI thread ID. Not thread safe!
//
///////////////////////////////////////////////////////////////////////////////

void Thread::guiThread ( unsigned long id )
{
  USUL_TRACE_SCOPE;
  StaticGuard guard ( Thread::_getStaticMutex() );
  _guiThread = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the GUI thread?
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::isGuiThread()
{
  USUL_TRACE_SCOPE;
  StaticGuard guard ( Thread::_getStaticMutex() );
  const unsigned long id ( Usul::Threads::currentThreadId() );
  return ( id == _guiThread );
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
//  Set the state and result.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_set ( Thread::State state, Thread::Result result )
{
  USUL_TRACE_SCOPE;
  this->_setState ( state );
  this->_setResult ( result );
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
  this->_setResult ( Thread::CANCELLED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Schedules the thread and immediately returns.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::start()
{
  USUL_TRACE_SCOPE;

  // Initialize the error.
  this->_setError ( "" );

  // Set the state and result.
  this->_set ( Thread::SCHEDULED, Thread::NORMAL );

  // Start the thread.
  this->_start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the thread.
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
      this->_set ( Thread::NOT_RUNNING, Thread::ERROR_RESULT );
      this->_notifyError();
    }

    // Otherwise, no error yet...
    else
    {
      // Set the system thread ID.
      this->_setSystemThreadId ( Usul::Threads::currentThreadId() );

      // Set the state and result.
      this->_set ( Thread::RUNNING, Thread::NORMAL );

      // Notify start callback.
      this->_notifyStarted();

      // Notify finished callback.
      this->_notifyFinished();

      // Set the state and result.
      this->_set ( Thread::NOT_RUNNING, Thread::NORMAL );
    }
  }

  // Handle special exception.
  catch ( const Usul::Exceptions::Cancelled & )
  {
    this->_set ( Thread::NOT_RUNNING, Thread::CANCELLED );
    this->_notifyCancelled();
  }

  // Handle standard exceptions.
  catch ( const std::exception &e )
  {
    this->_reportErrorNoThrow ( Usul::Strings::format ( "Error 3474512825: standard exception caught while running thread ", this->id(), ", ", e.what() ) );
    this->_set ( Thread::NOT_RUNNING, Thread::ERROR_RESULT );
    this->_notifyError();
  }

  // Handle unknown exceptions.
  catch ( ... )
  {
    this->_reportErrorNoThrow ( Usul::Strings::format ( "Error 1668279636: unknown exception caught while running thread ", this->id() ) );
    this->_set ( Thread::NOT_RUNNING, Thread::ERROR_RESULT );
    this->_notifyError();
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
  Guard guard ( this->mutex() );

  if ( true == _cancelledCB.valid() )
  {
    (*_cancelledCB) ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyError()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( true == _errorCB.valid() )
  {
    (*_errorCB) ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyFinished()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( true == _finishedCB.valid() )
  {
    (*_finishedCB) ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify callback.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_notifyStarted()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( true == _startedCB.valid() )
  {
    (*_startedCB) ( this );
  }
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
      std::cout << s << std::endl;
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
