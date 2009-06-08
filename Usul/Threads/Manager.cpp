
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread manager classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/System/Clock.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>
#include <algorithm>
#include <limits>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

Manager *Manager::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
  _mutex          (),
  _factory        ( 0x0 ),
  _guiThread      ( 0 ),
  _nextThreadId   ( 0 ),
  _threads        ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Manager::_destroy ), "3593335840" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton construction.
//
///////////////////////////////////////////////////////////////////////////////

Manager &Manager::instance()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 == _instance )
  {
    _instance = new Manager;
  }
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton destruction.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::destroy()
{
  USUL_TRACE_SCOPE_STATIC;
  delete _instance;
  _instance = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the members.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_destroy()
{
  _threads.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the thread. Client needs to set the function.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Thread::RefPtr Manager::create ( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( 0x0 == _factory )
  {
    USUL_ASSERT ( _factory ); // The client needs to set this.
    throw std::runtime_error ( "Error 2149832182: No thread factory set" );
  }

  Usul::Threads::Thread::RefPtr thread ( (*_factory) ( name ) );
  if ( false == thread.valid() )
  {
    throw std::runtime_error ( "Error 3505918135: Thread creation failed" );
  }

  _threads.push_back ( thread );
  return thread;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the factory-function. Return the previous one.
//
///////////////////////////////////////////////////////////////////////////////

Manager::FactoryFunction *Manager::factory ( Manager::FactoryFunction *factory )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  FactoryFunction *original = _factory;
  _factory = factory;
  return original;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the factory-function.
//
///////////////////////////////////////////////////////////////////////////////

Manager::FactoryFunction *Manager::factory()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _factory;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the GUI thread ID.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::guiThread ( unsigned long id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _guiThread = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the GUI thread ID.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Manager::guiThread() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _guiThread;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the GUI thread?
//
///////////////////////////////////////////////////////////////////////////////

bool Manager::isGuiThread() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  const unsigned long id ( Usul::Threads::currentThreadId() );
  return ( id == _guiThread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Mutex &Manager::mutex() const
{
  USUL_TRACE_SCOPE;
  return _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the next thread id. This will also increment the internal counter.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Manager::nextThreadId()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  unsigned long id ( _nextThreadId++ );
  return id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge all threads that are ready to be deleted.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::purge()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Remove all threads that are ready.
  _threads.remove_if ( std::mem_fun ( &Thread::isIdle ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the list of threads is empty.
//
///////////////////////////////////////////////////////////////////////////////

bool Manager::empty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _threads.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all tasks to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::wait ( unsigned long timeout )
{
  USUL_TRACE_SCOPE;

  // Save current time.
  const unsigned long start ( static_cast<unsigned long> ( Usul::System::Clock::milliseconds() ) );

  while ( false == this->empty() )
  {
    this->purge();

    // Check the time.
    const unsigned long now ( static_cast<unsigned long> ( Usul::System::Clock::milliseconds() ) );
    if ( now - start > timeout )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3602078852: exceeded allowable time period of ", 
          static_cast<double> ( timeout ) / 1000.0, 
          " seconds while waiting for threads" );
    }

    // Print some feedback about the threads we're waiting on.
    ThreadList threads ( Usul::Threads::Safe::get ( this->mutex(), _threads ) );
    if ( false == threads.empty() )
    {
      std::cout << "Waiting on these threads:" << std::endl;
      for ( ThreadList::const_iterator i = threads.begin(); i != threads.end(); ++i )
      {
        Usul::Threads::Thread::RefPtr thread ( *i );
        if ( true == thread.valid() )
        {
          std::cout << Usul::Strings::format ( "Name: ", thread->name(), ", ID: ", thread->id(), ", Address: ", thread.get() ) << std::endl;
        }
      }
    }

    // Sleep so that we don't overload the system.
    Usul::System::Sleep::milliseconds ( 500 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all tasks to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::wait()
{
  USUL_TRACE_SCOPE;
  this->wait ( std::numeric_limits<unsigned long>::max() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel all threads.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::cancel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  std::for_each ( _threads.begin(), _threads.end(), std::mem_fun ( &Thread::cancel ) );
}
