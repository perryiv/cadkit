
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DIRECTX_JOYSTICK_COMPONENT_CLASS_H_
#define _DIRECTX_JOYSTICK_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Devices/IDeviceCreate.h"

#include <string>



class DXJoystickComponent : public Usul::Base::Referenced,
                            public Usul::Interfaces::IPlugin,
                            public Usul::Devices::IDeviceCreate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( DXJoystickComponent );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DXJoystickComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  DXJoystickComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "DirectX Joystick"; }

  /// Usul::Interfaces::IDeviceCreate
  virtual Usul::Devices::IDevice*          createDevice ( Usul::Interfaces::IUnknown *caller = 0x0 );

protected: 

  // Do not copy.
  DXJoystickComponent ( const DXJoystickComponent & );
  DXJoystickComponent &operator = ( const DXJoystickComponent & );

  /// Use reference counting.
  virtual ~DXJoystickComponent();
};


#endif // _DIRECTX_JOYSTICK_COMPONENT_CLASS_H_
