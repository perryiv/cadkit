  
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxMouse.h"
#include "Usul/Devices/IButtonListener.h"
#include "Usul/Devices/State.h"

#include "FoxTools/Headers/Window.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/Definitions.h"

#include "Usul/Interfaces/IViewer.h"
#include "Usul/Interfaces/IGetBoundingBox.h"

#include "osg/BoundingBox"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FoxMouse ) MessageMap[] =
{
  // Message type, id, and handler.
  FXMAPFUNC ( FX::SEL_LEFTBUTTONPRESS,      0,   FoxMouse::onLeftBtnPress      ),  
  FXMAPFUNC ( FX::SEL_LEFTBUTTONRELEASE,    0,   FoxMouse::onLeftBtnRelease    ),  
  FXMAPFUNC ( FX::SEL_MIDDLEBUTTONPRESS,    0,   FoxMouse::onMiddleBtnPress    ),  
  FXMAPFUNC ( FX::SEL_MIDDLEBUTTONRELEASE,  0,   FoxMouse::onMiddleBtnRelease  ),
  FXMAPFUNC ( FX::SEL_RIGHTBUTTONPRESS,     0,   FoxMouse::onRightBtnPress     ),  
  FXMAPFUNC ( FX::SEL_RIGHTBUTTONRELEASE,   0,   FoxMouse::onRightBtnRelease   ),
  FXMAPFUNC ( FX::SEL_MOTION,               0,   FoxMouse::onMotion            ),
  FXMAPFUNC ( FX::SEL_KEYPRESS,             0,   FoxMouse::onKeyPress          ),
  FXMAPFUNC ( FX::SEL_KEYRELEASE,           0,   FoxMouse::onKeyRelease        ),
  FXMAPFUNC ( FX::SEL_MOUSEWHEEL,           0,   FoxMouse::onCommandMouseWheel ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( FoxMouse, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FoxMouse , FoxMouse::BaseDevice );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxMouse::FoxMouse ( FX::FXWindow *view, Usul::Interfaces::IUnknown* caller ) :
_view ( view )
{
  view->setTarget( this );
  view->setSelector ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxMouse::~FoxMouse()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Left Button Pressed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onLeftBtnPress     ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  Usul::Devices::State state;
  this->_setState ( state, *event );

  this->_notifyButtonPressed( 0, state );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Left Button Released.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onLeftBtnRelease   ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  Usul::Devices::State state;
  this->_setState ( state, *event );

  this->_notifyButtonReleased( 0, state );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Middle Button Pressed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onMiddleBtnPress   ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  Usul::Devices::State state;
  this->_setState ( state, *event );

  this->_notifyButtonPressed( 1, state );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Middle Button Released.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onMiddleBtnRelease ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  Usul::Devices::State state;
  this->_setState ( state, *event );

  this->_notifyButtonReleased( 1, state );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Right Button Pressed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onRightBtnPress    ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  Usul::Devices::State state;
  this->_setState ( state, *event );

  this->_notifyButtonPressed( 2, state );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Right Button Released.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onRightBtnRelease  ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  Usul::Devices::State state;
  this->_setState ( state, *event );

  this->_notifyButtonReleased( 2, state );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse moved
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onMotion ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent   *) data;

  Usul::Devices::State state;
  this->_setState ( state, *event );

  this->_notifyMotionListeners( state );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse wheel moved.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onCommandMouseWheel  ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent*) ( data );
  this->_notifyMouseWheelListeners ( event->code );
#if 0
  FX::FXEvent *event = (FX::FXEvent*) ( data );

  float distance         ( _trackball->getDistance() );

  Usul::Interfaces::IGetBoundingBox::QueryPtr getBound ( _trackball );
  if ( false == getBound.valid() )
    return 0;

  osg::BoundingBox bb ( getBound->getBoundingBox() );

  distance += event->code * ( .1 * bb.radius() ) / ::abs( event->code );

  _trackball->setDistance( distance );

  //_viewer->setViewMatrix( _trackball->getInverseMatrix() );

  Usul::Interfaces::IViewer::QueryPtr viewer ( _trackball );
  if ( viewer.valid() )
    viewer->render();
#endif
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* FoxMouse::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case IKeyboardDevice::IID:
    return static_cast < IKeyboardDevice* > ( this );
  case IDevice::IID:
    return static_cast < IDevice* > ( this );
  default:
    return 0x0;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void FoxMouse::_getXY ( const FX::FXEvent& event, float &x, float &y )
{
  float height ( (float) _view->getHeight() );
  float mouseX ( (float) event.win_x );
  float mouseY ( (float) event.win_y );

  x = mouseX;
  y = height - mouseY;
  //y = mouseY;
}


void FoxMouse::_setState ( Usul::Devices::State& state, const FX::FXEvent& event )
{
  bool left ( event.state & FX::LEFTBUTTONMASK ? true : false );
  bool middle ( event.state & FX::MIDDLEBUTTONMASK ? true : false );
  bool right ( event.state & FX::RIGHTBUTTONMASK ? true : false );

  float mouseX;
  float mouseY;

  this->_getXY( event, mouseX, mouseY );

  state.x( mouseX );
  state.y( mouseY );
  state.z( 0.0 );
  state.buttonPressed( 0, left );
  state.buttonPressed( 1, middle );
  state.buttonPressed( 2, right );

  state.numClicks( event.click_count );

  state.rootX( event.root_x );
  state.rootY( event.root_y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a key listener.
//
///////////////////////////////////////////////////////////////////////////////

void FoxMouse::addKeyListener    ( Usul::Devices::IKeyListener* listener )
{
  _keyListeners.push_back( listener );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a key listener.
//
///////////////////////////////////////////////////////////////////////////////

void FoxMouse::removeKeyListener ( Usul::Devices::IKeyListener* listener )
{
  KeyListeners::iterator iter = std::find_if ( _keyListeners.begin(), _keyListeners.end(), KeyListenerPtr::IsEqual ( listener ) );

  if( iter != _keyListeners.end() )
    _keyListeners.erase( iter );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key was pressed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onKeyPress        ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  for( KeyListeners::iterator iter = _keyListeners.begin(); iter != _keyListeners.end(); ++iter )
  {
    (*iter)->keyPressed( event->code );
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key was released.
//
///////////////////////////////////////////////////////////////////////////////

long FoxMouse::onKeyRelease      ( FX::FXObject *object, FX::FXSelector id, void *data )
{
  FX::FXEvent *event = (FX::FXEvent *) data;

  for( KeyListeners::iterator iter = _keyListeners.begin(); iter != _keyListeners.end(); ++iter )
  {
    (*iter)->keyReleased( event->code );
  }

  return 1;
}

