
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for an event listener.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_EVENT_LISTENER_H_
#define _USUL_INTERFACES_EVENT_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"

// Move the event classes to Usul when they mature.
namespace Display { namespace Events { class Event; } }


namespace Usul {
namespace Interfaces {


struct IEventListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IEventListener );

  // Id for this interface.
  enum { IID = 3909978530u };

  // Called when an event happens.
  virtual void            notify ( const Display::Events::Event &, Usul::Interfaces::IUnknown::RefPtr caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_EVENT_LISTENER_H_
