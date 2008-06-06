
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for intersect notification.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_INTERSECT_NOTIFY_H_
#define _USUL_INTERFACE_INTERSECT_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IIntersectNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IIntersectNotify );

  /// Id for this interface.
  enum { IID = 1884162340u };

  // Add the listener (IIntersectListener).
  virtual void                addIntersectListener ( Usul::Interfaces::IUnknown *listener ) = 0;

  // Remove all intersect listeners.
  virtual void                clearIntersectListeners() = 0;

  // Remove the listener (IIntersectListener).
  virtual void                removeIntersectListener ( Usul::Interfaces::IUnknown *caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_INTERSECT_NOTIFY_H_
