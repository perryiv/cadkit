
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
#include "Usul/Base/Referenced.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITimerNotify.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "QtCore/QCoreApplication"

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TimerCallback::TimerCallback ( TimerID id, unsigned int milliseconds, TimerCallback::UnknownPtr callback, bool singleShot ) : BaseClass(),
  _mutex(),
  _id ( id ),
  _milliseconds ( milliseconds ),
  _timer(),
  _callback ( callback )
{
  USUL_TRACE_SCOPE;

  // Make the timer.
  _timer.reset ( new QTimer );
  
  // Move to the main thread.
  this->moveToThread ( QCoreApplication::instance()->thread() );
  _timer->moveToThread ( QCoreApplication::instance()->thread() );
  
  // Set the parent. Do this after moving to the main thread.
  _timer->setParent ( this );

  // Set the connection.
  this->connect ( _timer.get(), SIGNAL ( timeout() ), this, SLOT ( _onTimeout() ) );

  // Is this a single-shot timer?
  _timer->setSingleShot ( singleShot );
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
  
  if ( false == Usul::Threads::Named::instance().is ( Usul::Threads::Names::GUI ) )
  {
    if ( false == QMetaObject::invokeMethod ( this, "_start", Qt::QueuedConnection ) )
    {
      std::cout << "Warning 7474543440: could not start the timer." << std::endl;
    }
  }
  else
  {
    this->_start();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::stop()
{
  USUL_TRACE_SCOPE;
  
  if ( false == Usul::Threads::Named::instance().is ( Usul::Threads::Names::GUI ) )
    QMetaObject::invokeMethod ( this, "_stop", Qt::QueuedConnection );
  else
    this->_stop();
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
    Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( notify, &Usul::Interfaces::ITimerNotify::timerNotify ), _id, "4014661170" );
  }

  {
    Guard guard ( _mutex );
    {
      if ( ( 0x0 != _timer ) && ( true == _timer->isSingleShot() ) )
      {
        _timer->start();
      }
    }
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
  Guard guard ( _mutex );
  
  // This probably isn't the best way to do this, but it will allow callbacks to be deleted when the only reference is the one in this object.
  const Usul::Base::Referenced* referenced ( dynamic_cast<const Usul::Base::Referenced*> ( _callback.get() ) );
  return ( 0x0 != referenced ? 1 == referenced->refCount() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::_start()
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
  _timer->setInterval ( _milliseconds );
  _timer->start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the timer.
//
///////////////////////////////////////////////////////////////////////////////

void TimerCallback::_stop()
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
