
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Glue code for OsgTools::Render::Viewer
//
///////////////////////////////////////////////////////////////////////////////

#include "Viewer.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

using namespace CadKit::Viewer::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer() : _viewer( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _viewer = new OsgTools::Render::Viewer( 0x0, 0x0, 0x0 );
  Usul::Pointers::reference( _viewer );
  _viewer->axes( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  Usul::Pointers::unreference( _viewer );
  _viewer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::!Viewer()
{
  Usul::Pointers::unreference( _viewer );
  _viewer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::render()
{
  if( _viewer )
    _viewer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resize( int w, int h )
{
  if( _viewer )
    _viewer->resize( w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundColor ( float r, float g, float b)
{
  if( _viewer )
    _viewer->backgroundColor ( osg::Vec4 ( r, g, b, 1.0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A button was pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonPress      ( float x, float y, bool left, bool middle, bool right )
{
  if( _viewer )
    _viewer->buttonPress ( x, y, left, middle, right );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A button was released
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonRelease    ( float x, float y, bool left, bool middle, bool right )
{
  if( _viewer )
    _viewer->buttonRelease ( x, y, left, middle, right );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clear()
{
  if( _viewer )
    _viewer->clear();
  Usul::Pointers::unreference( _viewer );
  _viewer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::create()
{
  if( _viewer )
    _viewer->create();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle Navigation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleNavigation ( float x, float y, bool left, bool middle, bool right, Type type )
{
  if( _viewer )
    _viewer->handleNavigation ( x, y, left, middle, right, static_cast < osgGA::GUIEventAdapter::EventType > ( type ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle Seek.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleSeek ( float x, float y, bool left )
{
  if( _viewer )
    _viewer->handleSeek ( x, y, left );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the camera.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::camera ( CameraOption option )
{
  _viewer->camera( static_cast < OsgTools::Render::Viewer::CameraOption > ( option ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setMode ( ViewMode mode )
{
  _viewer->setMode( static_cast < OsgTools::Render::Viewer::ViewMode > ( mode ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mode.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ViewMode Viewer::getMode ()
{
  return static_cast < CadKit::Viewer::Glue::Viewer::ViewMode > ( _viewer->getMode() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

System::IntPtr Viewer::viewer()
{
  return System::IntPtr ( _viewer );
}
