
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_IBUTTON_LISTENERS_H__
#define __DEVICES_IBUTTON_LISTENERS_H__


#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace Usul {
namespace Devices {

class State;

struct IButtonListener : public Usul::Base::Referenced
{
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IButtonListener );

  virtual void buttonPressed  ( const State& state ) = 0;
  virtual void buttonReleased ( const State& state ) = 0;
};

} // namespace Devices
} // namespace Usul


#endif // __DEVICES_IBUTTON_LISTENERS_H__

