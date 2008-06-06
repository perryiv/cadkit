
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_IBUTTON_LISTENERS_H__
#define __DEVICES_IBUTTON_LISTENERS_H__


#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Pointers/Pointers.h"

namespace Usul {
namespace Devices {

class State;

struct IButtonListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonListener );

  virtual void buttonPressed  ( unsigned int button, const State& state ) = 0;
  virtual void buttonReleased ( unsigned int button, const State& state ) = 0;
};

} // namespace Devices
} // namespace Usul


#endif // __DEVICES_IBUTTON_LISTENERS_H__

