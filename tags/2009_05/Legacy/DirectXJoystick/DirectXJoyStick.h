
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_DIRECTX_JOYSTICK_H__
#define __DEVICES_DIRECTX_JOYSTICK_H__

#include "Usul/Devices/BaseDevice.h"
#include "Usul/Devices/State.h"

#include "Usul/Interfaces/ITrackball.h"

#include "FoxTools/Headers/Object.h"

namespace Detail { class Joystick; }

class DirectXJoystick : public FX::FXObject,
                        public Usul::Devices::BaseDevice
{
public:

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DirectXJoystick );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  DirectXJoystick ( Usul::Interfaces::IUnknown *caller );
  virtual ~DirectXJoystick();

  virtual void clear();

  long                  onTimeout            ( FX::FXObject *object, FX::FXSelector id, void *data );

private:
  void _getTrackballInfo (const osg::Quat& rot, osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y);
  float _tb_project_to_sphere(float r, float x, float y);

  friend Detail::Joystick;

  DirectXJoystick() { }

  typedef Usul::Interfaces::ITrackball::ValidAccessQueryPtr TrackballPtr;

  TrackballPtr         _trackball;
  Usul::Devices::State _lastState;

  FXDECLARE( DirectXJoystick )
};



#endif // __DEVICES_DIRECTX_JOYSTICK_H__
