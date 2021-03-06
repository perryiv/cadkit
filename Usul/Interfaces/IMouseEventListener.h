
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a mouse event.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_MOUSE_EVENT_LISTENER_H_
#define _USUL_INTERFACES_MOUSE_EVENT_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osgGA { class GUIEventAdapter; }

namespace Usul {
namespace Interfaces {


struct IMouseEventListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMouseEventListener );

  // Id for this interface.
  enum { IID = 1673565693u };

  // Called when mouse event occurs.
  virtual void            mouseEventNotify ( osgGA::GUIEventAdapter&, Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_MOUSE_EVENT_LISTENER_H_
