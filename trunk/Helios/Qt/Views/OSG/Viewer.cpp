
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
#include "Usul/Commands/PolygonMode.h"
#include "Usul/Commands/RenderingPasses.h"
#include "Usul/Commands/RenderLoop.h"
#include "Usul/Commands/ShadeModel.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Make.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/DateTime.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Render/Defaults.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

#include "QtTools/Color.h"

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

#include <ctime>

using namespace CadKit::Helios::Views::OSG;
namespace Sections = Usul::Registry::Sections;
namespace Keys = Usul::Registry::Keys;
typedef Usul::Registry::Database Reg;


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
  _timerRenderLoop ( 0x0 ),
  _keys(),
  _lastMode ( OsgTools::Render::Viewer::NAVIGATION ),
  _mutex ( new Viewer::Mutex )
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

  // Set the focus.
  this->setFocus();

  // Delete on close.
  this->setAttribute ( Qt::WA_DeleteOnClose );

  // Initalize the placement and size.
  this->_initPlacement ();

  // Add this to the document.
  if ( 0x0 != doc )
    doc->addWindow ( this );

  // Initialize the timer.
  _timer = new QTimer ( this );

  // Initialize the timer for render loop.
  _timerRenderLoop = new QTimer ( this );

  // Save the mode.
  _lastMode = _viewer->getMode();

  // Add ourselves as a modified listener.
  Usul::Interfaces::IModifiedSubject::QueryPtr subject ( doc );
  if ( true == subject.valid() )
  {
    subject->addModifiedObserver ( this );
  }

  // Load initial settings.
  _viewer->trackballStateLoad();
  _viewer->backgroundLoad();

  // Enable drag 'n drop.
  this->setAcceptDrops ( true );
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

  // Clear the viewer.
  _viewer->clear();
  _viewer = 0x0;

  // Get the document.
  Viewer::Document::RefPtr document ( this->document() );
  if ( true == document.valid() )
  {
    // Remove this window from the document's sets.
    document->removeWindow ( this );

    // Tell the document this is closing.  
    // Make sure function is called after removeWindow is called.
    document->closing ( this );
  }

  // Remove ourselves as a modified listener.
  Usul::Interfaces::IModifiedSubject::QueryPtr subject ( this->document() );
  if ( true == subject.valid() )
  {
    subject->removeModifiedObserver ( this );
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
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  const bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  const bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  const bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

  // See if any mouses button are down.
  unsigned int mouse ( left || middle || right );

  // Set the event type.
  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::EventType type ( ( mouse ) ? EventAdapter::DRAG : EventAdapter::MOVE );
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, type) );
  viewer->mouseMove ( ea.get() );
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

  const bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  const bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  const bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

   // Set the event type.
  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, EventAdapter::PUSH ) );
  viewer->buttonPress ( ea.get() );
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

  const bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  const bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  const bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

  typedef OsgTools::Render::EventAdapter EventAdapter;
  EventAdapter::Ptr ea ( viewer->eventAdaptor ( x, y, left, middle, right, EventAdapter::RELEASE ) );
  viewer->buttonRelease ( ea.get() );
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

void Viewer::keyPressEvent ( QKeyEvent * event )
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

  // Process the key.
  switch ( event->key() )
  {
  // See if it was the space-bar or the r-key...
  case Qt::Key_Space:
  case Qt::Key_R:

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

    _lastMode = viewer->getMode();
    viewer->setMode ( OsgTools::Render::Viewer::SEEK );
    break;

  case Qt::Key_Escape:
    viewer->cycleMode();
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
    viewer->setMode ( _lastMode );
  }
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

  // Queue a repaint.
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

