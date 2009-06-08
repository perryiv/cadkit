
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper class for two instances of gadget::AnalogInterface.
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Devices/JoystickDevice.h"

#include "Usul/Strings/Format.h"

using namespace VRV::Devices;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( JoystickDevice, JoystickDevice::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

JoystickDevice::JoystickDevice ( const std::string &h, const std::string &v ) : 
  BaseClass(),
  _cbm(),
  _tm(),
  _hai(),
  _vai(),
  _h ( 0 ),
  _v ( 0 ),
  _hs ( JOYSTICK_AT_CENTER ),
  _vs ( JOYSTICK_AT_CENTER ),
  _analogTrim( 0.0, 0.0 ),
  _name( "" ),
  _horizontalModifier( 1.0 ),
  _verticalModifier( 1.0 )
{
  // Initialize.
  _hai.init ( h );
  _vai.init ( v );

  // Fill up the transition map.
  _tm[MessagePair(JOYSTICK_AT_CENTER,JOYSTICK_AT_RIGHT) ] = MessagePair(JOYSTICK_LEAVING_CENTER,JOYSTICK_ENTERING_RIGHT );
  _tm[MessagePair(JOYSTICK_AT_CENTER,JOYSTICK_AT_LEFT)  ] = MessagePair(JOYSTICK_LEAVING_CENTER,JOYSTICK_ENTERING_LEFT  );
  _tm[MessagePair(JOYSTICK_AT_CENTER,JOYSTICK_AT_UP)    ] = MessagePair(JOYSTICK_LEAVING_CENTER,JOYSTICK_ENTERING_UP    );
  _tm[MessagePair(JOYSTICK_AT_CENTER,JOYSTICK_AT_DOWN)  ] = MessagePair(JOYSTICK_LEAVING_CENTER,JOYSTICK_ENTERING_DOWN  );

  // The other way...
  _tm[MessagePair(JOYSTICK_AT_RIGHT, JOYSTICK_AT_CENTER)] = MessagePair(JOYSTICK_LEAVING_RIGHT, JOYSTICK_ENTERING_CENTER);
  _tm[MessagePair(JOYSTICK_AT_LEFT,  JOYSTICK_AT_CENTER)] = MessagePair(JOYSTICK_LEAVING_LEFT,  JOYSTICK_ENTERING_CENTER);
  _tm[MessagePair(JOYSTICK_AT_UP,    JOYSTICK_AT_CENTER)] = MessagePair(JOYSTICK_LEAVING_UP,    JOYSTICK_ENTERING_CENTER);
  _tm[MessagePair(JOYSTICK_AT_DOWN,  JOYSTICK_AT_CENTER)] = MessagePair(JOYSTICK_LEAVING_DOWN,  JOYSTICK_ENTERING_CENTER);

  // Skipping center (if update-rate is slow)...
  _tm[MessagePair(JOYSTICK_AT_LEFT,  JOYSTICK_AT_RIGHT) ] = MessagePair(JOYSTICK_LEAVING_LEFT,  JOYSTICK_ENTERING_RIGHT );
  _tm[MessagePair(JOYSTICK_AT_RIGHT, JOYSTICK_AT_LEFT)  ] = MessagePair(JOYSTICK_LEAVING_RIGHT, JOYSTICK_ENTERING_LEFT  );
  _tm[MessagePair(JOYSTICK_AT_UP,    JOYSTICK_AT_DOWN)  ] = MessagePair(JOYSTICK_LEAVING_UP,    JOYSTICK_ENTERING_DOWN  );
  _tm[MessagePair(JOYSTICK_AT_DOWN,  JOYSTICK_AT_UP)    ] = MessagePair(JOYSTICK_LEAVING_DOWN,  JOYSTICK_ENTERING_UP    );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

JoystickDevice::~JoystickDevice()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* JoystickDevice::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IJoystickFloat::IID:
    return static_cast< Usul::Interfaces::IJoystickFloat * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the internal state.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::update()
{
  _h = _hai->getData();
  _v = _vai->getData();
  /*std::string output = Usul::Strings::format( "Joytick: ", _name, "\t H = ", 
                                              this->joystickHorizontal(), "  V = ", 
                                              this->joystickVertical(), " | " );
  ::OutputDebugStringA( output.c_str() );
  std::cout << output << std::flush;*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the client of any state changes.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::notify()
{
  // Grab current state.
  Message hs ( _hs ), vs ( _vs );

  // Update the state.
  this->_updateState();

  // Notify for both directions.
  this->_notify ( hs, _hs );
  this->_notify ( vs, _vs );
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the client of any state changes.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::_notify ( Message last, Message current )
{
  // Look for a transition pair.
  Transitions::iterator i = _tm.find ( MessagePair ( last, current ) );

  // If we found one...
  if ( _tm.end() != i )
  {
    // Get the pair.
    MessagePair &mp = i->second;

    // Send the transitions.
    this->_sendMessage ( mp.first );
    this->_sendMessage ( mp.second );
  }

  // Always send the current state.
  this->_sendMessage ( current );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the message to the client.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::_sendMessage ( Message m )
{
  Callback *cb = this->callback ( m );
  if ( cb )
  {
    (*cb) ( m, this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::callback ( Message m, Callback::Ptr c )
{
  _cbm[m] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the callback.
//
///////////////////////////////////////////////////////////////////////////////

Callback *JoystickDevice::callback ( Message m )
{
  Callbacks::iterator i = _cbm.find ( m );
  return ( _cbm.end() == i ) ? 0x0 : i->second.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the current state.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::_updateState()
{
  // The gadget library always gives us [0,1].
  const float high ( 0.75f );
  const float low  ( 0.25f );

  // Horizontal.
  if( _horizontalModifier > 0.0 )
  {
    if ( _h > high )
      _hs = JOYSTICK_AT_RIGHT;
    else if ( _h < low )
      _hs = JOYSTICK_AT_LEFT;
    else
      _hs = JOYSTICK_AT_CENTER;
  }
  else
  {
    if ( _h > high )
      _hs = JOYSTICK_AT_LEFT;
    else if ( _h < low )
      _hs = JOYSTICK_AT_RIGHT;
    else
      _hs = JOYSTICK_AT_CENTER;
  }

  // Vertical.
  if( _verticalModifier > 0.0 )
  {
    if ( _v > high )
      _vs = JOYSTICK_AT_UP;
    else if ( _v < low )
      _vs = JOYSTICK_AT_DOWN;
    else
      _vs = JOYSTICK_AT_CENTER;
  }
  else
  {
    if ( _v > high )
      _vs = JOYSTICK_AT_DOWN;
    else if ( _v < low )
      _vs = JOYSTICK_AT_UP;
    else
      _vs = JOYSTICK_AT_CENTER;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the analog input in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

float JoystickDevice::joystickHorizontal() const
{
  //USUL_TRACE_SCOPE;
  return 2.0f * ( this->horizontal() + _analogTrim[0] ) - 1.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the analog input in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

float JoystickDevice::joystickVertical() const
{
  //USUL_TRACE_SCOPE;
  return 2.0f * ( this->vertical() + _analogTrim[1] ) - 1.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the analog trim value
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::analogTrim( float x, float y )
{
  _analogTrim.set( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::name( const std::string &name )
{
  _name = name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string JoystickDevice::name() const
{
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the modifier.  Should be either 1.0 or -1.0
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::horizontalModifier( float modifier )
{
  _horizontalModifier = modifier;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the modifier.  Should be either 1.0 or -1.0
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::verticalModifier( float modifier )
{
  _verticalModifier= modifier;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the current modifier
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec2f JoystickDevice::modifier()
{
  Usul::Math::Vec2f modifier ( _horizontalModifier, _verticalModifier );
  return modifier;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the device's horizontal state.
//
///////////////////////////////////////////////////////////////////////////////

JoystickDevice::Message JoystickDevice::horizontalState() const
{
  return _hs;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the device's vertical state.
//
///////////////////////////////////////////////////////////////////////////////

JoystickDevice::Message JoystickDevice::verticalState() const
{
  return _vs;
}
