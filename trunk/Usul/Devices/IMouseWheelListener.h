
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_IMOUSE_WHEEL_LISTENER_H__
#define __DEVICES_IMOUSE_WHEEL_LISTENER_H__


#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Pointers/Pointers.h"

namespace Usul {
namespace Devices {

class State;

struct IMouseWheelListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IMouseWheelListener );

  virtual void onMouseWheel ( double delta ) = 0;
};

} // namespace Usul
} // namespace Devices

#endif // __DEVICES_IMOTION_LISTENER_H__

