
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgFox/Views/FoxCanvas.h"
#include "OsgFox/Views/FoxContext.h"
#include "OsgFox/Views/Registry.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Interfaces/ISetCursor.h"

#include "OsgTools/Render/Defaults.h"

using namespace OsgFox::Views;

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FoxCanvas ) CanvasMap[] =
{
  // Message type,                     id,                                                      handler.
  FXMAPFUNC ( FX::SEL_CONFIGURE,       0,                                                       FoxCanvas::onResize                      ),
  FXMAPFUNC ( FX::SEL_PAINT,           0,                                                       FoxCanvas::onPaint                       ),
  FXMAPFUNC ( FX::SEL_TIMEOUT,         FoxCanvas::ID_ANIMATION_TIMEOUT,                         FoxCanvas::onTimeoutAnimation            ),
  FXMAPFUNC ( FX::SEL_TIMEOUT,         FoxCanvas::ID_SPIN_TIMEOUT,                              FoxCanvas::onTimeoutSpin                 ),
};

// This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT_ABSTRACT ( FoxCanvas, BaseClass, CanvasMap, ARRAYNUMBER ( CanvasMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor (needed for FOX_TOOLS_IMPLEMENT macro).
//
///////////////////////////////////////////////////////////////////////////////

FoxCanvas::FoxCanvas() : 
  BaseClass(),
  _viewer          ( 0x0 ),
  _timeouts        (),
  _rotCursor       ( 0x0 ),
  _transCursor     ( 0x0 ),
  _scaleCursor     ( 0x0 ),
  _navCursor       ( 0x0 ),
  _pickCursor      ( 0x0 )
{
  // Do not use this constructor.
  USUL_ERROR_CHECKER ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxCanvas::FoxCanvas ( Document *doc, FX::FXComposite *parent, FX::FXGLVisual *visual ) :
  BaseClass ( parent, visual, (FX::FXObject*) 0x0, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT ),
  _viewer          ( 0x0 ),
  _timeouts        (),
  _rotCursor       ( Cursor::make ( Cursor::CURSOR_ROTATE    ) ),
  _transCursor     ( Cursor::make ( Cursor::CURSOR_TRANSLATE ) ),
  _scaleCursor     ( Cursor::make ( Cursor::CURSOR_SCALE     ) ),
  _navCursor       ( Cursor::make ( Cursor::CURSOR_ROTATE    ) ),
  _pickCursor      ( Cursor::make ( Cursor::CURSOR_ARROW     ) )
{
  FoxContext::ValidRefPtr context ( new FoxContext ( *this ) );

  // Create the viewer.
  _viewer = new Viewer ( doc, context, this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxCanvas::~FoxCanvas()
{
  // Clear the viewer
  _viewer->clear();

  // Unref viewer.
  _viewer = 0x0;

}

///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::create()
{
  // Call the base class's function first.
  BaseClass::create();

  if ( _viewer.valid() )
  {
    // Create the viewer.
    _viewer->create();

    // Get the background color from the registry
    _viewer->backgroundColor ( OsgFox::Registry::read ( Usul::Registry::Sections::OPEN_GL_CANVAS, Usul::Registry::Keys::CLEAR_COLOR, OsgTools::Render::Defaults::CLEAR_COLOR ) );
  }

  // Create the cursors.
  _rotCursor->create();
  _transCursor->create();
  _scaleCursor->create();
  _navCursor->create();
  _pickCursor->create();

  // Pop this window to the front.
  this->setFocus();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

FoxCanvas::Document * FoxCanvas::document()
{
  return _viewer->document();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

const FoxCanvas::Document * FoxCanvas::document() const
{
  return _viewer->document();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

long FoxCanvas::onResize ( FX::FXObject *, FX::FXSelector, void *data )
{
  // Get the event.
  USUL_ERROR_CHECKER ( 0x0 != data );
  FX::FXEvent *event = (FX::FXEvent *) data;

  // Resize the window.
  unsigned int width  ( static_cast < unsigned int > ( event->rect.w ) );
  unsigned int height ( static_cast < unsigned int > ( event->rect.h ) );
  this->_resize ( width, height );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the window needs to be repainted.
//
///////////////////////////////////////////////////////////////////////////////

long FoxCanvas::onPaint ( FX::FXObject *, FX::FXSelector, void * )
{
  if ( _viewer.valid() )
  {
    _viewer->updateScene();
    _viewer->render();
  }
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

FoxCanvas::Viewer* FoxCanvas::viewer()
{
  return _viewer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const FoxCanvas::Viewer* FoxCanvas::viewer() const
{
  return _viewer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::viewer ( Viewer *v )
{
  _viewer = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::_resize ( unsigned int width, unsigned int height )
{
  _viewer->resize ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * FoxCanvas::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITimeoutAnimate::IID:
    return static_cast < Usul::Interfaces::ITimeoutAnimate* > ( this );
  case Usul::Interfaces::ITimeoutSpin::IID:
    return static_cast < Usul::Interfaces::ITimeoutSpin* > ( this );
  case Usul::Interfaces::ISetCursorType::IID:
    return static_cast < Usul::Interfaces::ISetCursorType* > ( this );
  default:
    return 0x0;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Ref
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::ref()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unref.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::unref ( bool )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::startAnimation ( double timeout )
{
  // Use the callback to animate.
  this->getApp()->addTimeout ( this, ID_ANIMATION_TIMEOUT, timeout );
  _timeouts[ ID_ANIMATION_TIMEOUT ] = timeout;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback that we are using for Animation.
//
///////////////////////////////////////////////////////////////////////////////

long FoxCanvas::onTimeoutAnimation ( FX::FXObject *, FX::FXSelector, void * )
{
  if( this->viewer() )
  {
    // Set the matrix and see if we should continue.
    if ( this->viewer()->timeoutAnimate () ) 
      this->getApp()->addTimeout ( this, ID_ANIMATION_TIMEOUT, _timeouts[ ID_ANIMATION_TIMEOUT ] );
  }
  
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the timer.
//
///////////////////////////////////////////////////////////////////////////////

long FoxCanvas::onTimeoutSpin ( FX::FXObject *object, FX::FXSelector selector, void *callData )
{
  if( this->viewer() )
  {
    this->viewer()->timeoutSpin();

    // Set a new timeout event.
    this->getApp()->addTimeout ( this, ID_SPIN_TIMEOUT, _timeouts[ ID_SPIN_TIMEOUT ] );
  }

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the spin timeout.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::stopSpin ( )
{
  // If there is a timer...
  if ( this->getApp()->hasTimeout ( this, ID_SPIN_TIMEOUT ) )
  {
    // Remove the timeout event.
    this->getApp()->removeTimeout ( this, ID_SPIN_TIMEOUT );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the spin timeout.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::startSpin ( double timeout )
{
  this->getApp()->addTimeout ( this, ID_SPIN_TIMEOUT, timeout );
  _timeouts[ ID_SPIN_TIMEOUT ] = timeout;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::setCursor ( FX::FXCursor *cursor )
{
  if ( cursor )
  {
    // We get the desired behavior when we set both.
    this->setDefaultCursor ( cursor );
    this->setDragCursor ( cursor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cursor.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::updateCursor()
{
  FX::FXCursor *cursor;
  
  // Check to see if there is a tool command loaded
  if ( 0x0 != this->viewer()->getTool() )
  {
    // See if this tool command sets its own cursor
    Usul::Interfaces::ISetCursor::QueryPtr setCursor ( this->viewer()->getTool() );
    if ( setCursor.valid() )
      cursor = setCursor->getCursor();

    // If not use the pick cursor as default
    else
      cursor = this->cursorPick();
  }
  // If no tool loaded...
  else
  {
    // Set the proper cursor
    switch ( this->viewer()->getMode() )
    {
    case OsgTools::Render::Viewer::NAVIGATION:
      cursor = this->cursorNav();
      break;
    case OsgTools::Render::Viewer::PICK:
      cursor = this->cursorPick();
      break;
    }
  }

  // Set the cursor
  this->setCursor ( cursor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor to the navigation cursor.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::setCursorNav()
{
  this->setCursor ( this->cursorNav() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor to the translation cursor.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::setCursorTrans()
{
  this->setCursor ( this->cursorTrans() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor to the rotation cursor.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::setCursorRot()
{
  this->setCursor ( this->cursorRot() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor to the scale cursor.
//
///////////////////////////////////////////////////////////////////////////////

void FoxCanvas::setCursorScale()
{
  this->setCursor ( this->cursorScale() );
}

