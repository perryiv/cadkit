
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Timer server.
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Core/TimerServer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace VRV::Core;

USUL_IMPLEMENT_TYPE_ID ( TimerServer );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TimerServer, TimerServer::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TimerServer::TimerServer() : BaseClass(),
  _timers(),
  _nextId ( 1 ), // Start at 1 so that clients can initialize timer ids with 0.
  _removed()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor. Qt deletes child widgets.
//
///////////////////////////////////////////////////////////////////////////////

TimerServer::~TimerServer()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &TimerServer::_destroy ), "1498464868" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::_destroy()
{
  USUL_TRACE_SCOPE;
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TimerServer::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  // No need to guard, should be re-entrant.
  
  switch ( iid )
  {
    case Usul::Interfaces::IUnknown::IID:
    case Usul::Interfaces::ITimerService::IID:
      return static_cast < Usul::Interfaces::ITimerService * > ( this );
    default:
      return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the timers.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::clear()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // First stop the timers.
  this->stop();

  // Now clear the collections.
  _timers.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the timers.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::stop()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  for ( Timers::iterator i = _timers.begin(); i != _timers.end(); ++i )
  {
    TimerCallback::RefPtr timer ( i->second );
    if ( 0x0 != timer.get() )
    {
      timer->stop();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the timers.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::start()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  for ( Timers::iterator i = _timers.begin(); i != _timers.end(); ++i )
  {
    TimerCallback::RefPtr timer ( i->second );
    if ( 0x0 != timer.get() )
    {
      timer->start();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the timer.
//
///////////////////////////////////////////////////////////////////////////////

TimerServer::TimerID TimerServer::timerAdd ( unsigned int milliseconds, Usul::Interfaces::IUnknown::RefPtr callback, bool singleShot )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get the next id.
  const TimerID id ( _nextId++ );

  // Add the timer.
  this->_timerAdd ( id, milliseconds, callback, singleShot );

  // Return the new timer id.
  return id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::_timerAdd ( TimerID id, unsigned int milliseconds, Usul::Interfaces::IUnknown::RefPtr callback, bool singleShot )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Ignore single shot.  Since VR Juggler fires away, I'm not sure single shot makes sense here.
  TimerCallback::RefPtr cb ( new TimerCallback ( id, milliseconds, callback ) );
  _timers.insert ( Timers::value_type ( id, cb ) );
  cb->start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::timerRemove ( TimerID id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Get the timer.
  TimerCallback::RefPtr timer ( _timers[id] );
  
  // Stop the timer and add it to the list to be deleted.
  if ( 0x0 != timer.get() )
  {
    timer->stop();
    _removed.push_back ( timer );
  }
  
  // Remove the timer from the map.
  _timers.erase ( id );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge all null timers, or timers with one reference count.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::purge()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Clear the list of timers that have been removed.  This will delete them.
  _removed.clear();
  
  typedef std::list<Timers::key_type> Keys;
  Keys doomed;
  
  // Find timers that need to be purged.
  for ( Timers::const_iterator i = _timers.begin(); i != _timers.end(); ++i )
  {
    TimerCallback::RefPtr timer ( i->second );
    if ( 0x0 == timer.get() )
    {
      doomed.push_back ( i->first );
    }
    else if ( true == timer->canPurge() )
    {
      doomed.push_back ( i->first );
    }
  }
  
  // Remove the id's from the map.
  for ( Keys::const_iterator i = doomed.begin(); i != doomed.end(); ++i )
    _timers.erase ( *i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update all timers.
//
///////////////////////////////////////////////////////////////////////////////

void TimerServer::update()
{
  USUL_TRACE_SCOPE;

  // Make a copy of the timers.  This is important because a timer may get removed by the timeout callback.
  Timers timers ( Usul::Threads::Safe::get (  this->mutex(), _timers ) );

  for ( Timers::iterator i = timers.begin(); i != timers.end(); ++i )
  {
    TimerCallback::RefPtr timer ( i->second );
    if ( 0x0 != timer.get() )
    {
      timer->update();
    }
  }
}
