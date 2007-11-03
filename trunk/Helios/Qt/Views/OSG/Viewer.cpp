
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
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Commands/PolygonMode.h"
#include "Usul/Commands/RenderingPasses.h"
#include "Usul/Commands/RenderLoop.h"
#include "Usul/Commands/ShadeModel.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Strings/Format.h"

#include "OsgTools/Render/Defaults.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

#include "QtTools/Color.h"

#include "QtCore/QUrl"
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
  _timerRenderLoop ( 0x0 ),
  _keys(),
  _lastMode ( OsgTools::Render::Viewer::NAVIGATION ),
  _sizes (),
  _mutex ( new Viewer::Mutex )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

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

  // Set good default background.
  //_viewer->defaultBackground();

  // Not working...
#if 1
  _viewer->backgroundCorners ( OsgTools::Render::Viewer::Corners::BOTTOM );
  _viewer->backgroundColor ( _viewer->backgroundColor() );
  _viewer->backgroundCorners ( OsgTools::Render::Viewer::Corners::TOP );
  _viewer->backgroundColor ( OsgTools::Render::Defaults::CLEAR_COLOR );
#endif

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

  // Load initial trackball settings.
  _viewer->trackballStateLoad();

  // Enable drag 'n drop.
  this->setAcceptDrops ( true );

  // Add common window sizes.
  // From http://en.wikipedia.org/wiki/Image:Standard_video_res.svg
  _sizes.push_back ( Size ( 640, 480 ) );
  _sizes.push_back ( Size ( 720, 480 ) );
  _sizes.push_back ( Size ( 768, 576 ) );
  _sizes.push_back ( Size ( 1024, 768 ) );
  _sizes.push_back ( Size ( 1280, 720 ) );
  _sizes.push_back ( Size ( 1920, 1080 ) );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );

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
  viewer->handleNavigation ( x, y, left, middle, right, type );
  viewer->handlePicking    ( x, y, false, 0 );
  viewer->handleDragging   ( x, y, OsgTools::Draggers::Dragger::MOVE );
  viewer->handleIntersect  ( x, y );

  // Handle tool.
  viewer->handleTool ( left, middle, right, true, x, y, 0.0 );
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

  bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

  viewer->buttonPress ( x, y, left, middle, right );

  // Does nothing unless in seek mode.
  viewer->handleSeek ( x, y, left );

  // Handle the events. Make sure you pick before you drag.
  viewer->handlePicking  ( x, y, left, 1 );
  viewer->handleDragging ( x, y, OsgTools::Draggers::Dragger::START );
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

  bool left   ( event->buttons().testFlag ( Qt::LeftButton  ) );
  bool middle ( event->buttons().testFlag ( Qt::MidButton   ) );
  bool right  ( event->buttons().testFlag ( Qt::RightButton ) );

  float x ( event->x() );
  float y ( this->height() - event->y() );

  viewer->buttonRelease ( x, y, left, middle, right );

  // Handle the events. Make sure you pick before you drag.
  viewer->handlePicking  ( x, y, left, 1 );
  viewer->handleDragging ( x, y, OsgTools::Draggers::Dragger::FINISH );

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
  Guard guard ( this->mutex() );

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
    Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );

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
  Guard guard ( this->mutex() );

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
  Guard guard ( this->mutex() );

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
  Guard guard ( this->mutex() );

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
  Guard guard ( this->mutex() );

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
  MenuKit::Menu::RefPtr view ( menu.find ( "&View", true ) );

  if ( view.valid () )
  {
    if ( view->items().size () > 0 )
      view->addSeparator ();

    Usul::Interfaces::IUnknown::QueryPtr viewer ( this->viewer () );

    // Typedefs.
    typedef MenuKit::Button Button;
    typedef MenuKit::RadioButton RadioButton;
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

    // Make the menu of common sizes.
    MenuKit::Menu::RefPtr size ( new MenuKit::Menu ( "Size" ) );
    for ( Sizes::const_iterator iter = _sizes.begin(); iter != _sizes.end(); ++iter )
    {
      Size s ( *iter );
      size->append ( new RadioButton ( Usul::Commands::genericCheckCommand ( Usul::Strings::format ( s[0], " x ", s[1] ), 
        Usul::Adaptors::bind2<void> ( s[0], s[1], Usul::Adaptors::memberFunction<void> ( this, &Viewer::_resize ) ), 
        Usul::Adaptors::bind2<bool> ( s[0], s[1], Usul::Adaptors::memberFunction<bool> ( this, &Viewer::_isSize ) ) ) ) );
    }

    view->append ( size );

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
  // Top left corner.
  viewer->backgroundCorners ( Corners::TOP_LEFT );
  osg::Vec4 topLeftColor ( viewer->backgroundColor ( ) );

  // Top right corner.
  viewer->backgroundCorners ( Corners::TOP_RIGHT );
  osg::Vec4 topRightColor ( viewer->backgroundColor ( ) );

  // Bottom left corner.
  viewer->backgroundCorners ( Corners::BOTTOM_LEFT );
  osg::Vec4 bottomLeftColor ( viewer->backgroundColor ( ) );

  viewer->backgroundCorners ( Corners::BOTTOM_RIGHT );
  osg::Vec4 bottomRightColor ( viewer->backgroundColor ( ) );

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
