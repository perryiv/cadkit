///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "VaporIntrusionGUIViewer.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IVaporIntrusionGUI.h"

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

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIViewer::VaporIntrusionGUIViewer ( Document *doc, const QGLFormat& format, QWidget* parent, IUnknown* caller ) :
BaseClass( doc, format, parent, caller ),
_cameraDirection( OsgTools::Render::Viewer::FRONT ),
_set( 0, 0, 0 ),
_depth( 0 ),
_mouseWheelPosition( 0 ),
_mouseWheelSensitivity( 10.0f )
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::mouseMoveEvent ( QMouseEvent * event )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  //BaseClass::mouseMoveEvent( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::mousePressEvent ( QMouseEvent * event )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  //BaseClass::mousePressEvent( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A mouse button has been released.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::mouseReleaseEvent ( QMouseEvent * event )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  //BaseClass::mouseReleaseEvent( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse wheel has been moved.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::wheelEvent ( QWheelEvent * event )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  // Query the active document
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument()->queryInterface( Usul::Interfaces::IVaporIntrusionGUI::IID ) );
  if( false == document.valid() )
    return;

  // Get the dimensions
  Usul::Math::Vec3ui dimensions ( document->getDimensions() );

  // Adjust the mouse wheel state.
  int delta ( event->delta() );
  unsigned int oldDepth ( _depth );

  // set the amount to move by
  if( delta < 0 && _depth > 0 )
    --_depth;
  if( delta > 0 && _depth < dimensions[0] - 1 )
    ++_depth;
 

  for( unsigned int i = 0; i < dimensions[1]; ++i )
  {
    for( unsigned int j = 0; j < dimensions[2]; ++j )
    {
      document->setAlpha( oldDepth, i, j, 0.1f );
      document->setAlpha( _depth, i, j, 0.8f );
    }
  }

  //BaseClass::wheelEvent( event );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key pressed.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::keyPressEvent ( QKeyEvent *event )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  BaseClass::keyPressEvent( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key released.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::keyReleaseEvent ( QKeyEvent * event )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  BaseClass::keyReleaseEvent( event );
 
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the Axis
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::camera ( CameraOption option )
{
  
  _cameraDirection = option;

  // Get the viewer
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // Set the camera viewing direction
  viewer->camera( _cameraDirection );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Axis
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIViewer::CameraOption VaporIntrusionGUIViewer::camera ()
{  
  return _cameraDirection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the set
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::set( Usul::Math::Vec3ui s )
{
  _set = s;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the set
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3ui VaporIntrusionGUIViewer::set()
{
  return _set;
}
