
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MOTION_LISTENER_H__
#define __MOTION_LISTENER_H__

#include "Usul/Devices/IMotionListener.h"

namespace OsgFox {
namespace Views {

class Canvas;

class MotionListener : public Usul::Devices::IMotionListener
{
public:
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MotionListener );

  MotionListener ( Canvas &canvas );

  virtual void onMotion( const Usul::Devices::State& state );

protected:
  MotionListener();
  virtual ~MotionListener();

private:
  Canvas &_canvas;
};

}
}

#endif //__MOTION_LISTENER_H__

