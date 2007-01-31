
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

#include "vrjGaPrecompiled.h"
#include "JoystickDevice.h"

using namespace vrjGA;


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
  _vs ( JOYSTICK_AT_CENTER )
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
//  Update the internal state.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickDevice::update()
{
  _h = _hai->getData();
  _v = _vai->getData();
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
  if ( _h > high )
    _hs = JOYSTICK_AT_RIGHT;
  else if ( _h < low )
    _hs = JOYSTICK_AT_LEFT;
  else
    _hs = JOYSTICK_AT_CENTER;

  // Vertical.
  if ( _v > high )
    _vs = JOYSTICK_AT_UP;
  else if ( _v < low )
    _vs = JOYSTICK_AT_DOWN;
  else
    _vs = JOYSTICK_AT_CENTER;
}
