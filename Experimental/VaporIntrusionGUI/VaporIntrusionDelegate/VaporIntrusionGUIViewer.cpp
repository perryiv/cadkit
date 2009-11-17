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
//  The mouse has been double clicked.
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::mouseDoubleClickEvent ( QMouseEvent * event )
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

  const bool left   ( ( true == event->buttons().testFlag ( Qt::LeftButton  ) ) );
  const bool right  ( ( true == event->buttons().testFlag ( Qt::RightButton ) ) );

  if( true == left )
  {
    // add the new grid point
    document->handleLeftMouseClick( Usul::Math::Vec3f ( p.x(), p.y(), p.z() ) );
  }
  if( true == right )
  {
    // remove the new grid point
    document->handleRightMouseClick( Usul::Math::Vec3f ( p.x(), p.y(), p.z() ) );
  }

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

  // get the key pressed
  int key ( event->key() );

  if( key == Qt::Key_Left || key == Qt::Key_Right || key == Qt::Key_Up || key == Qt::Key_Down )
  {
    this->_handleArrowKeys( key, document );
  }
  else
  {

    // Process the key.
    switch ( key )
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

      case Qt::Key_G:
      {
        document->setBuildMode2D( IVPI::BUILD_MODE_2D_XY );
        std::cout << "Setting 2D Grid Domain to the XY Grid" << std::endl;
        this->viewer()->camera( OsgTools::Render::Viewer::TOP );
      }
        break;

      case Qt::Key_B:
      {
        document->setBuildMode2D( IVPI::BUILD_MODE_2D_Z );
        std::cout << "Setting 2D Grid Domain Mode to the Z (Basement/Soil) Grid" << std::endl;
        this->viewer()->camera( OsgTools::Render::Viewer::FRONT );
      }
        break;

      case Qt::Key_Return:
      {
        // get the current mode
        int currentMode ( document->getEditMode2D() );

        // toggle the mode
        if( currentMode == IVPI::OBJECT_PLACEMENT_2D )
        {
          document->setEditMode2D( IVPI::OBJECT_SIZE_XY );
        }

        // toggle the mode
        if( currentMode == IVPI::OBJECT_SIZE_XY )
        {
          document->setEditMode2D( IVPI::OBJECT_SIZE_XZ );
          document->setBuildMode2D( IVPI::BUILD_MODE_2D_Z );
          this->viewer()->camera( OsgTools::Render::Viewer::FRONT );
          std::cout << "Setting 2D Grid Domain Mode to the Z (Basement/Soil) Grid" << std::endl;
        }

        // tell the document to create the object
        if( currentMode == IVPI::OBJECT_SIZE_XZ )
        {
          document->handleNewObject();
          document->setEditMode2D( IVPI::EDIT_MODE_IDLE );

          document->setBuildMode2D( IVPI::BUILD_MODE_2D_XY );
          std::cout << "Setting 2D Grid Domain to the XY Grid" << std::endl;
          this->viewer()->camera( OsgTools::Render::Viewer::TOP );
        }
        
        std::cout << "Setting 2D Grid Size Object Mode" << std::endl;
      }
        break;
    };
  }

#if 0
  BaseClass::keyPressEvent( event );
#else
  // only let the base class handle certain key presses
  if( key == Qt::Key_F || key == Qt::Key_W )
  {
    BaseClass::keyPressEvent( event );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle arrow key pressed events
//
///////////////////////////////////////////////////////////////////////////////

void VaporIntrusionGUIViewer::_handleArrowKeys( int key, IVPI::QueryPtr document )
{
  // initial movement values
  int x ( 0 );
  int y ( 0 );

  switch( key )
  {
    case Qt::Key_Up :
    {
      y = -1;
    }
    break;

    case Qt::Key_Down :
    {
      y = 1;
    }
    break;

    case Qt::Key_Left :
    {
      x = -1;
    }
    break;

    case Qt::Key_Right :
    {
      x = 1;
    }
    break;

  };

  // set the change in the document
  document->keyMovementChange( x, y );
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

  //BaseClass::keyReleaseEvent( event );
 
}

