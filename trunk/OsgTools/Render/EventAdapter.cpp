
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Adapts events for OSG.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/EventAdapter.h"

using namespace OsgTools::Render;

#if 0
///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EventAdapter::EventAdapter() : BaseClass (),
  _type    ( BaseClass::NONE ),
  _key     ( -1 ),
  _button  ( -1 ),
  _size    ( 100, 100 ),
  _mouse   ( 0, 0 ),
  _buttons ( 0 ),
  _seconds ( 0 )
{
	this->setMouseYOrientation ( Y_INCREASING_UPWARDS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EventAdapter::~EventAdapter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setWindowSize ( const Usul::Math::Vec2ui &size )
{
  _size.set ( size[0], size[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mouse coordinate in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setMouse ( const Usul::Math::Vec2f &xy )
{
  const float mouseX ( xy[0] );
  const float mouseY ( xy[1] );
  _mouse.set ( mouseX, mouseY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the event type.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setEventType ( const EventType &type )
{
  _type = type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the event time.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setSeconds ( double seconds )
{
  _seconds = seconds;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the buttons mask.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setButtonMask ( bool left, bool middle, bool right )
{
  _buttons = 0;
  _buttons |= ( ( left   ) ? BaseClass::LEFT_MOUSE_BUTTON   : 0 );
  _buttons |= ( ( middle ) ? BaseClass::MIDDLE_MOUSE_BUTTON : 0 );
  _buttons |= ( ( right  ) ? BaseClass::RIGHT_MOUSE_BUTTON  : 0 );
}

#else
///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EventAdapter::EventAdapter() : BaseClass ()
{
	this->setMouseYOrientation ( Y_INCREASING_UPWARDS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EventAdapter::~EventAdapter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setWindowSize ( const Usul::Math::Vec2ui &size )
{
  _Xmin = _Ymin = 0.0;
  _Xmax = size[0];
  _Ymax = size[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mouse coordinate in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setMouse ( const Usul::Math::Vec2f &xy )
{
  const float mouseX ( xy[0] );
  const float mouseY ( xy[1] );
  _mx = mouseX;
  _my = mouseY;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the event type.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setEventType ( const EventType &type )
{
  _eventType = type;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the event time.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setSeconds ( double seconds )
{
  _time = seconds;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the buttons mask.
//
///////////////////////////////////////////////////////////////////////////////

void EventAdapter::setButtonMask ( bool left, bool middle, bool right )
{
  _buttonMask = 0;
  _buttonMask |= ( ( left   ) ? BaseClass::LEFT_MOUSE_BUTTON   : 0 );
  _buttonMask |= ( ( middle ) ? BaseClass::MIDDLE_MOUSE_BUTTON : 0 );
  _buttonMask |= ( ( right  ) ? BaseClass::RIGHT_MOUSE_BUTTON  : 0 );
}
#endif
