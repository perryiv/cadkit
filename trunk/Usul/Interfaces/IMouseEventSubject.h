
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

#ifndef _USUL_INTERFACES_MOUSE_EVENT_SUBJECT_H_
#define _USUL_INTERFACES_MOUSE_EVENT_SUBJECT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IMouseEventSubject : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMouseEventSubject );

  // Id for this interface.
  enum { IID = 2313973612u };

  // Add the listener.
  virtual void            addMouseEventListener ( Usul::Interfaces::IUnknown * ) = 0;

  // Remove all listeners.
  virtual void            clearMouseEventListeners() = 0;

  // Remove the listener.
  virtual void            removeMouseEventListener ( Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_MOUSE_EVENT_SUBJECT_H_
