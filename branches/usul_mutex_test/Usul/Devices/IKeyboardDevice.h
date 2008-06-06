
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_IKEYBOARD_DEVICE_H__
#define __DEVICES_IKEYBOARD_DEVICE_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Devices {

struct IKeyListener;

struct IKeyboardDevice : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IKeyboardDevice );

    /// Id for this interface.
  enum { IID = 1962936565u };

  virtual void addKeyListener    ( IKeyListener* ) = 0;
  virtual void removeKeyListener ( IKeyListener* ) = 0;

};

} // namespace Usul
} // namespace Devices

#endif // __DEVICES_IKEYBOARD_DEVICE_H__
