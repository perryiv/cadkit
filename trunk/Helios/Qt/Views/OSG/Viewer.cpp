
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
  _refCount ( 0 )
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  // Clear the viewer.
  _viewer->clear();
  _viewer = 0x0;

  Usul::Documents::Manager::instance().close ( _document );

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

  // Make our document and view current.
  Usul::Documents::Manager::instance ().active ( this->document() );
  this->document()->activeView( _viewer.get() );
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
