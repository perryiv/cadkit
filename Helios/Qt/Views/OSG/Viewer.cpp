
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Views/OSG/Viewer.h"

#include "Usul/Documents/Manager.h"

#include "QtCore/QTimer"
#include "QtGui/QResizeEvent"

using namespace CadKit::Helios::Views::OSG;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( Document *doc, const QGLFormat& format, QWidget* parent ) :
  BaseClass ( format, parent ),
  _document ( doc ),
  _viewer ( 0x0 ),
  _refCount ( 0 ),
  _timer ( 0x0 ),
  _keys(),
  _lastMode ( OsgTools::Render::Viewer::NAVIGATION )
{
  // For convienence;
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Create the viewer.
  _viewer = new OsgTools::Render::Viewer ( doc, me, me );

  // Initialize openGL.
  this->initializeGL ();

  // Create the viewer.  This functions calls openGL functions.
  _viewer->create();

  // Set the focus policy.
  this->setFocusPolicy ( Qt::ClickFocus );

  // Set the focus.
  this->setFocus();

  // Delete on close.
  this->setAttribute ( Qt::WA_DeleteOnClose );

  // Initalize the placement and size.
  this->_initPlacement ();

  // Add this to the document
  this->document()->addWindow ( this );

  // Initialize the timer.
  _timer = new QTimer ( this );

  // Save the mode.
  _lastMode = _viewer->getMode();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  // Stop and delete the timer.
  if ( 0x0 != _timer )
  {
    _timer->stop();
    delete _timer;
    _timer = 0x0;
  }

  // Clear the viewer.
  _viewer->clear();
  _viewer = 0x0;

  // Remove this window from the document's sets.
  this->document()->removeWindow ( this );

  // Tell the document this is closing.  
  // Make sure function is called after removeWindow is called.
  this->document()->closing ( this );

  // Clear the keys.
  _keys.clear();

  // Better be zero
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Document * Viewer::document()
{
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::Document * Viewer::document() const
{
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Render::Viewer* Viewer::viewer()
{
  return _viewer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Render::Viewer* Viewer::viewer() const
{
  return _viewer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Viewer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IOpenGLContext::IID:
    return static_cast < Usul::Interfaces::IOpenGLContext* > ( this );
  case Usul::Interfaces::IWindow::IID:
    return static_cast < Usul::Interfaces::IWindow* > ( this );
  case Usul::Interfaces::ITimeoutAnimate::IID:
    return static_cast < Usul::Interfaces::ITimeoutAnimate* > ( this );
  case Usul::Interfaces::ITimeoutSpin::IID:
    return static_cast < Usul::Interfaces::ITimeoutSpin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ref
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unref.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::unref ( bool )
{
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::paintEvent  ( QPaintEvent * event )
{
  _viewer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resizeEvent ( QResizeEvent * event )
{
  const QSize& size ( event->size() );
  _viewer->resize ( static_cast < unsigned int > ( size.width() ), static_cast < unsigned int > ( size.height() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Window now has focus.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::focusInEvent ( QFocusEvent * event )
{
  // Call the base class first.
  BaseClass::focusInEvent ( event );

  // Make our view current.
  Usul::Documents::Manager::instance ().activeView ( _viewer.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make current.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::makeCurrent()
{
  BaseClass::makeCurrent();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap buffers.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::swapBuffers()
{
  BaseClass::swapBuffers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the placement. This gives the nice cascading behavior.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_initPlacement ()
{
  if( QWidget* parent = dynamic_cast < QWidget* > ( this->parent() ) )
  {
    // Shortcuts.
    double percent ( 0.6 );
    int width  ( parent->width() );
    int height ( parent->height() );
    int w ( static_cast <int> ( percent * width ) );
    int h ( static_cast <int> ( percent * height ) );

    // Declare the placement static.
    static int x ( 0 );
    static int y ( 0 );

    // Set the placement.
    this->move    ( x, y );
    this->resize  ( w, h );

    // Increment.
    x += 20;
    y += 20;

    // If we are too far then start over.
    if ( ( x + w ) > width || ( y + h ) > height )
    {
      x = 0;
      y = 0;
    }

    this->adjustSize();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseMoveEvent ( QMouseEvent * event )
{
  bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

  // See if any mouses button are down.
  unsigned int mouse ( left || middle || right );

  // Set the event type.
  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::EventType type ( ( mouse ) ? EventAdapter::DRAG : EventAdapter::MOVE );

  // Handle the events. Make sure you pick before you drag.
  this->viewer()->handleNavigation ( x, y, left, middle, right, type );
  this->viewer()->handlePicking    ( x, y, false, 0 );
  this->viewer()->handleDragging   ( x, y, OsgTools::Draggers::Dragger::MOVE );

  // Handle tool.
  this->viewer()->handleTool ( left, middle, right, true, x, y, 0.0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mousePressEvent ( QMouseEvent * event )
{
  bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

  this->viewer()->buttonPress ( x, y, left, middle, right );

  // Does nothing unless in seek mode.
  this->viewer()->handleSeek ( x, y, left );

  // Handle the events. Make sure you pick before you drag.
  this->viewer()->handlePicking  ( x, y, left, 1 );
  this->viewer()->handleDragging ( x, y, OsgTools::Draggers::Dragger::START );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseReleaseEvent ( QMouseEvent * event )
{
  bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

  this->viewer()->buttonRelease ( x, y, left, middle, right );

  // Handle the events. Make sure you pick before you drag.
  this->viewer()->handlePicking  ( x, y, left, 1 );
  this->viewer()->handleDragging ( x, y, OsgTools::Draggers::Dragger::START );

  // Make sure.
  OsgTools::Draggers::Dragger::dragging ( 0x0, osg::Matrixd::identity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to toggle the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline void togglePolygonMode ( OsgTools::Render::Viewer &v, const Usul::Interfaces::IPolygonMode::Mode &mode )
  {
    if ( mode == v.polygonMode() )
    {
      v.polygonMode ( Usul::Interfaces::IPolygonMode::NONE );
    }
    else
    {
      v.polygonMode ( mode );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyPressEvent ( QKeyEvent * event )
{
  // Stop any spin.
  this->viewer()->spin ( false );

  if ( 0x0 == event )
    return;

  _keys[event->key()] = true;

  // Process the key.
  switch ( event->key() )
  {
  // See if it was the space-bar or the r-key...
  case Qt::Key_Space:
  case Qt::Key_R:
    // Move the camera.
    this->viewer()->camera ( OsgTools::Render::Viewer::RESET );
    break;

  // See if it was the f-key...
  case Qt::Key_F:
    // Move the camera.
    this->viewer()->camera ( OsgTools::Render::Viewer::FIT );
    break;

  // See if it was the right-arrow key...
  case Qt::Key_Right:

    // Move the camera.
    this->viewer()->camera ( OsgTools::Render::Viewer::ROTATE_Y_N90 );
    break;

  // See if it was the left-arrow key...
  case Qt::Key_Left:

    // Move the camera.
    this->viewer()->camera ( OsgTools::Render::Viewer::ROTATE_Y_P90 );
    break;

  // See if it was the up-arrow key...
  case Qt::Key_Up:

    // Move the camera.
    this->viewer()->camera ( OsgTools::Render::Viewer::ROTATE_X_P90 );
    break;

  // See if it was the down-arrow key...
  case Qt::Key_Down:

    // Move the camera.
    this->viewer()->camera ( OsgTools::Render::Viewer::ROTATE_X_N90 );
    break;

  // See if it was the h key...
  case Qt::Key_H:
    Helper::togglePolygonMode ( *(this->viewer()), Usul::Interfaces::IPolygonMode::HIDDEN_LINES );
    this->viewer()->render();
    break;

  // See if it was the w key...
  case Qt::Key_W:
    Helper::togglePolygonMode ( *(this->viewer()), Usul::Interfaces::IPolygonMode::WIRE_FRAME );
    this->viewer()->render();
    break;

  // See if it was the p key...
  case Qt::Key_P:
    Helper::togglePolygonMode ( *(this->viewer()), Usul::Interfaces::IPolygonMode::POINTS );
    this->viewer()->render();
    break;

  // See if it was the s key...
  case Qt::Key_S:
    _lastMode = this->viewer()->getMode();
    this->viewer()->setMode ( OsgTools::Render::Viewer::SEEK );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyReleaseEvent ( QKeyEvent * event )
{
  if ( 0x0 == event )
    return;

  _keys[event->key()] = false;

  // See if it was the s key...
  if ( Qt::Key_S == event->key() )
  {
    this->viewer()->setMode ( _lastMode );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this window the active window.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setFocus()
{
  BaseClass::setFocus();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the title.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setTitle ( const std::string& title )
{
  this->setWindowTitle ( title.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::startAnimation ( double milliseconds )
{
  if ( 0x0 != _timer )
  {
    _timer->stop();
    this->connect ( _timer, SIGNAL ( timeout() ), this, SLOT ( _onTimeoutAnimation() ) );
    _timer->start ( static_cast<int> ( milliseconds ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback that we are using for Animation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onTimeoutAnimation()
{
  if ( 0x0 != this->viewer() )
  {
    // Set the matrix and see if we should continue.
    if ( false == this->viewer()->timeoutAnimate() ) 
    {
      _timer->stop();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the timer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onTimeoutSpin()
{
  if ( 0x0 != this->viewer() )
  {
    this->viewer()->timeoutSpin();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the spin timeout.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stopSpin()
{
  if ( 0x0 != _timer )
  {
    _timer->stop();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the spin timeout.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::startSpin ( double milliseconds )
{
  if ( 0x0 != _timer )
  {
    _timer->stop();
    this->connect ( _timer, SIGNAL ( timeout() ), this, SLOT ( _onTimeoutSpin() ) );
    _timer->start ( static_cast<int> ( milliseconds ) );
  }
}
