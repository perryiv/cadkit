
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for render listeners.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_RENDER_LISTENER_H_
#define _USUL_INTERFACE_RENDER_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IRenderListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRenderListener );

  /// Id for this interface.
  enum { IID = 1353798942u };

  // Called after a render.
  virtual void                postRenderNotify ( Usul::Interfaces::IUnknown *caller ) = 0;

  // Called before a render.
  virtual void                preRenderNotify ( Usul::Interfaces::IUnknown *caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_RENDER_LISTENER_H_
