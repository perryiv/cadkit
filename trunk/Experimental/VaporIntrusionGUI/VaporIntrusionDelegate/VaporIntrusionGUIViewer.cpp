///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "VaporIntrusionGUIViewer.h"

#include "Usul/Documents/Manager.h"
#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"
#include "Usul/Interfaces/IViewMode.h"

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



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIViewer::VaporIntrusionGUIViewer ( Document *doc, const QGLFormat& format, QWidget* parent, IUnknown* caller, MaterialContainers matContainers ) :
BaseClass( doc, format, parent, caller ),
_mouseWheelPosition( 0 ),
_mouseWheelSensitivity( 10.0f ),
_materialContainers( matContainers )
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

  if( true == viewer->picking() )
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
  if ( 0x0 == event )
    return;

  typedef OsgTools::Render::EventAdapter EventAdapter;
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;

  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  // Declare the event adapter.
  EventAdapter::Ptr ea ( new EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui( static_cast < unsigned int > ( this->width() ), static_cast < unsigned int > ( this->height() ) ) );
  
  // Get the necessary coordinates.
  const double x ( event->x() );
  const double y ( this->height() - event->y() );

  ea->setMouse ( Usul::Math::Vec2f( x, y ) );

  // Query for ISceneIntersect
  Usul::Interfaces::ISceneIntersect::QueryPtr intersector ( this->viewer() );

  // Check for a valid pointer
  if( false == intersector.valid() )
    return;

  // Create an intersections list
  Intersections hits;

  // Grab all the intersections and make sure there is a hit
  if( false == intersector->intersect( float( x ), float( y ), hits ) )
    return;

  // get the first hit
  osgUtil::LineSegmentIntersector::Intersection hit ( *hits.begin() );

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument()->queryInterface( VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::IID ) );
   
  // Check for a valid pointer
  if( false == document.valid() )
    return;

  // get the point on the model where the intersection occured
  osg::Vec3d p ( hit.getLocalIntersectPoint() );

  // add the new grid point
  document->addGridPointFromViewer( Usul::Math::Vec3f ( p.x(), p.y(), p.z() ) );

  // rebuild the scene
  document->rebuildScene();

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

  BaseClass::wheelEvent( event );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key pressed.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::keyPressEvent ( QKeyEvent * event )
{
  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return;

  if ( 0x0 == event )
    return;

   // Query the active document for IVaporIntrusionGUI
  IVPI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Check for a valid document
  if( false == document.valid() )
    return;

  // Process the key.
  switch ( event->key() )
  {
    case Qt::Key_X:
    {
      document->setEditMode2D( IVPI::EDIT_X_GRID_2D );
      std::cout << "Setting 2D Grid Edit Mode to the X axis" << std::endl;
    }
      break;
    case Qt::Key_Y:
    {
      document->setEditMode2D( IVPI::EDIT_Y_GRID_2D );
      std::cout << "Setting 2D Grid Edit Mode to the Y axis" << std::endl;
    }
      break;
  };

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

