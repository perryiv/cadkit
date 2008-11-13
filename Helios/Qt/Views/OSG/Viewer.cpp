
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/EditBackground.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/App/Application.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Commands/PolygonMode.h"
#include "Usul/Commands/RenderingPasses.h"
#include "Usul/Commands/RenderLoop.h"
#include "Usul/Commands/ShadeModel.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Make.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IContextMenuAdd.h"
#include "Usul/Interfaces/IKeyListener.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/DateTime.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"
#include "Usul/User/Directory.h"

#include "OsgTools/Render/Defaults.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

#include "QtTools/Color.h"
#include "QtTools/Menu.h"
#include "QtTools/Question.h"

#include "QtCore/QUrl"
#include "QtCore/QTimer"
#include "QtGui/QResizeEvent"
#include "QtGui/QDialog"
#include "QtGui/QHBoxLayout"
#include "QtGui/QVBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QSpinBox"
#include "QtGui/QLabel"
#include "QtGui/QFileDialog"
#include "QtGui/QMessageBox"

#include <ctime>
#include <limits>

using namespace CadKit::Helios::Views::OSG;
namespace Sections = Usul::Registry::Sections;
namespace Keys = Usul::Registry::Keys;
typedef Usul::Registry::Database Reg;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( Document *doc, const QGLFormat& format, QWidget* parent, IUnknown* caller ) :
  BaseClass ( format, parent ),
  _caller ( caller ),
  _document ( doc ),
  _viewer ( 0x0 ),
  _refCount ( 0 ),
  _timer ( 0x0 ),
  _isRenderLooping ( false ),
  _keys(),
  _lastMode ( OsgTools::Render::Viewer::NAVIGATION ),
  _threadId ( Usul::Threads::currentThreadId() ),
  _mutex ( new Viewer::Mutex ),
  _mouseWheelPosition ( 0 ),
  _mouseWheelSensitivity ( 10.0f )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // For convienence;
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Create the viewer.
  _viewer = new OsgTools::Render::Viewer ( 0x0, me, me );

  // Initialize openGL.
  this->initializeGL();

  // Create the viewer. This functions calls openGL functions.
  _viewer->create();

  // Set the document. Do this after create.
  _viewer->document ( doc );

  // Set the focus policy.
  this->setFocusPolicy ( Qt::ClickFocus );

  // Delete on close.
  this->setAttribute ( Qt::WA_DeleteOnClose );

  // Initalize the placement and size.
  this->_initPlacement();

  // Add this to the document.
  if ( 0x0 != doc )
    doc->addWindow ( this );

  // Initialize the timer.
  _timer = new QTimer ( this );

  // Save the mode.
  _lastMode = _viewer->getViewMode();

  // Add ourselves as a modified listener.
  Usul::Interfaces::IModifiedSubject::QueryPtr subject ( doc );
  if ( true == subject.valid() )
  {
    subject->addModifiedObserver ( this );
  }

  // We have a custom context menu.
  this->setContextMenuPolicy ( Qt::CustomContextMenu );

  // Notify us when a context menu is requested.
  QObject::connect ( this, SIGNAL ( customContextMenuRequested ( const QPoint& ) ), this,  SLOT ( _onContextMenuShow ( const QPoint& ) ) );

  // Enable drag 'n drop.
  this->setAcceptDrops ( true );

  // Update the cursor.
  this->updateCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  USUL_TRACE_SCOPE;

  // Stop and delete the timer.
  if ( 0x0 != _timer )
  {
    _timer->stop();
    delete _timer;
    _timer = 0x0;
  }

  // Clear the keys.
  _keys.clear();

  // Now delete the mutex.
  delete _mutex; _mutex = 0x0;

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
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::Document * Viewer::document() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Render::Viewer* Viewer::viewer()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _viewer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Render::Viewer* Viewer::viewer() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _viewer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Viewer::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

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
  case Usul::Interfaces::IModifiedObserver::IID:
    return static_cast < Usul::Interfaces::IModifiedObserver * > ( this );
  case Usul::Interfaces::IRenderLoop::IID:
    return static_cast < Usul::Interfaces::IRenderLoop * > ( this );
  case Usul::Interfaces::IRedraw::IID:
    return static_cast < Usul::Interfaces::IRedraw * > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::IQuestion::IID:
    return static_cast < Usul::Interfaces::IQuestion * > ( this );
  case Usul::Interfaces::ISaveFileDialog::IID:
    return static_cast < Usul::Interfaces::ISaveFileDialog * > ( this );
  case Usul::Interfaces::IToolBarAdd::IID:
    return static_cast < Usul::Interfaces::IToolBarAdd * > ( this );
  
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
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unref.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::unref ( bool )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::paintEvent ( QPaintEvent * event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( true == viewer.valid() )
    viewer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resizeEvent ( QResizeEvent * event )
{
  USUL_TRACE_SCOPE;
  
  BaseClass::resizeEvent ( event );
  
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( true == viewer.valid() )
  {
    const QSize& size ( event->size() );
    viewer->resize ( static_cast < unsigned int > ( size.width() ), static_cast < unsigned int > ( size.height() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Window now has focus.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::focusInEvent ( QFocusEvent * event )
{
  USUL_TRACE_SCOPE;

  // Call the base class first.
  BaseClass::focusInEvent ( event );

  // Make our view current.
  Usul::Documents::Manager::instance().activeView ( this->viewer() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Window has lost the focus.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::focusOutEvent ( QFocusEvent * event )
{
  USUL_TRACE_SCOPE;

  // Call the base class first.
  BaseClass::focusInEvent ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the current thread the thread we were created in.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::isContextThread() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return Usul::Threads::currentThreadId() == _threadId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make current.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::makeCurrent()
{
  USUL_TRACE_SCOPE;
  BaseClass::makeCurrent();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap buffers.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::swapBuffers()
{
  USUL_TRACE_SCOPE;
  BaseClass::swapBuffers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the placement. This gives the nice cascading behavior.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_initPlacement()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( QWidget* parent = dynamic_cast < QWidget* > ( this->parent() ) )
  {
    // Shortcuts.
    const double percent ( 0.8 );
    const int width  ( parent->width() );
    const int height ( parent->height() );
    const int w ( static_cast <int> ( percent * width ) );
    const int h ( static_cast <int> ( percent * height ) );

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
//  Preferred size.  Needed for cascading.
//
///////////////////////////////////////////////////////////////////////////////

QSize Viewer::sizeHint() const
{
  if ( QWidget* parent = dynamic_cast < QWidget* > ( this->parent() ) )
  {
    // Shortcuts.
    const double percent ( 0.8 );
    const int width  ( parent->width() );
    const int height ( parent->height() );
    const int w ( static_cast <int> ( percent * width ) );
    const int h ( static_cast <int> ( percent * height ) );
    
    return QSize ( w, h );
  }

  return QSize ( -1, -1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseMoveEvent ( QMouseEvent * event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // See if the mouse buttons are down. If the corresponding keys are 
  // down then we simulate a mouse-down state.
  const bool left   ( ( true == event->buttons().testFlag ( Qt::LeftButton  ) ) || ( true == _keys[Qt::Key_R] ) );
  const bool middle ( ( true == event->buttons().testFlag ( Qt::MidButton   ) ) || ( true == _keys[Qt::Key_T] ) );
  const bool right  ( ( true == event->buttons().testFlag ( Qt::RightButton ) ) || ( true == _keys[Qt::Key_Z] ) );

  const float x ( event->x() );
  const float y ( this->height() - event->y() );

  // See if any mouse buttons are down.
  bool mouse ( left || middle || right );

  // Set the event type.
  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::EventType type ( ( mouse ) ? EventAdapter::DRAG : EventAdapter::MOVE );
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, type ) );
  viewer->mouseMove ( ea.get() );
  this->updateCursor ( left, middle, right );

  // Resetting this every mouse-move makes it not not very effective.
  // Keeping this here as a reminder.
  //_mouseWheelPosition = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mousePressEvent ( QMouseEvent * event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // See if the mouse buttons are down. If the corresponding keys are 
  // down then we simulate a mouse-down state.
  const bool left   ( ( true == event->buttons().testFlag ( Qt::LeftButton  ) ) || ( true == _keys[Qt::Key_R] ) );
  const bool middle ( ( true == event->buttons().testFlag ( Qt::MidButton   ) ) || ( true == _keys[Qt::Key_T] ) );
  const bool right  ( ( true == event->buttons().testFlag ( Qt::RightButton ) ) || ( true == _keys[Qt::Key_Z] ) );

  const float x ( event->x() );
  const float y ( this->height() - event->y() );

  // Set the event type.
  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, EventAdapter::PUSH ) );
  viewer->buttonPress ( ea.get() );
  this->updateCursor ( left, middle, right );
  
  // Reset this.
  _mouseWheelPosition = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseReleaseEvent ( QMouseEvent * event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // See if the mouse buttons are down. If the corresponding keys are 
  // down then we simulate a mouse-down state.
  const bool left   ( ( true == event->buttons().testFlag ( Qt::LeftButton  ) ) || ( true == _keys[Qt::Key_R] ) );
  const bool middle ( ( true == event->buttons().testFlag ( Qt::MidButton   ) ) || ( true == _keys[Qt::Key_T] ) );
  const bool right  ( ( true == event->buttons().testFlag ( Qt::RightButton ) ) || ( true == _keys[Qt::Key_Z] ) );

  const float x ( event->x() );
  const float y ( this->height() - event->y() );

  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, EventAdapter::RELEASE ) );
  viewer->buttonRelease ( ea.get() );
  this->updateCursor ( left, middle, right );

  // Reset this.
  _mouseWheelPosition = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been double-clicked.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseDoubleClickEvent ( QMouseEvent * event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // Set and restore the seek state.
  OsgTools::Render::Viewer::ViewMode mode ( viewer->getViewMode() );
  viewer->setViewMode ( OsgTools::Render::Viewer::SEEK );
  Usul::Scope::Caller::RefPtr resetMode ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 
    ( mode, ( Usul::Adaptors::memberFunction ( viewer.get(), &OsgTools::Render::Viewer::setViewMode ) ) ) ) );

  // Simulate mouse button state.
  const bool left   ( true  );
  const bool middle ( false );
  const bool right  ( true  );

  const float x ( event->x() );
  const float y ( this->height() - event->y() );

  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, EventAdapter::PUSH ) );
  viewer->buttonPress ( ea.get() );

  // Reset this.
  _mouseWheelPosition = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse wheel has been moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::wheelEvent ( QWheelEvent * event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // Same as the right mouse moving.
  const bool left   ( false );
  const bool middle ( false );
  const bool right  ( true  );

  // Adjust the mouse wheel state.
  _mouseWheelPosition += event->delta();

  // Pretend the mouse moved in the y-direction.
  const float x ( event->x() );
  const float y ( static_cast < float > ( _mouseWheelPosition ) / _mouseWheelSensitivity );
  
  // Set the event type.
  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::EventType type ( EventAdapter::DRAG );
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, type ) );
  viewer->mouseMove ( ea.get() );
  this->updateCursor ( left, middle, right );
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
    USUL_TRACE_SCOPE_STATIC;
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

void Viewer::keyPressEvent ( QKeyEvent *event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // Stop any spin.
  viewer->spin ( false );

  if ( 0x0 == event )
    return;

  _keys[event->key()] = true;

  Usul::Interfaces::IKeyListener::QueryPtr kl ( this->document() );
  bool handled ( kl.valid() ? kl->keyPressed( event->key() ) : false );
  if ( false == handled )
  {
    // Process the key.
    switch ( event->key() )
    {
    // See if it was the space-bar...
    case Qt::Key_Space:

      // Move the camera.
      viewer->camera ( OsgTools::Render::Viewer::RESET );
      break;

    // See if it was the f-key...
    case Qt::Key_F:

      // Move the camera.
      viewer->camera ( OsgTools::Render::Viewer::FIT );
      break;

    // See if it was the right-arrow key...
    case Qt::Key_Right:

      // Move the camera.
      viewer->camera ( OsgTools::Render::Viewer::ROTATE_Y_N45 );
      break;

    // See if it was the left-arrow key...
    case Qt::Key_Left:

      // Move the camera.
      viewer->camera ( OsgTools::Render::Viewer::ROTATE_Y_P45 );
      break;

    // See if it was the up-arrow key...
    case Qt::Key_Up:

      // Move the camera.
      viewer->camera ( OsgTools::Render::Viewer::ROTATE_X_P45 );
      break;

    // See if it was the down-arrow key...
    case Qt::Key_Down:

      // Move the camera.
      viewer->camera ( OsgTools::Render::Viewer::ROTATE_X_N45 );
      break;

    // See if it was the h key...
    case Qt::Key_H:

      Helper::togglePolygonMode ( *(viewer), Usul::Interfaces::IPolygonMode::HIDDEN_LINES );
      viewer->render();
      break;

    // See if it was the w key...
    case Qt::Key_W:

      Helper::togglePolygonMode ( *(viewer), Usul::Interfaces::IPolygonMode::WIRE_FRAME );
      viewer->render();
      break;

    // See if it was the p key...
    case Qt::Key_P:

      Helper::togglePolygonMode ( *(viewer), Usul::Interfaces::IPolygonMode::POINTS );
      viewer->render();
      break;

    // See if it was the s key...
    case Qt::Key_S:

      _lastMode = viewer->getViewMode();
      viewer->setViewMode ( OsgTools::Render::Viewer::SEEK );
      break;

    case Qt::Key_Escape:

      //this->showNormal();
      viewer->cycleMode();
      break;

    case Qt::Key_F11:
      {
        if ( true == Usul::Bits::has ( this->windowState(), Qt::WindowFullScreen ) )
        {
          this->showNormal();
        }
        else
        {
          this->showFullScreen();
        }
      }
      break;
    }
  }
  // Update the cursor.
  this->updateCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyReleaseEvent ( QKeyEvent * event )
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  _keys[event->key()] = false;

  // See if it was the s key...
  if ( Qt::Key_S == event->key() )
  {
    viewer->setViewMode ( _lastMode );
  }

  // Update the cursor.
  this->updateCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this window the active window.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setFocus()
{
  USUL_TRACE_SCOPE;
  BaseClass::setFocus();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the title.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setTitle ( const std::string& title )
{
  USUL_TRACE_SCOPE;
  this->setWindowTitle ( title.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::startAnimation ( double milliseconds )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

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
  USUL_TRACE_SCOPE;

  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // Set the matrix and see if we should continue.
  if ( false == viewer->timeoutAnimate() ) 
  {
    Guard guard ( this );
    _timer->stop();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the timer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onTimeoutSpin()
{
  USUL_TRACE_SCOPE;

  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  viewer->timeoutSpin();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the spin timeout.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stopSpin()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

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
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _timer )
  {
    _timer->stop();
    this->connect ( _timer, SIGNAL ( timeout() ), this, SLOT ( _onTimeoutSpin() ) );
    _timer->start ( static_cast<int> ( milliseconds ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the document is modified (Usul::Interfaces::IModifiedObserver).
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::subjectModified ( Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Queue a repaint. Do not change this behavior unless you also change the 
  // fact that IDocument's requestRedraw() uses the modified-observers to 
  // implement the request, which results in this function being called.
  this->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Mutex &Viewer::mutex() const
{
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set render loop flag.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::renderLoop ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Do nothing if it's already the same.
  if ( state == this->renderLoop() )
    return;

  // Set the flag.
  _isRenderLooping = state;

  // If we're supposed to start...
  if ( true == state )
  {
    // Fire a single event.
    this->_oneRenderLoopTimeout();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fire one render-loop timeout.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_oneRenderLoopTimeout()
{
  USUL_TRACE_SCOPE;
  QTimer::singleShot ( 100, this, SLOT ( _onTimeoutRenderLoop() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get render loop flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::renderLoop() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _isRenderLooping;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the viewer to render.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onTimeoutRenderLoop()
{
  USUL_TRACE_SCOPE;

  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( true == viewer.valid() )
  {
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( viewer, &OsgTools::Render::Viewer::render ), "1600949648" );
  }

  // Do it again if we should.
  if ( true == this->renderLoop() )
  {
    this->_oneRenderLoopTimeout();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::menuAdd( MenuKit::Menu &menu, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  MenuKit::Menu::RefPtr view ( menu.find ( "&View", true ) );

  if ( view.valid() )
  {
    this->_menuAdd ( *view );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_menuAdd( MenuKit::Menu &menu, Usul::Interfaces::IUnknown * caller )
{
  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  
  // Return now if no viewer.
  if ( false == viewer.valid() )
    return;
  
  // Add a separator if the menu already has items.
  if ( menu.items().size() > 0 )
    menu.addSeparator();

  Usul::Interfaces::IUnknown::QueryPtr unknown ( this->viewer() );

  // Typedefs.
  typedef MenuKit::Button Button;
  typedef MenuKit::RadioButton RadioButton;
  typedef MenuKit::ToggleButton ToggleButton;
  typedef Usul::Commands::RenderingPasses RenderingPasses;
  typedef Usul::Commands::PolygonMode PolygonMode;
  typedef Usul::Interfaces::IPolygonMode IPolygonMode;
  typedef Usul::Commands::ShadeModel ShadeModel;
  typedef Usul::Interfaces::IShadeModel IShadeModel;
  typedef OsgTools::Render::Viewer OsgViewer;

  namespace UC = Usul::Commands;
  namespace UA = Usul::Adaptors;

  // Background menu.
  {
    MenuKit::Menu::RefPtr background ( new MenuKit::Menu ( "&Background" ) );
    background->append ( new Button ( USUL_MAKE_COMMAND ( "&Edit...", "", this, &Viewer::editBackground ) ) );
    background->append ( new Button ( USUL_MAKE_COMMAND ( "&Default", "", viewer.get(), &OsgViewer::defaultBackground ) ) );
    menu.append ( background.get() );
  }
  
  // Clip distances.
  {
    MenuKit::Menu::RefPtr clip ( new MenuKit::Menu ( "&Clip Distances" ) );
    clip->append ( new ToggleButton ( UC::genericToggleCommand ( 
                                                                "&Auto", 
                                                                UA::memberFunction<void> ( viewer.get(), &OsgViewer::computeNearFarSet ), 
                                                                UA::memberFunction<bool> ( viewer.get(), &OsgViewer::computeNearFarGet ) ) ));
    clip->append ( new Button ( USUL_MAKE_COMMAND ( "&Edit...", "", this, &Viewer::editClippingDistances ) ) );
    menu.append ( clip.get() );
  }
  
  // Camera menu.
  {
    MenuKit::Menu::RefPtr gotoMenu ( new MenuKit::Menu ( "&Camera" ) );
    
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "&Fit",    "eye.png",    viewer.get(), &OsgViewer::camera, OsgViewer::FIT   ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "&Home",   "home.png",   viewer.get(), &OsgViewer::camera, OsgViewer::RESET ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "&Front",  "Front.png",  viewer.get(), &OsgViewer::camera, OsgViewer::FRONT ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "&Back",   "Back.png",   viewer.get(), &OsgViewer::camera, OsgViewer::BACK ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "&Left",   "Left.png",   viewer.get(), &OsgViewer::camera, OsgViewer::LEFT ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "&Right",  "Right.png",  viewer.get(), &OsgViewer::camera, OsgViewer::RIGHT ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "&Top",    "Top.png",    viewer.get(), &OsgViewer::camera, OsgViewer::TOP ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "B&ottom", "Bottom.png", viewer.get(), &OsgViewer::camera, OsgViewer::BOTTOM ) ) );
    gotoMenu->addSeparator();
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Rotate Y +45", "", viewer.get(), &OsgViewer::camera, OsgViewer::ROTATE_Y_P45 ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Rotate Y -45", "", viewer.get(), &OsgViewer::camera, OsgViewer::ROTATE_Y_N45 ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Rotate X +45", "", viewer.get(), &OsgViewer::camera, OsgViewer::ROTATE_X_N45 ) ) );
    gotoMenu->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Rotate X -45", "", viewer.get(), &OsgViewer::camera, OsgViewer::ROTATE_X_N45 ) ) );

    menu.append ( gotoMenu.get() );
  }

  // Mode menu.
  {
    MenuKit::Menu::RefPtr modes ( new MenuKit::Menu ( "&Modes" ) );
    modes->append ( new RadioButton ( UC::genericCheckCommand ( "&Navigate", 
                                                                           UA::bind1<void> ( OsgViewer::NAVIGATION, UA::memberFunction<void> ( viewer.get(), &OsgViewer::setViewMode ) ), 
                                                                           UA::bind1<bool> ( OsgViewer::NAVIGATION, UA::memberFunction<bool> ( viewer.get(), &OsgViewer::isViewModeCurrent ) ) ) ) );
    modes->append ( new RadioButton ( UC::genericCheckCommand ( "&Pick", 
                                                                           UA::bind1<void> ( OsgViewer::PICK, UA::memberFunction<void> ( viewer.get(), &OsgViewer::setViewMode ) ), 
                                                                           UA::bind1<bool> ( OsgViewer::PICK, UA::memberFunction<bool> ( viewer.get(), &OsgViewer::isViewModeCurrent ) ) ) ) );
    modes->append ( new RadioButton ( UC::genericCheckCommand ( "&Seek", 
                                                                           UA::bind1<void> ( OsgViewer::SEEK, UA::memberFunction<void> ( viewer.get(), &OsgViewer::setViewMode ) ), 
                                                                           UA::bind1<bool> ( OsgViewer::SEEK, UA::memberFunction<bool> ( viewer.get(), &OsgViewer::isViewModeCurrent ) ) ) ) );
    menu.append ( modes.get() );
  }
  
  // Rendering passes menu.
  {
    MenuKit::Menu::RefPtr passes ( new MenuKit::Menu ( "&Rendering Passes" ) );
    menu.append ( passes.get() );

    passes->append ( new RadioButton ( new RenderingPasses ( "&1",   1, unknown.get() ) ) );
    passes->append ( new RadioButton ( new RenderingPasses ( "&3",   3, unknown.get() ) ) );
    passes->append ( new RadioButton ( new RenderingPasses ( "&9",   9, unknown.get() ) ) );
    passes->append ( new RadioButton ( new RenderingPasses ( "1&2", 12, unknown.get() ) ) );
  }
  
#ifdef __APPLE__
  menu.append ( new ToggleButton ( UC::genericToggleCommand ( 
                                                            "&Mutli-Sample", 
                                                             UA::memberFunction<void> ( viewer.get(), &OsgViewer::useMultisampleSet ), 
                                                             UA::memberFunction<bool> ( viewer.get(), &OsgViewer::useMultisampleGet ) ) ) );
#endif

  MenuKit::Button::RefPtr rl ( new MenuKit::ToggleButton ( new UC::RenderLoop ( "Render &Loop", unknown.get() ) ) );
  menu.append ( rl );

  // Polygons menu.
  {
    MenuKit::Menu::RefPtr polygons ( new MenuKit::Menu ( "&Polygons" ) );
    menu.append ( polygons.get() );

    polygons->append ( new RadioButton ( new PolygonMode ( "&Filled",       IPolygonMode::FILLED, unknown.get() ) ) );
    polygons->append ( new RadioButton ( new PolygonMode ( "&Hidden Lines", IPolygonMode::HIDDEN_LINES, unknown.get() ) ) );
    polygons->append ( new RadioButton ( new PolygonMode ( "&Wireframe",    IPolygonMode::WIRE_FRAME, unknown.get() ) ) );
    polygons->append ( new RadioButton ( new PolygonMode ( "&Points",       IPolygonMode::POINTS, unknown.get() ) ) );
  }

  // Shading menu.
  {
    MenuKit::Menu::RefPtr shading ( new MenuKit::Menu ( "S&hading" ) );
    menu.append ( shading.get() );

    shading->append ( new RadioButton ( new ShadeModel ( "&Smooth", IShadeModel::SMOOTH, unknown.get() ) ) );
    shading->append ( new RadioButton ( new ShadeModel ( "&Flat",   IShadeModel::FLAT, unknown.get() ) ) );
  }
  
  // Lods menu.
  {
    MenuKit::Menu::RefPtr lods ( new MenuKit::Menu ( "&LODs" ) );
    menu.append ( lods.get() );
    
    lods->append ( new ToggleButton ( 
                          UC::genericToggleCommand ( 
                                                    "&Use Low LOD When Navigating", 
                                                    UA::memberFunction<void> ( viewer.get(), &OsgViewer::useLowLodsSet ), 
                                                    UA::memberFunction<bool> ( viewer.get(), &OsgViewer::useLowLodsGet ) ) ) );
    lods->append ( new ToggleButton ( 
                          UC::genericToggleCommand ( 
                                                    "&Always Use High LOD", 
                                                    UA::memberFunction<void> ( viewer.get(), &OsgViewer::useHighLodsSet ), 
                                                    UA::memberFunction<bool> ( viewer.get(), &OsgViewer::useHighLodsGet ) ) ) );
  }

  // Add common window sizes.
  // From http://en.wikipedia.org/wiki/Image:Standard_video_res.svg
  typedef Usul::Math::Vec2ui Size;
  typedef std::vector < Size > Sizes;
  Sizes sizes;
  sizes.push_back ( Size ( 640, 480 ) );
  sizes.push_back ( Size ( 720, 480 ) );
  sizes.push_back ( Size ( 768, 576 ) );
  sizes.push_back ( Size ( 1024, 768 ) );
  sizes.push_back ( Size ( 1280, 720 ) );
  sizes.push_back ( Size ( 1920, 1080 ) );
  sizes.push_back ( Size ( 4200, 1050 ) );

  // Make the menu of common sizes.
  MenuKit::Menu::RefPtr size ( new MenuKit::Menu ( "&Size" ) );
  for ( Sizes::const_iterator iter = sizes.begin(); iter != sizes.end(); ++iter )
  {
    Size s ( *iter );
    size->append ( new RadioButton ( UC::genericCheckCommand ( Usul::Strings::format ( s[0], " x ", s[1] ), 
      UA::bind2<void> ( s[0], s[1], UA::memberFunction<void> ( this, &Viewer::_resize ) ), 
      UA::bind2<bool> ( s[0], s[1], UA::memberFunction<bool> ( this, &Viewer::_isSize ) ) ) ) );
  }
  size->append ( new Button ( UC::genericCommand ( "Custom...", 
      UA::memberFunction<void> ( this, &Viewer::_customSize ), 
      UC::TrueFunctor() ) ) );

  menu.append ( size );

  // Frame dump button.
  //menu.append ( new ToggleButton ( UC::genericToggleCommand ( "Frame &Dump", UA::memberFunction<void> ( this, &Viewer::_frameDump ), UA::memberFunction<bool> ( this, &Viewer::_isFrameDump ) ) ) );
  // Frame-dump menu.
  {
    MenuKit::Menu::RefPtr dump ( new MenuKit::Menu ( "Frame &Dump" ) );
    dump->append ( new RadioButton ( UC::genericCheckCommand ( "&Never", 
                                                              UA::bind1<void> ( IFrameDump::NEVER_DUMP, UA::memberFunction<void> ( this, &Viewer::_frameDumpStateSet ) ), 
                                                               UA::bind1<bool> ( IFrameDump::NEVER_DUMP, UA::memberFunction<bool> ( this, &Viewer::_isFrameDumpState ) ) ) ) );
    dump->append ( new RadioButton ( UC::genericCheckCommand ( "If Document &Idle", 
                                                               UA::bind1<void> ( IFrameDump::IF_DOCUMENT_NOT_BUSY, UA::memberFunction<void> ( this, &Viewer::_frameDumpStateSet ) ), 
                                                               UA::bind1<bool> ( IFrameDump::IF_DOCUMENT_NOT_BUSY, UA::memberFunction<bool> ( this, &Viewer::_isFrameDumpState ) ) ) ) );
    dump->append ( new RadioButton ( UC::genericCheckCommand ( "&Always", 
                                                               UA::bind1<void> ( IFrameDump::ALWAYS_DUMP, UA::memberFunction<void> ( this, &Viewer::_frameDumpStateSet ) ), 
                                                               UA::bind1<bool> ( IFrameDump::ALWAYS_DUMP, UA::memberFunction<bool> ( this, &Viewer::_isFrameDumpState ) ) ) ) );
    menu.append ( dump.get() );
  }

  // Axes button.
  menu.append ( new ToggleButton ( UC::genericToggleCommand ( "Show &Axes", UA::memberFunction<void> ( _viewer.get(), &OsgViewer::axesShown ), UA::memberFunction<bool> ( viewer.get(), &OsgViewer::isAxesShown ) ) ) );

  // On-screen text button.
  menu.append ( new ToggleButton ( UC::genericToggleCommand ( "Show &Text", UA::memberFunction<void> ( _viewer.get(), &OsgViewer::textShown ), UA::memberFunction<bool> ( viewer.get(), &OsgViewer::isTextShown ) ) ) );
  
  // Back face culling.
  menu.append ( new ToggleButton ( UC::genericToggleCommand ( "Show Back &Faces", UA::memberFunction<void> ( _viewer.get(), &OsgViewer::showBackFaces ), UA::memberFunction<bool> ( viewer.get(), &OsgViewer::isBackFacesShowing ) ) ) );
  
  // Two Sided Lighting.
  menu.append ( new ToggleButton ( UC::genericToggleCommand ( "Two Sided &Lighting", UA::memberFunction<void> ( _viewer.get(), &OsgViewer::twoSidedLightingSet ), UA::memberFunction<bool> ( viewer.get(), &OsgViewer::twoSidedLightingGet ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the background.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::editBackground()
{
  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );

  // Make sure it's valid.
  if ( false == viewer.valid() )
    return;

  // Useful typedef.
  typedef OsgTools::Render::Viewer::Corners Corners;

  // The edit background dialog.
  EditBackground dialog ( viewer.get(), this );

  // Save current corner configuration.
  unsigned int corners ( viewer->backgroundCorners() );

  // Save current colors.
  viewer->backgroundCorners  ( Corners::TOP_LEFT );
  osg::Vec4 topLeftColor     ( viewer->backgroundColor() );
  viewer->backgroundCorners  ( Corners::TOP_RIGHT );
  osg::Vec4 topRightColor    ( viewer->backgroundColor() );
  viewer->backgroundCorners  ( Corners::BOTTOM_LEFT );
  osg::Vec4 bottomLeftColor  ( viewer->backgroundColor() );
  viewer->backgroundCorners  ( Corners::BOTTOM_RIGHT );
  osg::Vec4 bottomRightColor ( viewer->backgroundColor() );

  // Set the corner value before showing dialog.
  viewer->backgroundCorners ( corners );

  // Restore colors if canceled.
  if ( QDialog::Rejected == dialog.exec() )
  {
    // Top left corner.
    viewer->backgroundCorners ( Corners::TOP_LEFT );
    viewer->backgroundColor ( topLeftColor );

    // Top right corner.
    viewer->backgroundCorners ( Corners::TOP_RIGHT );
    viewer->backgroundColor ( topRightColor );

    // Bottom left corner.
    viewer->backgroundCorners ( Corners::BOTTOM_LEFT );
    viewer->backgroundColor ( bottomLeftColor );

    // Bottom right corner.
    viewer->backgroundCorners ( Corners::BOTTOM_RIGHT );
    viewer->backgroundColor ( bottomRightColor );

    // Set corners state.
    viewer->backgroundCorners ( corners );
  }

  // Request paint.
  this->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dragging has entering window.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::dragEnterEvent ( QDragEnterEvent *event )
{
  USUL_TRACE_SCOPE;
  
  Document::RefPtr document ( this->document() );
  
  if ( false == document.valid() || 0x0 == event )
    return;

  typedef QList < QUrl > Urls;
  typedef Urls::const_iterator ConstIterator;

  Urls urls ( event->mimeData()->urls() );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile().toStdString() );

    if ( document->canInsert ( file ) )
    {
      event->acceptProposedAction();
      return;
    }
  }

  event->accept();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Files have been dropped.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::dropEvent ( QDropEvent *event )
{
  USUL_TRACE_SCOPE;

  Document::RefPtr document ( this->document() );

  if ( false == document.valid() || 0x0 == event )
    return;

  typedef QList < QUrl > Urls;
  typedef Urls::const_iterator ConstIterator;

  Urls urls ( event->mimeData()->urls() );

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile().toStdString() );

    document->read ( file, me.get() );
  }

  event->acceptProposedAction();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_resize ( unsigned int w, unsigned int h )
{
  this->resize ( static_cast < int > ( w ), static_cast < int > ( h ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the windows current size.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_isSize ( unsigned int w, unsigned int h ) const
{
  QSize size ( this->size() );
  return static_cast < int > ( w ) == size.width() && static_cast < int > ( h ) == size.height();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enter a custom size.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_customSize()
{
  QDialog dialog;
  dialog.setWindowTitle ( "Enter Custom Size" );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );

  QSpinBox *x ( new QSpinBox );
  QSpinBox *y ( new QSpinBox );

  x->setMaximum ( 4096 );
  y->setMaximum ( 4096 );

  const QSize size ( this->size() );

  x->setValue ( size.width() );
  y->setValue ( size.height() );

  {
    QHBoxLayout *layout ( new QHBoxLayout );
    layout->addWidget ( new QLabel ( "X:" ) );
    layout->addWidget ( x );
    layout->addWidget ( new QLabel ( "Y:" ) );
    layout->addWidget ( y );
    topLayout->addLayout ( layout );
  }

  {
    QPushButton *ok ( new QPushButton ( "Ok" ) );
    QPushButton *cancel ( new QPushButton ( "Cancel" ) );

    connect ( ok, SIGNAL ( clicked() ), &dialog, SLOT ( accept() ) );
    connect ( cancel, SIGNAL ( clicked() ), &dialog, SLOT ( reject() ) );

    QHBoxLayout *layout ( new QHBoxLayout );
    layout->addStretch();
    layout->addWidget( ok );
    layout->addWidget( cancel );
    topLayout->addLayout ( layout );
  }

  if ( QDialog::Accepted == dialog.exec() )
  {
    const int xValue ( x->value() );
    const int yValue ( y->value() );

    this->resize ( xValue, yValue );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame dump state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_frameDumpStateSet ( Usul::Interfaces::IFrameDump::DumpState state )
{
  USUL_TRACE_SCOPE;
  
  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );

  // Make sure it's valid.
  if ( false == viewer.valid() )
    return;

  // Get needed interface.
  Usul::Interfaces::IFrameDump::QueryPtr frameDump ( viewer );
  if ( false == frameDump.valid() )
    return;

  // Always turn it off first.
  frameDump->setFrameDumpState ( IFrameDump::NEVER_DUMP );

  // Are we turning it off?
  if ( IFrameDump::NEVER_DUMP == state )
    return;
  
  // Set the properties.
  if ( false == this->_frameDumpProperties() )
    return;

  // Set the frame dump state.
  frameDump->setFrameDumpState ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame dump state.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IFrameDump::DumpState Viewer::_frameDumpStateGet() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _viewer.valid() ? _viewer->frameDump().dump() : IFrameDump::NEVER_DUMP );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the frame-dump state?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_isFrameDumpState ( IFrameDump::DumpState state ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( _viewer.valid() ) ? state == _viewer->frameDump().dump() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show a dialog for frame dump properties.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_frameDumpProperties()
{
  // Get the directory from the registry.
  const std::string defaultFrameDumpDir ( Usul::User::Directory::documents ( true, false ) );
  std::string directory ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::FRAME_DUMP_DIRECTORY].get ( defaultFrameDumpDir ) );

  // Confirm that this is the correct directory.
  QString dir ( QFileDialog::getExistingDirectory ( this, "Select a directory", directory.c_str() ) );
  if ( 0 == dir.size() )
    return false;

  // Set the directory in the registry.
  directory = dir.toStdString();
  Reg::instance()[Sections::VIEWER_SETTINGS][Keys::FRAME_DUMP_DIRECTORY] = directory;

  // Get the current time.
  ::tm time ( Usul::System::DateTime::local() );

  // Convert it to a string.
  const unsigned int size ( 1024 );
  char buffer[size];
  ::memset ( buffer, '\0', size );
  ::strftime ( buffer, size - 1, "%Y_%m_%d_%H_%M_%S", &time );

  directory += std::string ( "/" ) + std::string ( buffer ) + std::string ( "/" );

  // Make the directory.
  Usul::File::make ( directory );

  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );

  // Make sure it's valid.
  if ( false == viewer.valid() )
    return false;

  // Set the frame dump properties.
  viewer->frameDump().dir ( directory );
  viewer->frameDump().base( "" );
  viewer->frameDump().digits ( 9 ); // 9 works better with ffmpeg.
  viewer->frameDump().ext ( ".jpg" );

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IRedraw::redraw()
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::redraw()
{
  Usul::Interfaces::IRedraw::QueryPtr draw ( this->viewer() );
  if ( true == draw.valid() )
    draw->redraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IRedraw::setStatsDisplay()
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setStatsDisplay ( bool b )
{
  Usul::Interfaces::IRedraw::QueryPtr draw ( this->viewer() );
  if ( true == draw.valid() )
    draw->setStatsDisplay ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the context menu.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onContextMenuShow ( const QPoint& pos )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );

  // Return if no viewer, or if we aren't in the proper mode.
  if ( false == viewer.valid() || false == viewer->picking() )
    return;

  // Make the menu.
  MenuKit::Menu::RefPtr menu ( new MenuKit::Menu );
  
  // Ask the document if it wants to add to the context menu.
  Usul::Interfaces::IContextMenuAdd::QueryPtr cma ( this->document() );
  if ( cma.valid() )
    cma->contextMenuAdd ( *menu, Usul::Math::Vec2ui ( pos.x(), pos.y() ), Usul::Interfaces::IUnknown::QueryPtr ( viewer ) );
  
  // Add our buttons to the menu.
  this->_menuAdd ( *menu );
  
  typedef MenuKit::Button Button;
  menu->append ( new Button ( USUL_MAKE_COMMAND ( "OpenGL information", "", this, &Viewer::_openGLInformation ) ) );

  QtTools::Menu qMenu;
  qMenu.menu ( menu.get() );
  qMenu.exec ( this->mapToGlobal ( pos ) );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Close event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::closeEvent ( QCloseEvent *event )
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Viewer::_closeEvent ), event, "2663774419" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure we can close.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_closeEvent ( QCloseEvent* event )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Get the document.
  Document::RefPtr doc ( this->document() );

  // Ask the document if we can close.
  if ( true == doc.valid() && false == doc->canClose ( this, Usul::Interfaces::IUnknown::QueryPtr ( this ) ) )
  {
    event->ignore();
    return;
  }

  // Close.  Removing this from the document's windows.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Viewer::_close ), "1749695082" );

  // Pass along to the base class.
  BaseClass::closeEvent ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prompt the user (IQuestion).
//
///////////////////////////////////////////////////////////////////////////////

std::string Viewer::question ( const std::string &buttons, const std::string &title, const std::string &text )
{
  return QtTools::question ( this, buttons, title, text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the document that we are closing.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_close()
{
  // Stop the timers.
  this->renderLoop ( false );
  this->stopSpin();

  // Clean up the viewer first.
  {
    Guard guard ( this->mutex() );
  
    if ( _viewer.valid() )
    {
      
      // Save viewer's state first.
      _viewer->stateSave();

      // Clear the viewer.
      _viewer->clear();
    }

    _viewer = 0x0;
  }

  // Get the document.
  Viewer::Document::RefPtr document ( this->document() );

  // Remove ourselves as a modified listener.
  Usul::Interfaces::IModifiedSubject::QueryPtr subject ( document );
  if ( true == subject.valid() )
  {
    subject->removeModifiedObserver ( this );
  }

  // If the document is valid...
  if ( true == document.valid() )
  {
    // Remove this window from the document's sets.
    document->removeWindow ( this );

    // Tell the document this is closing.  
    // Make sure removeWindow is called called first.

    // Ask document if it has a job to close it.
    Usul::Jobs::Job::RefPtr job ( document->closeJob() );
    if ( true == job.valid() )
    {
      Usul::Jobs::Manager::instance().addJob ( job );
    }

    // No job, just close here.
    else
    {
      document->closing ( this );
    }
  }

  // Clear the document.
  {
    Guard guard ( this );
    _document = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the caller.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::IUnknown * Viewer::caller()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _caller.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the caller.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::IUnknown * Viewer::caller() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _caller.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the file to save to (ISaveFileDialog).
//
///////////////////////////////////////////////////////////////////////////////

Viewer::FileResult Viewer::getSaveFileName  ( const std::string &title, const Filters &filters )
{
  USUL_TRACE_SCOPE;
  Usul::Interfaces::ISaveFileDialog::QueryPtr dialog ( this->caller() );
  return ( dialog.valid() ? dialog->getSaveFileName( title, filters ) : FileResult () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Force the window closed.  Will not prompt.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::forceClose()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Close.  Removing this from the document's windows.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Viewer::_close ), "3614568329" );
  this->close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cursor.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateCursor()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );

  // Make sure it's valid.
  if ( false == viewer.valid() )
    return;

  const ViewMode mode ( viewer->getViewMode() );

  if ( OsgTools::Render::Viewer::NAVIGATION == mode )
    this->setCursor ( Qt::OpenHandCursor );
  else if ( OsgTools::Render::Viewer::PICK == mode )
    this->setCursor ( Qt::ArrowCursor );
  else if ( OsgTools::Render::Viewer::SEEK == mode )
    this->setCursor ( Qt::CrossCursor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cursor.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateCursor( bool left, bool middle, bool right )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  const bool buttons ( left || middle || right );

  // Call the other one if no buttons pressed.
  if ( false == buttons )
    this->updateCursor();
  else
  {
    // Get the viewer.
    OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );

    // Make sure it's valid.
    if ( false == viewer.valid() )
      return;

    const ViewMode mode ( viewer->getViewMode() );

    if ( OsgTools::Render::Viewer::NAVIGATION == mode )
      this->setCursor ( Qt::ClosedHandCursor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  View OpenGL information.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_openGLInformation()
{
  // Format the string.
  std::ostringstream message;
  
  this->makeCurrent();
  message << "OpenGL information:" << std::endl;
  const char *vendor     ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_VENDOR     ) ) );
  const char *renderer   ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_RENDERER   ) ) );
  const char *version    ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_VERSION    ) ) );
  
  if ( vendor )
    message << "\n  -- Vendor: " << vendor;
  if ( renderer )
    message << "\n  -- Renderer: " << renderer;
  if ( version )
    message << "\n  -- Version: " << version;
  
  // This is useful info, but makes the dialog to big to close.
#if 0
  const char *extensions ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_EXTENSIONS ) ) );
  
  if ( extensions )
  {
    std::string e ( extensions );
    std::replace ( e.begin(), e.end(), ' ', '\n' );
    message << "\n  -- Extensions: " << e;
  }
#endif
  
  message << "\nContext information:" << std::endl;
  QGLFormat format ( this->format() );
  message << "Depth buffer: " << ( format.depth() ? Usul::Strings::format ( format.depthBufferSize(), " bits" ) : "Disabled" ) << std::endl;
  message << "Accumulation buffer: " << ( format.accum() ? Usul::Strings::format ( format.accumBufferSize(), " bits" ) : "Disabled" ) << std::endl;
  message << "Stencil buffer: " << ( format.stencil() ? Usul::Strings::format ( format.stencilBufferSize(), " bits" ) : "Disabled" ) << std::endl;
  message << "Mutlisample buffer: " << ( format.sampleBuffers() ? Usul::Strings::format ( format.samples(), " samples" ) : "Disabled" ) << std::endl;
  message << "Stereo: " << ( format.stereo() ? "Enabled" : "Disabled" ) << std::endl;
  
  QMessageBox::information ( this, "OpenGL information", message.str().c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the toolbar.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::toolBarAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller )
{
  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  
  // Return now if no viewer.
  if ( false == viewer.valid() )
    return;
  
  Usul::Interfaces::IUnknown::QueryPtr unknown ( this->viewer() );
  
  // Typedefs.
  typedef MenuKit::Button Button;
  typedef OsgTools::Render::Viewer OsgViewer;
  
  // Camera menu.
  MenuKit::Menu::RefPtr toolBar ( new MenuKit::Menu ( "view_toolbar" ) );

  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Fit",    "eye.png",    viewer.get(), &OsgViewer::camera, OsgViewer::FIT   ) ) );
  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Home",   "home.png",   viewer.get(), &OsgViewer::camera, OsgViewer::RESET ) ) );
  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Front",  "Front.png",  viewer.get(), &OsgViewer::camera, OsgViewer::FRONT ) ) );
  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Back",   "Back.png",   viewer.get(), &OsgViewer::camera, OsgViewer::BACK ) ) );
  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Left",   "Left.png",   viewer.get(), &OsgViewer::camera, OsgViewer::LEFT ) ) );
  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Right",  "Right.png",  viewer.get(), &OsgViewer::camera, OsgViewer::RIGHT ) ) );
  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Top",    "Top.png",    viewer.get(), &OsgViewer::camera, OsgViewer::TOP ) ) );
  toolBar->append ( new Button ( USUL_MAKE_COMMAND_ARG0 ( "Bottom", "Bottom.png", viewer.get(), &OsgViewer::camera, OsgViewer::BOTTOM ) ) );
  
  menu.append ( toolBar.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the saved state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stateLoad()
{
  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  
  if ( true == viewer.valid() )
  {
    // Load state.
    viewer->stateLoad();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the clipping distances.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::editClippingDistances()
{
  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  
  // Return if we don't have a viewer.
  if ( false == viewer.valid() )
    return;
  
  QDialog dialog;
  dialog.setWindowTitle ( "Enter Custom Clipping Distances" );
  
  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );
  
  QDoubleSpinBox *zNear ( new QDoubleSpinBox );
  QDoubleSpinBox *zFar ( new QDoubleSpinBox );
  
  zNear->setRange ( std::numeric_limits<double>::min(), std::numeric_limits<double>::max() );
  zFar->setRange  ( std::numeric_limits<double>::min(), std::numeric_limits<double>::max() );
  
  double nearValue ( 0.0 ), farValue ( 0.0 );
  viewer->getClippingDistances ( nearValue, farValue );
  
  zNear->setValue ( nearValue );
  zFar->setValue  ( farValue );
  
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    layout->addWidget ( new QLabel ( "Near:" ) );
    layout->addWidget ( zNear );
    layout->addWidget ( new QLabel ( "Far:" ) );
    layout->addWidget ( zFar );
    topLayout->addLayout ( layout );
  }
  
  {
    QPushButton *ok ( new QPushButton ( "Ok" ) );
    QPushButton *cancel ( new QPushButton ( "Cancel" ) );
    
    QObject::connect ( ok, SIGNAL ( clicked() ), &dialog, SLOT ( accept() ) );
    QObject::connect ( cancel, SIGNAL ( clicked() ), &dialog, SLOT ( reject() ) );
    
    QHBoxLayout *layout ( new QHBoxLayout );
    layout->addStretch();
    layout->addWidget( ok );
    layout->addWidget( cancel );
    topLayout->addLayout ( layout );
  }
  
  if ( QDialog::Accepted == dialog.exec() )
  {
    nearValue = zNear->value();
    farValue  = zFar->value();
    
    viewer->computeNearFarSet ( false );
    viewer->setClippingDistances ( nearValue, farValue );
  }
}
