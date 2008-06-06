
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL view for the GUI-FOX library.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "SgFoxView.h"

#include "GUI/FOX/FoxDefine.h"
#include "GUI/FOX/FoxCursor.h"

#include "Standard/SlAssert.h"

# include "SceneGraph/Core/SgMessageIds.h"

using namespace CadKit;


//////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
//////////////////////////////////////////////////////////////////////////

FXDEFMAP ( SgFoxView ) SgFoxViewMap[] =
{
  // Message type, id, and handler.
  FXMAPFUNC ( SEL_PAINT, 0, SgFoxView::onPaint ),
  FXMAPFUNC ( SEL_MOTION, 0, SgFoxView::onMotion ),
  FXMAPFUNC ( SEL_TIMEOUT, 0, SgFoxView::onTimeout ),
  FXMAPFUNC ( SEL_LEFTBUTTONPRESS, 0, SgFoxView::onLeftBtnPress ),
  FXMAPFUNC ( SEL_LEFTBUTTONRELEASE, 0, SgFoxView::onLeftBtnRelease ),
  FXMAPFUNC ( SEL_MIDDLEBUTTONPRESS, 0, SgFoxView::onMiddleBtnPress ),
  FXMAPFUNC ( SEL_MIDDLEBUTTONRELEASE, 0, SgFoxView::onMiddleBtnRelease ),
  FXMAPFUNC ( SEL_RIGHTBUTTONPRESS, 0, SgFoxView::onRightBtnPress ),
  FXMAPFUNC ( SEL_RIGHTBUTTONRELEASE, 0, SgFoxView::onRightBtnRelease ),
  FXMAPFUNC ( SEL_KEYPRESS, 0, SgFoxView::onKeyPress ),
  FXMAPFUNC ( SEL_KEYRELEASE, 0, SgFoxView::onKeyRelease ),
  FXMAPFUNC ( SEL_CONFIGURE, 0, SgFoxView::onConfigure ),
};

// This class, base class, array of message handlers, size of the array.
FXIMPLEMENT ( SgFoxView, SgFoxCanvas, SgFoxViewMap, ARRAYNUMBER ( SgFoxViewMap ) );


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

SgFoxView::SgFoxView ( FXComposite *parent, FXGLVisual *visual ) :
  SgFoxCanvas ( parent, visual ),
  _viewer ( 0x0 ),
  _timer ( 0x0 ),
  _rotateCursor ( 0x0 ),
  _translateCursor ( 0x0 ),
  _scaleCursor ( 0x0 ),
  _viewingCursor ( 0x0 ),
  _pickingCursor ( 0x0 ),
  _seekingCursor ( 0x0 )
{
  // Empty.
}


//////////////////////////////////////////////////////////////////////////
//
//  Default constructor (needed for FXIMPLEMENT macro).
//
//////////////////////////////////////////////////////////////////////////

SgFoxView::SgFoxView() : 
  SgFoxCanvas(), 
  _viewer ( 0x0 ),
  _timer ( 0x0 ),
  _rotateCursor ( 0x0 ),
  _translateCursor ( 0x0 ),
  _scaleCursor ( 0x0 ),
  _viewingCursor ( 0x0 ),
  _pickingCursor ( 0x0 ),
  _seekingCursor ( 0x0 )
{
  // Don't use this constructor.
  SL_ASSERT ( 0 );
}


//////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
//////////////////////////////////////////////////////////////////////////

