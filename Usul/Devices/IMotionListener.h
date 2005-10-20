
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_IMOTION_LISTENER_H__
#define __DEVICES_IMOTION_LISTENER_H__


#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace Usul {
namespace Devices {

class State;

struct IMotionListener : public Usul::Base::Referenced
{
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IMotionListener );

  virtual void onMotion( const State& state ) = 0;
};

} // namespace Usul
} // namespace Devices

#endif // __DEVICES_IMOTION_LISTENER_H__

