
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IADD_DOCK_WINDOW_H__
#define __USUL_INTERFACES_IADD_DOCK_WINDOW_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IAddDockWindow : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAddDockWindow );

  /// Id for this interface.
  enum { IID = 3439614978u };

  virtual void                     addDockWindow ( Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;

}; // struct IAddDockWindow


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_IADD_DOCK_WINDOW_H__
