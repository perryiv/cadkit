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

#include "OsgTools/State/StateSet.h"

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

#include "osg/Material"

Usul::Types::Uint32 VaporIntrusionGUIViewer::_selectedViewID   ( 0 );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIViewer::VaporIntrusionGUIViewer ( Document *doc, const QGLFormat& format, QWidget* parent, IUnknown* caller, MaterialContainer* matContainer ) :
BaseClass( doc, format, parent, caller ),
_cameraDirection( OsgTools::Render::Viewer::FRONT ),
_set( 0, 0, 0 ),
_depth( 0 ),
_mouseWheelPosition( 0 ),
_mouseWheelSensitivity( 10.0f ),
_id( 0 ),
_materialContainer( matContainer )
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
  typedef OsgTools::Render::EventAdapter EventAdapter;
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;

  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

  // Declare the event adapter.
  EventAdapter::Ptr ea ( new EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui( static_cast < unsigned int > ( this->width() ), static_cast < unsigned int > ( this->height() ) ) );
  
  // Get the necessary coordinates.
  const double x ( event->x() );
  const double y ( this->height() - event->y() );

  ea->setMouse ( Usul::Math::Vec2f( x, y ) );

  // Query for ISceneIntersect
  Usul::Interfaces::ISceneIntersect::QueryPtr sceneIntersect ( this->viewer() );

  // Check for a valid pointer
  if( false == sceneIntersect.valid() )
    return;

  // Create an intersections list
  Intersections intersections;

  // Grab all the intersections
  sceneIntersect->intersect( float( x ), float( y ), intersections );

   // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument()->queryInterface( Usul::Interfaces::IVaporIntrusionGUI::IID ) );

  // Check for a valid pointer
  if( false == document.valid() )
    return;

  // Get the dimensions
  Usul::Math::Vec3ui dimensions ( document->dimensions() );
  
  typedef Usul::Properties::Attribute<Usul::Math::Vec3ui,osg::Referenced> Vec3UserData;
  
  for( Intersections::iterator iter = intersections.begin(); iter != intersections.end(); ++iter )
  {
    unsigned int size ( (*iter).nodePath.size() );

    for( unsigned int i = 0; i < size; ++i )
    {
      // Get the drawable
      osg::ref_ptr< osg::Node > node ( (*iter).nodePath.at( i ) );
      
      // make sure the node is valid
      if( true == node.valid() )
      {
        
        Vec3UserData::RefPtr ud ( dynamic_cast< Vec3UserData* > ( node->getUserData() ) );
        if( true == ud.valid() )
        {
          Usul::Math::Vec3ui value ( ud->value() );
          this->_colorCube( value[0], value[1], value[2], document );
          break;
        }
      }
    }
  }
 
  //BaseClass::mousePressEvent( event );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Color the cube
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::_colorCube( unsigned int x, unsigned int y, unsigned int z, Usul::Interfaces::IVaporIntrusionGUI* document )
{
  Guard guard ( this );

  osg::Vec4 osgColor ( 1.0, 0.0, 0.0, 1.0 );
  std::string name;
  std::string value;

  if( 0x0 != _materialContainer )
  {
    osgColor = _materialContainer->getCurrentColor();
    name = _materialContainer->getCurrentName();
    value = _materialContainer->getCurrentValue();
  }
  Usul::Math::Vec4f color( osgColor.r(), osgColor.g(), osgColor.b(), osgColor.a() );


  // These next set of if statements are necessary even though they do the same thing.
  // Since there are different 2D views we only want the active one to operate on the
  // the cubes.

  // Top view -- Y is depth
  if( _cameraDirection == RenderViewer::TOP && _depth == y )
  {
   // Set the material for the cube
    document->setMaterial( x, y, z, color ); 

    // add the value
    document->addValueAt( x, y, z, name, value );

  }

  // Left View -- X is depth
  else if( _cameraDirection == RenderViewer::LEFT && _depth == x )
  {
    // Set the material for the cube
    document->setMaterial( x, y, z, color );

    // add the value
    document->addValueAt( x, y, z, name, value ); 

  }

  // Front ( and default ) View -- Z is depth
  else if ( _cameraDirection == RenderViewer::FRONT && _depth == z )
  {
    // Set the material for the cube
    document->setMaterial( x, y, z, color );

    // add the value
    document->addValueAt( x, y, z, name, value );
  }

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

  // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );
  if( false == document.valid() )
    return;

  // Get the dimensions
  Usul::Math::Vec3ui dimensions ( document->dimensions() );

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
          Guard guard ( this );

          document->setAlpha( x, y, z, 0.1f );
          _selectedViewID = this->id();
        }
      }
    }
  }

  // Adjust the mouse wheel state.
  int delta ( event->delta() );
  
  this->_focusChange( delta, dimensions, document );

  //BaseClass::wheelEvent( event );
  
}


///////////////////////////////////////////////////////////////////////////////
//
// Slide the current focal area.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::_focusChange( int delta, Usul::Math::Vec3ui dimensions, Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document )
{
  unsigned int oldDepth ( _depth );

  // i,j dimensions
  unsigned int iDim ( 0 );
  unsigned int jDim( 0 );

  // Top view
  if( _cameraDirection == RenderViewer::TOP )
  {
    Guard guard ( this );

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
    Guard guard ( this );

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
    Guard guard ( this );

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
      Guard guard ( this );

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

   // Query the active document for IVaporIntrusionGUI
  Usul::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument()->queryInterface( Usul::Interfaces::IVaporIntrusionGUI::IID ) );
  if( false == document.valid() )
    return;

  // Get the dimensions
  Usul::Math::Vec3ui dimensions ( document->dimensions() );

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
          Guard guard ( this );

          document->setAlpha( x, y, z, 0.1f );
          _selectedViewID = this->id();
        }
      }
    }
  }

  // What key was pressed
  switch ( event->key() )
  {
  
    // If it was the comma key...
    case Qt::Key_Comma:
      
      this->_focusChange( -1, dimensions, document );
      break;

    // If it was the period key...
    case Qt::Key_Period:

      this->_focusChange( 1, dimensions, document );
      break;

    // No match? Do nothing.
    default:
      break;
  }

  // Let the baseclass handle key events as well.
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

