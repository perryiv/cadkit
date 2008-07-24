
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal timer callback.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/TimerCallback.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITimerNotify.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TimerCallback::TimerCallback ( TimerID id, unsigned int milliseconds, TimerCallback::UnknownPtr callback ) : BaseClass(),
  _mutex(),
  _id ( id ),
  _milliseconds ( milliseconds ),
  _timer(),
  _callback ( callback )
{
  USUL_TRACE_SCOPE;

  // Make the timer.
  _timer.reset ( new QTimer ( this ) );

  // Set the connection.
  this->connect ( _timer.get(), SIGNAL ( timeout() ), this, SLOT ( _onTimeout() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor. Qt deletes child widgets.
//
///////////////////////////////////////////////////////////////////////////////

TimerCallback::~TimerCallback()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &TimerCallback::_destroy ), "4218934207" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor contents.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::_destroy()
{
  USUL_TRACE_SCOPE;

  // Stop the timer if it's running.
  this->stop();

  // Disconnect the slot.
  this->disconnect();

  // This should delete the timer.
  _timer.reset();

  // Clear the callback.
  _callback = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::start()
{
  USUL_TRACE_SCOPE;
  Guard guard ( _mutex );

  // Make sure the timer is valid.
  if ( 0x0 == _timer.get() )
    return;

  // Is the timer already running?
  if ( true == _timer->isActive() )
    return;

  // Start the timer.
  _timer->start ( _milliseconds );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::stop()
{
  USUL_TRACE_SCOPE;
  Guard guard ( _mutex );
  
  // Make sure the timer is valid.
  if ( 0x0 == _timer.get() )
    return;
  
  // Is the timer running?
  if ( false == _timer->isActive() )
    return;
  
  // Stop the timer.
  _timer->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Timer slot.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::_onTimeout()
{
  USUL_TRACE_SCOPE;

  UnknownPtr callback ( Usul::Threads::Safe::get ( _mutex, _callback ) );
  Usul::Interfaces::ITimerNotify::QueryPtr notify ( callback );
  if ( true == notify.valid() )
  {
    notify->timerNotify ( _id );
  }
}
