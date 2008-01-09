
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for render notification.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_RENDER_NOTIFY_H_
#define _USUL_INTERFACE_RENDER_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IRenderNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRenderNotify );

  /// Id for this interface.
  enum { IID = 1870602259u };

  // Add the listener (IRenderListener).
  virtual void                addRenderListener ( Usul::Interfaces::IUnknown *listener ) = 0;

  // Remove all render listeners.
  virtual void                clearRenderListeners() = 0;

  // Remove the listener (IRenderListener).
  virtual void                removeRenderListener ( Usul::Interfaces::IUnknown *caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_RENDER_NOTIFY_H_