void Viewer::renderLoop ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != _timerRenderLoop )
  {
    _timerRenderLoop->stop();

    if ( b )
    {
      this->connect ( _timerRenderLoop, SIGNAL ( timeout() ), this, SLOT ( _onTimeoutRenderLoop() ) );

      // Set the timer to go off every 15 milliseconds.
      _timerRenderLoop->start ( static_cast<int> ( 15 ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get render loop flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::renderLoop () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  return ( 0x0 != _timerRenderLoop ? _timerRenderLoop->isActive () : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_onTimeoutRenderLoop()
{
  USUL_TRACE_SCOPE;
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( true == viewer.valid() )
    viewer->render ();
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

  if ( view.valid () )
  {
    if ( view->items().size () > 0 )
      view->addSeparator ();

    Usul::Interfaces::IUnknown::QueryPtr viewer ( this->viewer () );

    // Typedefs.
    typedef MenuKit::Button Button;
    typedef MenuKit::RadioButton RadioButton;
    typedef MenuKit::ToggleButton ToggleButton;
    typedef Usul::Commands::RenderingPasses RenderingPasses;
    typedef Usul::Commands::PolygonMode PolygonMode;
    typedef Usul::Interfaces::IPolygonMode IPolygonMode;
    typedef Usul::Commands::ShadeModel ShadeModel;
    typedef Usul::Interfaces::IShadeModel IShadeModel;

    view->append ( new Button ( Usul::Commands::genericCommand ( "Edit Background", Usul::Adaptors::memberFunction<void> ( this, &Viewer::editBackground ), Usul::Commands::TrueFunctor() ) ) );

    // Rendering passes menu.
    {
      MenuKit::Menu::RefPtr passes ( new MenuKit::Menu ( "Rendering Passes" ) );
      view->append ( passes.get() );

      passes->append ( new RadioButton ( new RenderingPasses ( "1", 1, viewer.get () ) ) );
      passes->append ( new RadioButton ( new RenderingPasses ( "3", 3, viewer.get () ) ) );
      passes->append ( new RadioButton ( new RenderingPasses ( "9", 9, viewer.get () ) ) );
      passes->append ( new RadioButton ( new RenderingPasses ( "12", 12, viewer.get () ) ) );
    }

    MenuKit::Button::RefPtr rl ( new MenuKit::ToggleButton ( new Usul::Commands::RenderLoop ( "Render Loop", viewer.get() ) ) );
    view->append ( rl );

    // Polygons menu.
    {
      MenuKit::Menu::RefPtr polygons ( new MenuKit::Menu ( "Polygons" ) );
      view->append ( polygons.get() );

      polygons->append ( new RadioButton ( new PolygonMode ( "Filled",       IPolygonMode::FILLED, viewer.get() ) ) );
      polygons->append ( new RadioButton ( new PolygonMode ( "Hidden Lines", IPolygonMode::HIDDEN_LINES, viewer.get() ) ) );
      polygons->append ( new RadioButton ( new PolygonMode ( "Wireframe",    IPolygonMode::WIRE_FRAME, viewer.get() ) ) );
      polygons->append ( new RadioButton ( new PolygonMode ( "Points",       IPolygonMode::POINTS, viewer.get() ) ) );
    }

    // Shading menu.
    {
      MenuKit::Menu::RefPtr shading ( new MenuKit::Menu ( "Shading" ) );
      view->append ( shading.get() );

      shading->append ( new RadioButton ( new ShadeModel ( "Smooth", IShadeModel::SMOOTH, viewer.get() ) ) );
      shading->append ( new RadioButton ( new ShadeModel ( "Flat",   IShadeModel::FLAT, viewer.get() ) ) );
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

    // Make the menu of common sizes.
    MenuKit::Menu::RefPtr size ( new MenuKit::Menu ( "Size" ) );
    for ( Sizes::const_iterator iter = sizes.begin(); iter != sizes.end(); ++iter )
    {
      Size s ( *iter );
      size->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( Usul::Strings::format ( s[0], " x ", s[1] ), 
        Usul::Adaptors::bind2<void> ( s[0], s[1], Usul::Adaptors::memberFunction<void> ( this, &Viewer::_resize ) ), 
        Usul::Adaptors::bind2<bool> ( s[0], s[1], Usul::Adaptors::memberFunction<bool> ( this, &Viewer::_isSize ) ) ) ) );
    }
    size->append ( new Button ( Usul::Commands::genericCommand ( "Custom...", 
        Usul::Adaptors::memberFunction<void> ( this, &Viewer::_customSize ), 
        Usul::Commands::TrueFunctor() ) ) );

    view->append ( size );

    // Frame dump button.
    view->append ( new ToggleButton ( Usul::Commands::genericToggleCommand ( "Frame Dump", Usul::Adaptors::memberFunction<void> ( this, &Viewer::_frameDump ), Usul::Adaptors::memberFunction<bool> ( this, &Viewer::_isFrameDump ) ) ) );

    typedef OsgTools::Render::Viewer OsgViewer;

    // Axes button.
    view->append ( new ToggleButton ( Usul::Commands::genericToggleCommand ( "Show Axes", Usul::Adaptors::memberFunction<void> ( _viewer.get(), &OsgViewer::axesShown ), Usul::Adaptors::memberFunction<bool> ( _viewer.get(), &OsgViewer::isAxesShown ) ) ) );
  }
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

  Urls urls ( event->mimeData()->urls () );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile ().toStdString () );

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

  Urls urls ( event->mimeData()->urls () );

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  for ( ConstIterator i = urls.begin(); i != urls.end(); ++ i )
  {
    std::string file ( i->toLocalFile ().toStdString () );

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
  QSize size ( this->size () );
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

  const QSize size ( this->size () );

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

void Viewer::_frameDump ( bool b )
{
  USUL_TRACE_SCOPE;
  
  // Get the viewer.
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );

  // Make sure it's valid.
  if ( false == viewer.valid() )
    return;

  // Set the frame dump state.
  _viewer->frameDump().dump ( ( b && this->_frameDumpProperties() ? true : false ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame dump state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_isFrameDump() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _viewer.valid() ? _viewer->frameDump().dump() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show a dialog for frame dump properties.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_frameDumpProperties()
{
  // Get the directory from the registry.
  std::string directory ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::FRAME_DUMP_DIRECTORY].get<std::string> ( "" ) );

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
  viewer->frameDump().digits ( 10 );
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
