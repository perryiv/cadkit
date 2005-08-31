
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for receiving render notifications.
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
  enum { IID = 3487150999u };

  /// Notification of a render.
  virtual void                    renderNotify ( IUnknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_RENDER_NOTIFY_H_
