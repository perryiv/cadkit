
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FOX_CAN_CLOSE_H_
#define _USUL_INTERFACE_FOX_CAN_CLOSE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

  struct IWindow;

struct ICanClose : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICanClose );

  /// Id for this interface.
  enum { IID = 1072485030u };

  virtual bool canClose ( Usul::Interfaces::IWindow *window, Usul::Interfaces::IUnknown *caller = 0x0, bool checkNumWindows = true ) = 0;

}; // class ICanClose


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_FOX_CAN_CLOSE_H_
