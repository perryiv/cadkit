
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "BaseDevice.h"

using namespace Usul::Devices;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( BaseDevice , Usul::Base::Referenced );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

BaseDevice::BaseDevice() :
_navigating( true ),
_buttonListeners(),
_motionListeners(),
_mouseWheelListeners()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

BaseDevice::~BaseDevice()
{
  _buttonListeners.clear();
  _motionListeners.clear();
  _mouseWheelListeners.clear();
}


Usul::Interfaces::IUnknown* BaseDevice::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
    return static_cast < Usul::Interfaces::IUnknown* > ( this );
  default:
    return 0x0;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a button listener.
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::addButtonListener( unsigned int button, IButtonListener* listener )
{
  _buttonListeners[button].push_back( listener );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify button listeners of a button pressed.
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::_notifyButtonPressed ( unsigned int button, const State& state )
{
  ButtonListeners& listeners = _buttonListeners[button];

  for( ButtonListeners::iterator iter = listeners.begin(); iter != listeners.end(); ++iter )
  {
    (*iter)->buttonPressed ( button, state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify button listeners of a button released.
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::_notifyButtonReleased ( unsigned int button, const State& state )
{
  ButtonListeners& listeners = _buttonListeners[button];

  for( ButtonListeners::iterator iter = listeners.begin(); iter != listeners.end(); ++iter )
  {
    (*iter)->buttonReleased( button, state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a motion listener.
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::addMotionListener( IMotionListener* listener )
{
  _motionListeners.push_back ( listener );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify motion listeners of a motion event.
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::_notifyMotionListeners ( const State& state )
{
  for( MotionListeners::iterator iter = _motionListeners.begin(); iter != _motionListeners.end(); ++iter )
  {
    (*iter)->onMotion( state );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::clear()
{
  _buttonListeners.clear();
  _motionListeners.clear();
  _mouseWheelListeners.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add mouse wheel listeners.
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::addMouseWheelListener ( IMouseWheelListener *listener )
{
  _mouseWheelListeners.push_back ( listener );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Add mouse wheel listeners.
//
///////////////////////////////////////////////////////////////////////////////

void BaseDevice::_notifyMouseWheelListeners ( double delta )
{
  for( MouseWheelListeners::iterator iter = _mouseWheelListeners.begin(); iter != _mouseWheelListeners.end(); ++iter )
  {
    (*iter)->onMouseWheel ( delta );
  }
}
