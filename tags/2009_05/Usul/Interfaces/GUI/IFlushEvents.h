
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for flushing the event queue.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FLUSH_EVENT_QUEUE_H_
#define _USUL_INTERFACE_FLUSH_EVENT_QUEUE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IFlushEvents : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFlushEvents );

  /// Id for this interface.
  enum { IID = 1994255490u };

  /// Process the events.
  virtual void flushEventQueue() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_FLUSH_EVENT_QUEUE_H_
