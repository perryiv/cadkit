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

Usul::Types::Uint32 VaporIntrusionGUIViewer::_selectedViewID   ( 0 );

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
_mouseWheelSensitivity( 10.0f ),
_id( 0 )
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

  // If we weren't the last view window to run reset the alpha for the scene
  // and set the current running view tag to our id
  if( this->id() != _selectedViewID )
  {
    for( unsigned int x = 0; x < dimensions[0]; ++x )
    {
      for( unsigned int y = 0; y < dimensions[1]; ++y )
      {
        for( unsigned int z = 0; z < dimensions[2]; ++z )
        {
          document->setAlpha( x, y, z, 0.1f );
          _selectedViewID = this->id();
        }
      }
    }
  }

  // Adjust the mouse wheel state.
  int delta ( event->delta() );
  unsigned int oldDepth ( _depth );

  // i,j dimensions
  unsigned int iDim ( 0 );
  unsigned int jDim( 0 );

  // Top view
  if( _cameraDirection == RenderViewer::TOP )
  {
    iDim = dimensions[0];
    jDim = dimensions[2];

    // set the amount to move by
    if( delta < 0 && _depth > 0 )
      --_depth;
    if( delta > 0 && _depth < dimensions[1] - 1 )
      ++_depth;

  }

  // Left View
  else if( _cameraDirection == RenderViewer::LEFT )
  {
    iDim = dimensions[1];
    jDim = dimensions[2];

    // set the amount to move by
    if( delta < 0 && _depth > 0 )
      --_depth;
    if( delta > 0 && _depth < dimensions[0] - 1 )
      ++_depth;
  }

  // Front ( and default ) View
  else
  {
    iDim = dimensions[0];
    jDim = dimensions[1];

    // set the amount to move by
    if( delta < 0 && _depth > 0 )
      --_depth;
    if( delta > 0 && _depth < dimensions[2] - 1 )
      ++_depth;
  }

  // Set and unset the alpha for the active selection
  for( unsigned int i = 0; i < iDim; ++i )
  {
    for( unsigned int j = 0; j < jDim; ++j )
    {

      // Top View
      if( _cameraDirection == RenderViewer::TOP )
      {
        document->setAlpha( i, oldDepth, j, 0.1f );
        document->setAlpha( i, _depth, j, 0.8f );
      }

      // Left View
      else if( _cameraDirection == RenderViewer::LEFT )
      {
        document->setAlpha( oldDepth, i, j, 0.1f );
        document->setAlpha( _depth, i, j, 0.8f );
      }

      // Front (default) View
      else
      {
        document->setAlpha( i, j, oldDepth, 0.1f );
        document->setAlpha( i, j, _depth, 0.8f );
      }
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


///////////////////////////////////////////////////////////////////////////////
//
//  Get the set
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::id( Usul::Types::Uint32 i )
{
  _id = i;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the set
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 VaporIntrusionGUIViewer::id()
{
  return _id;
}

