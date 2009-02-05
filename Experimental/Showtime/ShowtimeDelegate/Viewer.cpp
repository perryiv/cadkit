
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Qt viewer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Showtime/ShowtimeDelegate/Viewer.h"

#include "Display/Events/Events.h"

#include "OsgTools/Render/Defaults.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ICanClose.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include "QtCore/QUrl"
#include "QtGui/QResizeEvent"

#include "boost/bind.hpp"

#include <limits>

using namespace Showtime;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( IUnknown::RefPtr doc, const QGLFormat& format, QWidget* parent, IUnknown* caller ) :
  BaseClass ( format, parent ),
  _viewer ( new Display::View::Viewer ( doc ) ),
  _caller ( caller ),
  _document ( doc ),
  _refCount ( 0 ),
  _threadId ( Usul::Threads::currentThreadId() ),
  _mutex ( new Viewer::Mutex ),
  _keysDown()
{
  USUL_TRACE_SCOPE;

  // Initialize openGL.
  this->initializeGL();

  // For convienence;
  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Set the focus policy.
  this->setFocusPolicy ( Qt::ClickFocus );

  // Delete on close.
  this->setAttribute ( Qt::WA_DeleteOnClose );

  // Initalize the placement and size.
  this->_initPlacement();

  // Add us to the document.
  IDocument::QueryPtr document ( _document );
  if ( true == document.valid() )
    document->addWindow ( this );

  // Add ourselves as a modified listener.
  Usul::Interfaces::IModifiedSubject::QueryPtr subject ( doc );
  if ( true == subject.valid() )
  {
    subject->addModifiedObserver ( this );
  }

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
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Viewer::_destroy ), "1668467860" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this object.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_destroy()
{
  USUL_TRACE_SCOPE;

  // Clear members.
  _viewer = 0x0;
  _caller = Usul::Interfaces::IUnknown::RefPtr ( 0x0 );
  _document = 0x0;
  _keysDown.clear();

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

Viewer::IUnknown::RefPtr Viewer::document()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _document;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::IUnknown::RefPtr Viewer::document() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _document;
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
    return static_cast < Usul::Interfaces::IOpenGLContext * > ( this );
  case Usul::Interfaces::IWindow::IID:
    return static_cast < Usul::Interfaces::IWindow * > ( this );
  case Usul::Interfaces::IModifiedObserver::IID:
    return static_cast < Usul::Interfaces::IModifiedObserver * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the mouse state.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class EventType > inline Viewer::ButtonsDown buttonsDown ( EventType *event )
  {
    USUL_TRACE_SCOPE;
    Viewer::ButtonsDown mouse;

    if ( 0x0 != event )
    {
      if ( true == event->buttons().testFlag ( Qt::LeftButton ) )
        mouse.insert ( 0 );
      if ( true == event->buttons().testFlag ( Qt::MidButton ) )
        mouse.insert ( 1 );
      if ( true == event->buttons().testFlag ( Qt::RightButton ) )
        mouse.insert ( 2 );
    }

    return mouse;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::paintGL()
{
  USUL_TRACE_SCOPE;

  ViewerPtr viewer ( this->viewer() );
  if ( true == viewer.valid() )
    viewer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resizeGL ( int width, int height )
{
  USUL_TRACE_SCOPE;

  BaseClass::resizeGL ( width, height );

  ViewerPtr viewer ( this->viewer() );
  if ( ( true == viewer.valid() ) && ( width > 0 ) && ( height > 0 ) )
  {
    viewer->resize ( static_cast < unsigned int > ( width ), static_cast < unsigned int > ( height ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Window now has focus.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::focusInEvent ( QFocusEvent *event )
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

void Viewer::focusOutEvent ( QFocusEvent *event )
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
  return ( Usul::Threads::currentThreadId() == _threadId );
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
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
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
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return QSize ( -1, -1 ) );
  Guard guard ( this );

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

void Viewer::mouseMoveEvent ( QMouseEvent *e )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == e )
    return;

  Display::Events::MouseMove event ( e->x(), e->y(), this->keysDown(), Helper::buttonsDown ( e ) );
  this->_notify ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mousePressEvent ( QMouseEvent *e )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == e )
    return;

  Display::Events::MousePress event ( e->button(), this->keysDown(), Helper::buttonsDown ( e ) );
  this->_notify ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseReleaseEvent ( QMouseEvent *e )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == e )
    return;

  Display::Events::MouseRelease event ( e->button(), this->keysDown(), Helper::buttonsDown ( e ) );
  this->_notify ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been double-clicked.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseDoubleClickEvent ( QMouseEvent *e )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == e )
    return;

  Display::Events::MouseDoubleClick event ( e->button(), this->keysDown(), Helper::buttonsDown ( e ) );
  this->_notify ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse wheel has been moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::wheelEvent ( QWheelEvent *e )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == e )
    return;

  Display::Events::MouseWheel event ( e->delta(), this->keysDown(), Helper::buttonsDown ( e ) );
  this->_notify ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyPressEvent ( QKeyEvent *e )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == e )
    return;

  {
    Guard guard ( this );
    _keysDown.insert ( e->key() );
  }

  Display::Events::KeyPress event ( e->key(), this->keysDown(), Viewer::ButtonsDown() );
  this->_notify ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::keyReleaseEvent ( QKeyEvent *e )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == e )
    return;

  {
    Guard guard ( this );
    _keysDown.erase ( e->key() );
  }

  Display::Events::KeyRelease event ( e->key(), this->keysDown(), Viewer::ButtonsDown() );
  this->_notify ( event );
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
//  Get the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Mutex &Viewer::mutex() const
{
  return *_mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dragging has entering window.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::dragEnterEvent ( QDragEnterEvent *event )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  
  IDocument::QueryPtr document ( this->document() );
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
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IRead::QueryPtr document ( this->document() );
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
//  Close event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::closeEvent ( QCloseEvent *event )
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &Viewer::_closeEvent ), event, "1483190776" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure we can close.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_closeEvent ( QCloseEvent *event )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  Guard guard ( this );

  // Get the document.
  Usul::Interfaces::ICanClose::QueryPtr doc ( this->document() );

  // Ask the document if we can close.
  if ( true == doc.valid() && false == doc->canClose ( this, Usul::Interfaces::IUnknown::QueryPtr ( this ) ) )
  {
    event->ignore();
    return;
  }

  // Close.  Removing this from the document's windows.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Viewer::_close ), "1638161623" );

  // Pass along to the base class.
  BaseClass::closeEvent ( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Tell the document that we are closing.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_close()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Clean up the viewer first.
  {
    Guard guard ( this );
  
    // Save viewer's state and clear.
    if ( _viewer.valid() )
    {
      _viewer->stateSave();
      _viewer->clear();
    }

    _viewer = 0x0;
  }

  // Get the document.
  IDocument::QueryPtr document ( this->document() );

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

Viewer::IUnknown::RefPtr Viewer::caller()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _caller;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the caller.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::IUnknown::RefPtr Viewer::caller() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _caller;
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
  Guard guard ( this );

  // Close. Removing this from the document's windows.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Viewer::_close ), "3614568329" );
  this->close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count.
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
//  Decrement the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::unref ( bool )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0 == _refCount )
  {
    USUL_ASSERT ( 0 );
    throw Usul::Exceptions::Exception ( "Error 1957966139: Reference count is already 0" );
  }

  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the internal viewer.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ViewerPtr Viewer::viewer()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the internal viewer.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::ViewerPtr Viewer::viewer() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the document is modified (Usul::Interfaces::IModifiedObserver).
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::subjectModified ( Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;

  // Queue a repaint. Do not change this behavior unless you also change the 
  // fact that IDocument's requestRedraw() uses the modified-observers to 
  // implement the request, which results in this function being called.
  this->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the saved state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stateLoad()
{
  USUL_TRACE_SCOPE;

  ViewerPtr viewer ( this->viewer() );
  if ( true == viewer.valid() )
  {
    viewer->stateLoad();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the set of keys that are currenty pressed.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::KeysDown Viewer::keysDown() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _keysDown;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the event to the internal viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_notify ( Display::Events::Event &e )
{
  USUL_TRACE_SCOPE;

  ViewerPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  viewer->notify ( e );
}