SgFoxView::~SgFoxView()
{
  // If we have an active timer, then remove it.
  SL_VERIFY ( true == this->stopSpin() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
/////////////////////////////////////////////////////////////////////////////

bool SgFoxView::init()
{
  SL_ASSERT ( this );
  SL_ASSERT ( this->isCreated() );

  // Make new cursors.
  _rotateCursor =    FoxCursor::create ( FoxCursor::CURSOR_ROTATE,    this->isCreated(), this->getApp() );
  _translateCursor = FoxCursor::create ( FoxCursor::CURSOR_TRANSLATE, this->isCreated(), this->getApp() );
  _scaleCursor =     FoxCursor::create ( FoxCursor::CURSOR_SCALE,     this->isCreated(), this->getApp() );
  _viewingCursor =   FoxCursor::create ( FoxCursor::CURSOR_ROTATE,    this->isCreated(), this->getApp() );
  _pickingCursor =   FoxCursor::create ( FoxCursor::CURSOR_ARROW,     this->isCreated(), this->getApp() );
  _seekingCursor =   FoxCursor::create ( FoxCursor::CURSOR_SEEK,      this->isCreated(), this->getApp() );

  // Check allocation.
  GUI_FOX_CHECK_EXPRESSION ( _rotateCursor.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( _translateCursor.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( _scaleCursor.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( _viewingCursor.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( _pickingCursor.isValid() );
  GUI_FOX_CHECK_EXPRESSION ( _seekingCursor.isValid() );

  // Set the default cursor.
  this->setDefaultCursor ( _viewingCursor );

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Render the viewer if there is one.
//
/////////////////////////////////////////////////////////////////////////////

bool SgFoxView::render()
{
  SL_ASSERT ( this );

  // If there's a viewer then render it.
  if ( _viewer ) 
    return _viewer->render();

  // If not...
  return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  View the whole scene.
//
/////////////////////////////////////////////////////////////////////////////

bool SgFoxView::viewAll ( bool renderNow )
{
  SL_ASSERT ( this );

  // If there's a viewer...
  if ( _viewer ) 
  {
    // Then tell it to view all.
    if ( false == _viewer->viewAll() ) 
      return false;

    // If we are supposed to render now...
    if ( renderNow ) 
      return this->render();

    // Otherwise, we did good.
    else 
      return true;
  }

  // If not...
  return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
/////////////////////////////////////////////////////////////////////////////

void SgFoxView::setViewer ( SgViewer *viewer ) 
{
  SL_ASSERT ( this );

  // If they passed the same one then return.
  if ( _viewer == viewer ) 
    return;

  // Set the new one.
  _viewer = viewer;

  // If they passed a good value...
  if ( _viewer.isValid() )
  {
    // Set it's callback function.
    viewer->setCallback ( &SgFoxView::_viewerMessageCallback, this );
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Callback for the viewer.
//
//////////////////////////////////////////////////////////////////////////

bool SgFoxView::_viewerMessageCallback ( SgViewer::Message &message )
{
  SL_ASSERT ( message.getClientData() );
  SgFoxView *me = (SgFoxView *) message.getClientData();
  return me->_viewerMessageExecution ( message );
}


//////////////////////////////////////////////////////////////////////////
//
//  Message handler for the viewer.
//
//////////////////////////////////////////////////////////////////////////

bool SgFoxView::_viewerMessageExecution ( SgViewer::Message &message )
{
  SL_ASSERT ( this );
  SL_ASSERT ( _rotateCursor.isValid() );
  SL_ASSERT ( _translateCursor.isValid() );
  SL_ASSERT ( _scaleCursor.isValid() );

  // Get the message id.
  switch ( message.getId() )
  {
  case SG_VIEWER_SPIN_START:

    // Should only be in here when there is no timer.
    SL_ASSERT ( _timer == 0x0 );
    return this->spin();
  
  case SG_VIEWER_SPIN_FINISH:

    // Should be a good timer.
    SL_ASSERT ( _timer );
    return this->stopSpin();

  case SG_VIEWER_ROTATING:

    // Set the appropriate cursor.
    this->_setCursor ( _rotateCursor );
    break;

  case SG_VIEWER_TRANSLATING:

    // Set the appropriate cursor.
    this->_setCursor ( _translateCursor );
    break;

  case SG_VIEWER_SCALING:

    // Set the appropriate cursor.
    this->_setCursor ( _scaleCursor );
    break;

  case SG_VIEWER_VIEWING_MODE:

    // Set the appropriate cursor.
    this->_setCursor ( _viewingCursor );
    break;

  case SG_VIEWER_PICKING_MODE:

    // Set the appropriate cursor.
    this->_setCursor ( _pickingCursor );
    break;

  case SG_VIEWER_SEEKING_MODE:

    // Set the appropriate cursor.
    this->_setCursor ( _seekingCursor );
    break;

  case SG_VIEWER_SEEK_MISS:

    break;

  case SG_VIEWER_SEEK_HIT:

    break;

  default:

    SL_ASSERT ( 0 ); // What message is this?
    return false;
  }

  // If we get this far then we did good.
  return true;
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the cursor.
//
//////////////////////////////////////////////////////////////////////////

void SgFoxView::_setCursor ( FXCursor *cursor )
{
  SL_ASSERT ( this );
  SL_ASSERT ( cursor );

  // We get the desired behavior when we set both.
  this->setDefaultCursor ( cursor );
  this->setDragCursor ( cursor );
}


//////////////////////////////////////////////////////////////////////////
//
//  Spin the geometry.
//
//////////////////////////////////////////////////////////////////////////

bool SgFoxView::spin()
{
  SL_ASSERT ( this );

  // We can't spin if there is no viewer.
  if ( _viewer.isNull() ) 
    return false;

  // First, stop any spin there is.
  if ( false == this->stopSpin() ) 
    return false;

  // Should be true.
  SL_ASSERT ( _timer == 0x0 );

  // Set a new timeout event.
  _timer = this->getApp()->addTimeout ( this, 0, _viewer->getSpinTimerInterval() );

  // We should have a good timer now.
  return ( NULL != _timer );
}


//////////////////////////////////////////////////////////////////////////
//
//  Stop the spinning.
//
//////////////////////////////////////////////////////////////////////////

bool SgFoxView::stopSpin()
{
  // If there is a timer...
  if ( _timer )
  {
    // Remove the timeout event.
    this->getApp()->removeTimeout ( _timer );

    // Reset the timer pointer.
    _timer = 0x0;
  }

  // It worked.
  return true;
}


//////////////////////////////////////////////////////////////////////////
//
//  Called by the timer.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onTimeout ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( _timer );
  SL_ASSERT ( _viewer.isValid() );

  // Set a new timeout event.
  _timer = this->getApp()->addTimeout ( this, 0, _viewer->getSpinTimerInterval() );

  // Tell the viewer to spin the camera a little.
  _viewer->spinNotify();

  // Tell the viewer to render now.
  return ( _viewer->render() ) ? 1 : 0;
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the window needs to be repainted.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onPaint ( FXObject *, FXSelector, void * )
{
  SL_ASSERT ( this );

  // If we have a viewer, tell it to render.
  if ( _viewer.isValid() ) 
    return ( _viewer->render() ) ? 1 : 0;

  // If we don't have a viewer, then we didn't paint.
  return 0;
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the mouse buttons that are down.
//
//////////////////////////////////////////////////////////////////////////

SlUint32 SgFoxView::_getMouseButtonsDown ( const FXEvent *event ) const
{
  SL_ASSERT ( this );
  SL_ASSERT ( event );

  // Determine what buttons are down.
  SlUint32 buttonsDown = 0;
  if ( event->state & LEFTBUTTONMASK )   
  {
    buttonsDown |= SgViewer::MOUSE_LEFT;
  }
  if ( event->state & MIDDLEBUTTONMASK ) 
  {
    buttonsDown |= SgViewer::MOUSE_MIDDLE;
  }
  if ( event->state & RIGHTBUTTONMASK )  
  {
    buttonsDown |= SgViewer::MOUSE_RIGHT;
  }

  // Return the buttons down.
  return buttonsDown;
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the mouse is moved over the window.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onMotion ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If there's a viewer...
  if ( _viewer )
  {
    // Tell the viewer that the mouse moved.
    SL_VERIFY ( true == _viewer->mouseMoved ( event->win_x, event->win_y, this->_getMouseButtonsDown ( event ) ) );
    //static unsigned int count = 0; SL_TRACE ( "In SgFoxView::onMotion(), x = %3d, y = %3d, event = %3d, count = %3d\n", event->win_x, event->win_y, event->state, count++ );
  }

  // Call the base class's function.
  return SgFoxCanvas::onMotion ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when a key is pressed.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onKeyPress ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that a key is down.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->keyDown ( event->code ) );

  // Call the base class's function.
  return SgFoxCanvas::onKeyPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when a key is released.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onKeyRelease ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that a key is up.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->keyUp ( event->code ) );

  // Call the base class's function.
  return SgFoxCanvas::onKeyRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is pressed.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onLeftBtnPress ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->mouseButtonDown ( event->click_x, event->click_y, SgViewer::MOUSE_LEFT, this->_getMouseButtonsDown ( event ) ) );

  // Call the base class's function.
  return SgFoxCanvas::onLeftBtnPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the left mouse button is released.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onLeftBtnRelease ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->mouseButtonUp ( event->click_x, event->click_y, SgViewer::MOUSE_LEFT, this->_getMouseButtonsDown ( event ) ) );

  // Call the base class's function.
  return SgFoxCanvas::onLeftBtnRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the middle mouse button is pressed.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onMiddleBtnPress ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->mouseButtonDown ( event->click_x, event->click_y, SgViewer::MOUSE_MIDDLE, this->_getMouseButtonsDown ( event ) ) );

  // Call the base class's function.
  return SgFoxCanvas::onMiddleBtnPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the middle mouse button is released.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onMiddleBtnRelease ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->mouseButtonUp ( event->click_x, event->click_y, SgViewer::MOUSE_MIDDLE, this->_getMouseButtonsDown ( event ) ) );

  // Call the base class's function.
  return SgFoxCanvas::onMiddleBtnRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the right mouse button is pressed.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onRightBtnPress ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->mouseButtonDown ( event->click_x, event->click_y, SgViewer::MOUSE_RIGHT, this->_getMouseButtonsDown ( event ) ) );

  // Call the base class's function.
  return SgFoxCanvas::onRightBtnPress ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when the right mouse button is released.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onRightBtnRelease ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the mouse is down.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->mouseButtonUp ( event->click_x, event->click_y, SgViewer::MOUSE_RIGHT, this->_getMouseButtonsDown ( event ) ) );

  // Call the base class's function.
  return SgFoxCanvas::onRightBtnRelease ( object, selector, callData );
}


//////////////////////////////////////////////////////////////////////////
//
//  Called when this window is resized.
//
//////////////////////////////////////////////////////////////////////////

long SgFoxView::onConfigure ( FXObject *object, FXSelector selector, void *callData )
{
  SL_ASSERT ( this );
  SL_ASSERT ( callData );

  // Get the event.
  FXEvent *event = (FXEvent *) callData;

  // If we have a viewer, tell it that the size changed.
  if ( _viewer ) 
    SL_VERIFY ( true == _viewer->resize ( event->rect.w, event->rect.h ) );

  // Call the base class's function.
  return SgFoxCanvas::onConfigure ( object, selector, callData );
}
