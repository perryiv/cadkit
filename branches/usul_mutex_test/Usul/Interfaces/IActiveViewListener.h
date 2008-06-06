
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IACTIVE_VIEW_LISTENER_H__
#define __USUL_INTERFACES_IACTIVE_VIEW_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IActiveViewListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IActiveViewListener );

  /// Id for this interface.
  enum { IID = 2585242778u };

  /// The active document has changed.
  virtual void                          activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView ) = 0;

}; // struct IActiveViewListener


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IACTIVE_VIEW_LISTENER_H__
