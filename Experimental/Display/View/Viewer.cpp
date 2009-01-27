
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Display/View/Viewer.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IEventListener.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace Display::View;

USUL_IMPLEMENT_TYPE_ID ( Viewer );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( IUnknown::RefPtr doc ) : BaseClass ( doc ),
  _eventListeners()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Viewer::_destroy ), "2176674373" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
/////////////////////////////////////////////////////////////////////////////

void Viewer::_destroy()
{
  USUL_TRACE_SCOPE;
  _eventListeners.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stateSave() const
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stateLoad()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of an event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::notify ( Display::Events::Event &e )
{
  USUL_TRACE_SCOPE;
  USUL_TRY_BLOCK
  {
    this->_notify ( e );
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2706486451" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification of an event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_notify ( Display::Events::Event &e )
{
  USUL_TRACE_SCOPE;

  // Get the listeners.
  EventListeners listenerMap ( Usul::Threads::Safe::get ( this->mutex(), _eventListeners ) );
  EventListeners::const_iterator j ( listenerMap.find ( e.type() ) );
  ListenerSequence listeners ( ( listenerMap.end() == j ) ? ListenerSequence() : j->second );

  // Needed in the loop. Don't cause a deletion.
  Usul::Interfaces::IUnknown::QueryPtr me ( ( this->refCount() > 0 ) ? this : 0x0 );

  // Call the listeners.
  for ( ListenerSequence::iterator i = listeners.begin(); i != listeners.end(); ++i )
  {
    Usul::Interfaces::IEventListener::QueryPtr listener ( *i );
    if ( true == listener.valid() )
    {
      listener->notify ( e, me );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::listenerAppend ( EventType type, IUnknown::RefPtr unknown )
{
  USUL_TRACE_SCOPE;
  if ( true == unknown.valid() )
  {
    WriteLock lock ( this );
    EventListeners::mapped_type &listeners ( _eventListeners[type] );
    listeners.insert ( listeners.end(), unknown );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::listenerPrepend ( EventType type, IUnknown::RefPtr unknown )
{
  USUL_TRACE_SCOPE;
  if ( true == unknown.valid() )
  {
    WriteLock lock ( this );
    EventListeners::mapped_type &listeners ( _eventListeners[type] );
    listeners.insert ( listeners.begin(), unknown );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::listenerRemove ( EventType type, IUnknown::RefPtr unknown )
{
  USUL_TRACE_SCOPE;
  if ( true == unknown.valid() )
  {
    WriteLock lock ( this );
    EventListeners::mapped_type &listeners ( _eventListeners[type] );
    listeners.erase ( std::find ( listeners.begin(), listeners.end(), unknown ) );
    if ( true == listeners.empty() )
    {
      _eventListeners.erase ( type );
    }
  }
}
