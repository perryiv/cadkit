
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Thread.h"
#include "Exceptions.h"
#include "ErrorChecker.h"

#include "Usul/Threads/Guard.h"
#include "Usul/Errors/Assert.h"

#include "vpr/Thread/ThreadFunctor.h"
#include "vpr/Thread/Thread.h"

using namespace VRV;
using namespace VRV::Threads;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Thread , Thread::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Thread::Thread() : BaseClass(),
  _thread  ( new vpr::Thread ),
  _done    ( false ),
  _running ( false ),
  _name    (),
  _error   ()
{
  ErrorChecker ( 3635737612u, 0x0 != _thread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Thread::~Thread()
{
  ErrorChecker ( 3774580748u, 0x0 != _thread );

  // If the thread is running...
  if ( this->running() )
  {
    // Wait for the thread. Otherwise, bad things happen.
    this->stop();
    _thread->join();
  }

  // Note: Calling "delete _thread" crashes on Windows. Should test on unix.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to get and set a value.
//
///////////////////////////////////////////////////////////////////////////////

namespace VRV
{
  namespace Detail
  {
    template < class ValueType, class MutexType > 
    inline ValueType get ( ValueType &value, MutexType &mutex )
    {
      Usul::Threads::Guard<MutexType> guard ( mutex );
      return value;
    }
    template < class ValueType, class MutexType > 
    inline void set ( const ValueType &from, ValueType &to, MutexType &mutex )
    {
      Usul::Threads::Guard<MutexType> guard ( mutex );
      to = from;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Thread::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case VRV::Interfaces::IThread::IID:
    return static_cast<VRV::Interfaces::IThread*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<VRV::Interfaces::IThread*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the thread.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::start()
{
  ErrorChecker ( 3888587022u, 0x0 != _thread );

  // Should not be running.
  if ( this->running() )
  {
    Usul::Exceptions::Thrower<VRV::Exceptions::FailedToStartJob>
      ( "Error 3300626853, job is currently running.",
        "\n\tJob name: ", _name.value() );
  }

  // Should not be done.
  if ( this->done() )
  {
    Usul::Exceptions::Thrower<VRV::Exceptions::FailedToStartJob>
      ( "Error 3099560136, starting a job that has finished.",
        "\n\tJob name: ", _name.value() );
  }

  // Make a thread-functor.
  vpr::BaseThreadFunctor *functor = new vpr::ThreadMemberFunctor<Thread> ( this, &Thread::_run, 0x0 );
  USUL_ASSERT ( functor );

  // Tell the thread to use this functor.
  _thread->setFunctor ( functor );

  // Start the thread.
  vpr::ReturnStatus result = _thread->start();
  if ( vpr::ReturnStatus::Succeed != result.code() )
  {
    Usul::Exceptions::Thrower<VRV::Exceptions::FailedToStartJob>
      ( "Error 1246069591, failed to start thread for job.",
        "\n\tJob name: ", _name.value(),
        "\n\tResult:   ", result.code() );
  }

  // We are running. Set this here because there is no way to know when the 
  // system will get around to calling _run().
  this->_setRunning ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The worker-thread enters here.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_run ( void * )
{
  try
  {
    // Make sure there is no error.
    this->_setError ( "" );

    // We are running.
    this->_setRunning ( true );

    // Inheriting classes implement this.
    this->_run();
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    this->_setError ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    // Note: we may get here if the thread is deleted while it is running,
    // in which case "this" is invalid. TODO, find a solution.
    this->_setError ( "Unknown exception" );
  }

  // We are no longer running and we are done.
  this->_setRunning ( false );
  this->_setDone ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the thread.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::stop()
{
  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the thread done?
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::done() const
{
  return VRV::Detail::get ( _done.value(), _done.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the thread running?
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::running() const
{
  return VRV::Detail::get ( _running.value(), _running.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Thread::name() const
{
  return VRV::Detail::get ( _name.value(), _name.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::name ( const std::string &n )
{
  VRV::Detail::set ( n, _name.value(), _name.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the error.
//
///////////////////////////////////////////////////////////////////////////////

std::string Thread::error() const
{
  return VRV::Detail::get ( _error.value(), _error.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the error.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_setError ( const std::string &e )
{
  VRV::Detail::set ( e, _error.value(), _error.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the done flag.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_setDone ( bool d )
{
  VRV::Detail::set ( d, _done.value(), _done.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the running flag.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_setRunning ( bool r )
{
  VRV::Detail::set ( r, _running.value(), _running.mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Report final results, if any.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::report()
{
  std::string message ( VRV::Detail::get ( _error.value(), _error.mutex() ) );
  if ( !message.empty() )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 3062147839, Job failed.",
        "\n\tJob name: ", _name.value(),
        "\n\tError:     ", message );
  }
}
