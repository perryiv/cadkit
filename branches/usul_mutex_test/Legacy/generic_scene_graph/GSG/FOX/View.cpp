
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL view.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/FOX/Precompiled.h"
#include "GSG/FOX/View.h"

#include "GSG/Core/Viewer.h"

#define SPIN_TIMER_INTERVAL 1

using namespace GSG;
using namespace GSG::FOX;


//////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
//////////////////////////////////////////////////////////////////////////

FXDEFMAP ( View ) ViewMap[] =
{
  // Message type, id, and handler.
  FXMAPFUNC ( SEL_PAINT,               0, View::onPaint ),
  FXMAPFUNC ( SEL_MOTION,              0, View::onMotion ),
  FXMAPFUNC ( SEL_TIMEOUT,             0, View::onTimeout ),
  FXMAPFUNC ( SEL_LEFTBUTTONPRESS,     0, View::onLeftBtnPress ),
  FXMAPFUNC ( SEL_LEFTBUTTONRELEASE,   0, View::onLeftBtnRelease ),
  FXMAPFUNC ( SEL_MIDDLEBUTTONPRESS,   0, View::onMiddleBtnPress ),
  FXMAPFUNC ( SEL_MIDDLEBUTTONRELEASE, 0, View::onMiddleBtnRelease ),
  FXMAPFUNC ( SEL_RIGHTBUTTONPRESS,    0, View::onRightBtnPress ),
  FXMAPFUNC ( SEL_RIGHTBUTTONRELEASE,  0, View::onRightBtnRelease ),
  FXMAPFUNC ( SEL_KEYPRESS,            0, View::onKeyPress ),
  FXMAPFUNC ( SEL_KEYRELEASE,          0, View::onKeyRelease ),
  FXMAPFUNC ( SEL_CONFIGURE,           0, View::onConfigure ),
};

// This class, base class, array of message handlers, size of the array.
FXIMPLEMENT ( View, Canvas, ViewMap, ARRAYNUMBER ( ViewMap ) );


//////////////////////////////////////////////////////////////////////////
//
//  Default constructor (needed for FXIMPLEMENT macro).
//
//////////////////////////////////////////////////////////////////////////

View::View() : 
  Canvas(), 
  _viewer          ( 0x0 ),
  _timer           ( 0x0 ),
  _rotateCursor    ( 0x0 ),
  _translateCursor ( 0x0 ),
  _scaleCursor     ( 0x0 ),
  _viewingCursor   ( 0x0 ),
  _pickingCursor   ( 0x0 ),
  _seekingCursor   ( 0x0 )
{
  // Do not use this constructor.
  GSG_ASSERT ( false );
  ErrorChecker ( false );
}


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

View::View ( FXComposite *parent, FXGLVisual *visual ) :
  Canvas ( parent, visual ), 
  _viewer          ( 0x0 ),
  _timer           ( 0x0 ),
  _rotateCursor    ( Cursor::create ( Cursor::CURSOR_ROTATE,    false, visual->getApp() ) ),
  _translateCursor ( Cursor::create ( Cursor::CURSOR_TRANSLATE, false, visual->getApp() ) ),
  _scaleCursor     ( Cursor::create ( Cursor::CURSOR_SCALE,     false, visual->getApp() ) ),
  _viewingCursor   ( Cursor::create ( Cursor::CURSOR_ROTATE,    false, visual->getApp() ) ),
  _pickingCursor   ( Cursor::create ( Cursor::CURSOR_ARROW,     false, visual->getApp() ) ),
  _seekingCursor   ( Cursor::create ( Cursor::CURSOR_SEEK,      false, visual->getApp() ) )
{
  // Check allocation.
  ErrorChecker ( 0x0 != _rotateCursor.get() );
  ErrorChecker ( 0x0 != _translateCursor.get() );
  ErrorChecker ( 0x0 != _scaleCursor.get() );
  ErrorChecker ( 0x0 != _viewingCursor.get() );
  ErrorChecker ( 0x0 != _pickingCursor.get() );
  ErrorChecker ( 0x0 != _seekingCursor.get() );

  // Set the default cursor.
  this->setDefaultCursor ( _viewingCursor.get() );
}


//////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
//////////////////////////////////////////////////////////////////////////

