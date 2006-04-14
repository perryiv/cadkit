
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

#include "DXJoystickComponent.h"
#include "DirectXJoystick.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DXJoystickComponent , DXJoystickComponent::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( DXJoystickComponent );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DXJoystickComponent::DXJoystickComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DXJoystickComponent::~DXJoystickComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *DXJoystickComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Devices::IDeviceCreate::IID:
    return static_cast < Usul::Devices::IDeviceCreate*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create a Fox Mouse
//
/////////////////////////////////////////////////////////////////////////////

Usul::Devices::IDevice*  DXJoystickComponent::createDevice ( Usul::Interfaces::IUnknown *caller )
{
  DirectXJoystick::ValidRefPtr joystick ( new DirectXJoystick ( caller ) );
  return joystick.release();
}
