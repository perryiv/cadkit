
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal timer callback.
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Core/TimerCallback.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITimerNotify.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"
#include "Usul/System/Clock.h"


using namespace VRV::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TimerCallback::TimerCallback ( TimerID id, unsigned int milliseconds, TimerCallback::UnknownPtr callback ) : BaseClass(),
  _id ( id ),
  _lastTimeCalled ( 0 ),
  _timeoutInMilliseconds ( milliseconds ),
  _callback ( callback ),
  _active ( false )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TimerCallback::~TimerCallback()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Timer slot.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::_onTimeout()
{
  USUL_TRACE_SCOPE;

  // Return now if we aren't active.
  bool active ( Usul::Threads::Safe::get ( this->mutex(), _active ) );
  if ( false == active )
    return;
  
  UnknownPtr callback ( Usul::Threads::Safe::get ( this->mutex(), _callback ) );
  Usul::Interfaces::ITimerNotify::QueryPtr notify ( callback );

  if ( true == notify.valid() )
  {
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( notify, &Usul::Interfaces::ITimerNotify::timerNotify ), _id, "5655535300" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can this timer callback be purged?
//
///////////////////////////////////////////////////////////////////////////////

bool TimerCallback::canPurge() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // This probably isn't the best way to do this, but it will allow callbacks to be deleted when the only reference is the one in this object.
  const Usul::Base::Referenced* referenced ( dynamic_cast<const Usul::Base::Referenced*> ( _callback.get() ) );
  return ( 0x0 != referenced ? 1 == referenced->refCount() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::start()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  // Is the timer already running?
  if ( true == _active )
    return;

  // Start the timer.
  _active = true;
  _lastTimeCalled = Usul::System::Clock::milliseconds();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::stop()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Is the timer running?
  if ( false == _active )
    return;

  // Stop the timer.
  _active = false;
  _lastTimeCalled = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.  Will fire callback if time elapsed is greater than the timeout.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::update()
{
  USUL_TRACE_SCOPE;

  // Make a RefPtr to ourselfs so we don't get deleted in the _onTimeout function.
  TimerCallback::RefPtr me ( this );

  // Guard rest of the function.
  Guard guard ( this->mutex() );

  // Return now if we aren't active.
  if ( false == _active )
    return;

  // Get the current time.
  Usul::Types::Uint64 currentTime ( Usul::System::Clock::milliseconds() );
  
  // Get the elapsed time.
  Usul::Types::Uint64 elapsedTime ( currentTime - _lastTimeCalled );

  // Call the timeout function if the elapsed time is greater than our timeout.
  if ( elapsedTime > _timeoutInMilliseconds )
  {
    this->_onTimeout();
  }

  _lastTimeCalled = currentTime;
}