View::~View()
{
  // If we have an active timer, then remove it.
  this->spin ( false );

  // Remove the viewer-callback.
  _viewer->callback ( 0x0 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Viewer-callback class.
//
/////////////////////////////////////////////////////////////////////////////

namespace GSG
{
  namespace Detail
  {
    // Callback class.
    struct ViewerCB : public Viewer::Callback
    {
      GSG_DECLARE_REFERENCED ( ViewerCB );
      ViewerCB ( View *v ) : Viewer::Callback(), _v ( v ){}
      ViewerCB ( const ViewerCB &cb ) : Viewer::Callback ( cb ), _v ( cb._v ){}
      virtual void operator () ( Viewer::Message id, Path *, Viewer & );
    protected:
      GSG::FOX::View *_v;
      virtual ~ViewerCB();
    };
    GSG_IMPLEMENT_REFERENCED ( ViewerCB );
    void ViewerCB::operator () ( Viewer::Message id, Path *path, Viewer &viewer )
    {
      if ( _v )
        _v->notify ( id, path, viewer );
    }
    ViewerCB::~ViewerCB(){}
    void ViewerCB::setFrom ( const ViewerCB &v )
    {
      Lock lock ( this );
      _v = v._v;
      BaseClass::setFrom ( v );
    }
  };
};


/////////////////////////////////////////////////////////////////////////////
//
//  Render the viewer if there is one.
//
/////////////////////////////////////////////////////////////////////////////

void View::render()
{
  // If there's a viewer then render it.
  if ( _viewer.valid() ) 
    _viewer->render();
}


/////////////////////////////////////////////////////////////////////////////
//
//  View the whole scene.
//
/////////////////////////////////////////////////////////////////////////////

void View::viewAll ( bool renderNow )
{
  // If there is a viewer...
  if ( _viewer.valid() ) 
  {
    // Then tell it to view all.
    _viewer->viewAll();

    // If we are supposed to render now...
    if ( renderNow ) 
      this->render();
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
/////////////////////////////////////////////////////////////////////////////

void View::viewer ( Viewer *viewer ) 
{
  // Set the new one.
  _viewer = viewer;

  // If they passed a good value...
  if ( _viewer.valid() )
  {
    // Set its callback.
    viewer->callback ( new Detail::ViewerCB ( this ) );
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Message handler for the viewer.
//
//////////////////////////////////////////////////////////////////////////

void View::notify ( Viewer::Message id, Path *path, Viewer &viewer )
{
  // Behavior is based on the message id.
  switch ( id )
  {
  case Viewer::SPIN_START:

    // Should only be in here when there is no timer.
    ErrorChecker ( _timer == 0x0 );
    this->spin ( true );
    break;
  
  case Viewer::SPIN_FINISH:

    // Should be a good timer.
    ErrorChecker ( 0x0 != _timer );
    this->spin ( false );
    break;

  case Viewer::ROTATING:

    // Set the appropriate cursor.
    this->_setCursor ( _rotateCursor.get() );
    break;

  case Viewer::TRANSLATING:

    // Set the appropriate cursor.
    this->_setCursor ( _translateCursor.get() );
    break;

  case Viewer::SCALING:

    // Set the appropriate cursor.
    this->_setCursor ( _scaleCursor.get() );
    break;

  case Viewer::VIEWING_MODE:

    // Set the appropriate cursor.
    this->_setCursor ( _viewingCursor.get() );
    break;

  case Viewer::PICKING_MODE:

    // Set the appropriate cursor.
    this->_setCursor ( _pickingCursor.get() );
    break;

  case Viewer::SEEKING_MODE:

    // Set the appropriate cursor.
    this->_setCursor ( _seekingCursor.get() );
    break;

  case Viewer::SEEK_MISS:

    break;

  case Viewer::SEEK_HIT:

    break;

  default:

    GSG_ASSERT ( 0 ); // What message is this?
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the cursor.
//
//////////////////////////////////////////////////////////////////////////

void View::_setCursor ( FXCursor *cursor )
{
  ErrorChecker ( 0x0 != cursor );

  // We get the desired behavior when we set both.
  this->setDefaultCursor ( cursor );
  this->setDragCursor ( cursor );
}


//////////////////////////////////////////////////////////////////////////
//
//  Start/stop the geometry spinning.
//
//////////////////////////////////////////////////////////////////////////

void View::spin ( bool state )
{
  // Stop any spin there is.
  if ( _timer )
  {
    // Remove the timeout event.
    this->getApp()->removeTimeout ( _timer );

    // Reset the timer pointer.
    _timer = 0x0;
  }

  // Should be true.
  GSG_ASSERT ( 0x0 == _timer );

  // If we are supposed to spin.
  if ( state && _viewer.valid() ) 
  {
    // Set a new timeout event.
    _timer = this->getApp()->addTimeout ( this, 0, SPIN_TIMER_INTERVAL );
  }

  // We should have a good timer now.
  GSG_ASSERT ( ( 0x0 != _timer ) == state );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called by the timer.
//
//////////////////////////////////////////////////////////////////////////

long View::onTimeout ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != _timer );
  ErrorChecker ( _viewer.valid() );

  // Set a new timeout event.
  _timer = this->getApp()->addTimeout ( this, 0, SPIN_TIMER_INTERVAL );

  // Tell the viewer to spin the camera a little.
  _viewer->spinNotify();

  // Tell the viewer to render now.
  _viewer->render();

  // The event was handled.
  return 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the window needs to be repainted.
//
//////////////////////////////////////////////////////////////////////////

long View::onPaint ( FXObject *, FXSelector, void * )
{
  // If we have a viewer, tell it to render.
  if ( _viewer.valid() ) 
  {
    _viewer->render();
    return 1;
  }

  // If we don't have a viewer, then we didn't paint.
  return 0;
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the mouse buttons that are down.
//
//////////////////////////////////////////////////////////////////////////

unsigned int View::_buttonsDown ( const FXEvent *event ) const
{
  ErrorChecker ( 0x0 != event );

  // Determine what buttons are down.
  unsigned int buttonsDown = 0;
  if ( event->state & LEFTBUTTONMASK )   
  {
    buttonsDown |= Viewer::MOUSE_LEFT;
  }
  if ( event->state & MIDDLEBUTTONMASK ) 
  {
    buttonsDown |= Viewer::MOUSE_MIDDLE;
  }
  if ( event->state & RIGHTBUTTONMASK )  
  {
    buttonsDown |= Viewer::MOUSE_RIGHT;
  }

  // Return the buttons down.
  return buttonsDown;
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse is moved over the window.
//
//////////////////////////////////////////////////////////////////////////

long View::onMotion ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If there's a viewer then tell it that the mouse moved.
  if ( _viewer.valid() )
    _viewer->mouseMoved ( event->win_x, event->win_y, this->_buttonsDown ( event ) );

  // Call the base class's function.
  return Canvas::onMotion ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when a key is pressed.
//
//////////////////////////////////////////////////////////////////////////

long View::onKeyPress ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that a key is down.
  if ( _viewer.valid() ) 
    _viewer->keyDown ( event->code );

  // Call the base class's function.
  return Canvas::onKeyPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when a key is released.
//
//////////////////////////////////////////////////////////////////////////

long View::onKeyRelease ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that a key is up.
  if ( _viewer.valid() ) 
    _viewer->keyUp ( event->code );

  // Call the base class's function.
  return Canvas::onKeyRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is pressed.
//
//////////////////////////////////////////////////////////////////////////

long View::onLeftBtnPress ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer.valid() ) 
    _viewer->mouseButtonDown ( event->click_x, event->click_y, Viewer::MOUSE_LEFT, this->_buttonsDown ( event ) );

  // Call the base class's function.
  return Canvas::onLeftBtnPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is released.
//
//////////////////////////////////////////////////////////////////////////

long View::onLeftBtnRelease ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer.valid() ) 
    _viewer->mouseButtonUp ( event->click_x, event->click_y, Viewer::MOUSE_LEFT, this->_buttonsDown ( event ) );

  // Call the base class's function.
  return Canvas::onLeftBtnRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the middle mouse button is pressed.
//
//////////////////////////////////////////////////////////////////////////

long View::onMiddleBtnPress ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer.valid() ) 
    _viewer->mouseButtonDown ( event->click_x, event->click_y, Viewer::MOUSE_MIDDLE, this->_buttonsDown ( event ) );

  // Call the base class's function.
  return Canvas::onMiddleBtnPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the middle mouse button is released.
//
//////////////////////////////////////////////////////////////////////////

long View::onMiddleBtnRelease ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer.valid() ) 
    _viewer->mouseButtonUp ( event->click_x, event->click_y, Viewer::MOUSE_MIDDLE, this->_buttonsDown ( event ) );

  // Call the base class's function.
  return Canvas::onMiddleBtnRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the right mouse button is pressed.
//
//////////////////////////////////////////////////////////////////////////

long View::onRightBtnPress ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer.valid() ) 
    _viewer->mouseButtonDown ( event->click_x, event->click_y, Viewer::MOUSE_RIGHT, this->_buttonsDown ( event ) );

  // Call the base class's function.
  return Canvas::onRightBtnPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the right mouse button is released.
//
//////////////////////////////////////////////////////////////////////////

long View::onRightBtnRelease ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer.valid() ) 
    _viewer->mouseButtonUp ( event->click_x, event->click_y, Viewer::MOUSE_RIGHT, this->_buttonsDown ( event ) );

  // Call the base class's function.
  return Canvas::onRightBtnRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when this window is resized.
//
//////////////////////////////////////////////////////////////////////////

long View::onConfigure ( FXObject *object, FXSelector selector, void *callData )
{
  ErrorChecker ( 0x0 != callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the size changed.
  if ( _viewer.valid() ) 
    _viewer->resize ( event->rect.w, event->rect.h );

  // Call the base class's function.
  return Canvas::onConfigure ( object, selector, callData );
}
