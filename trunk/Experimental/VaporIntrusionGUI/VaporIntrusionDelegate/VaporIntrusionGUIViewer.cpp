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
  this->installEventFilter(this);
  this->setMouseTracking(true);
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
  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument()->queryInterface( VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::IID ) );
   
  // Check for a valid pointer
  if( false == document.valid() )
    return;

  // get the mouse position in world space
  osg::Vec3d p ( this->_getIntersectPoint( event ) );

  // set the mouse coords
  document->setMouseCoords( Usul::Math::Vec3f ( p.x(), p.y(), p.z() ), event->globalX(), event->globalY() );

  const bool left   ( ( true == event->buttons().testFlag ( Qt::LeftButton  ) ) );
  const bool right  ( ( true == event->buttons().testFlag ( Qt::RightButton ) ) );
  const bool middle ( ( true == event->buttons().testFlag ( Qt::MidButton   ) ) );

  if( false == left && false == right )
  {
    BaseClass::mouseMoveEvent( event );
  }
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
//  Get the mouse intersection point
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3d VaporIntrusionGUIViewer::_getIntersectPoint( QMouseEvent * event )
{
   typedef OsgTools::Render::EventAdapter EventAdapter;
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;

  OsgTools::Render::Viewer::RefPtr viewer ( this->viewer() );
  if ( false == viewer.valid() )
    return osg::Vec3d( 0.0, 0.0, 0.0 );

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
    return osg::Vec3d( 0.0, 0.0, 0.0 );

  // Create an intersections list
  Intersections hits;

  // Grab all the intersections and make sure there is a hit
  if( false == intersector->intersect( float( x ), float( y ), hits ) )
    return osg::Vec3d( 0.0, 0.0, 0.0 );

  // get the first hit
  osgUtil::LineSegmentIntersector::Intersection hit ( *hits.begin() );

  // get the point on the model where the intersection occured
  osg::Vec3d p ( hit.getLocalIntersectPoint() );

  return p;
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

  // Query the active document for IVaporIntrusionGUI
  VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument()->queryInterface( VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI::IID ) );
   
  // Check for a valid pointer
  if( false == document.valid() )
    return;

  const bool left   ( ( true == event->buttons().testFlag ( Qt::LeftButton  ) ) );
  const bool right  ( ( true == event->buttons().testFlag ( Qt::RightButton ) ) );
  const bool middle ( ( true == event->buttons().testFlag ( Qt::MidButton   ) ) );

	const bool shift ( ( true == event->modifiers().testFlag( Qt::ShiftModifier		) ) );
	const bool ctrl  ( ( true == event->modifiers().testFlag( Qt::ControlModifier ) ) );
	const bool alt   ( ( true == event->modifiers().testFlag( Qt::AltModifier			) ) );


  osg::Vec3d p ( this->_getIntersectPoint( event ) );

	Usul::Math::Vec3f point ( p.x(), p.y(), p.z() );

  // set the mouse coords
  // document->setMouseCoords( Usul::Math::Vec3f ( p.x(), p.y(), p.z() ) );

  if( true == left )
  {
		// simulate right click
		if( true == shift )
		{
			// remove the new grid point
			document->handleRightMouseClick( point );
		}
		// pick sources
		else if( true == ctrl && false == alt )
		{
			document->objectPick( point, IVPI::OBJECT_MODIFIER_SOURCE );
		}
		// pick soils
		else if( false == ctrl && true == alt )
		{
			document->objectPick( point, IVPI::OBJECT_MODIFIER_SOURCE );
		}
		// pick the building
		else if( true == ctrl && true == alt )
		{
			document->objectPick( point, IVPI::OBJECT_MODIFIER_BUILDING );
		}
		else
		{
			// add the new grid point
			document->handleLeftMouseClick( point );
		}
  }
  if( true == right )
  {
    // remove the new grid point
    document->handleRightMouseClick( point );
  }

  // rebuild the scene
  //document->rebuildScene();

  if( true == middle )
  {
    BaseClass::mousePressEvent( event );
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

  // get the current build mode
  int buildMode ( document->getBuildMode2D() );

  // get the current edit mode
  int editMode ( document->getEditMode2D() );

  // get the current view mode
  int viewMode ( document->getViewMode2D() );

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

      case Qt::Key_Comma:
      {
        document->setViewMode2D( IVPI::VIEW_MODE_2D_XY );       
      }
      break;

      case Qt::Key_Escape :
      {
        // reset edit mode to idle
        document->setEditMode2D( IVPI::EDIT_MODE_IDLE );

        // reset to teh default view
        std::cout << "Setting 2D Grid Domain to the XY Grid" << std::endl;
        document->setViewMode2D( IVPI::VIEW_MODE_2D_XY ); 

        // set the object type to nothing
        document->setObjectMode( IVPI::OBJECT_NOTHING );

        // clear the temp object
        document->clearObject();
      }
      break;

      case Qt::Key_B :
      {
        // set the edit mode to object placement
        document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY );

        // set the correct build mode
        document->setViewMode2D( IVPI::VIEW_MODE_2D_XY );

        // set the object type to building
        document->setObjectMode( IVPI::OBJECT_BUILDING );
      }
        break;

      case Qt::Key_I :
      {
        // set the edit mode to object placement
        document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY );

        // set the correct build mode
        document->setViewMode2D( IVPI::VIEW_MODE_2D_XY );

        // set the object type to building
        document->setObjectMode( IVPI::OBJECT_SOIL );
      }
        break;
 
      case Qt::Key_S :
      {
        // set the edit mode to object placement
        document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY );

        // set the correct build mode
        document->setViewMode2D( IVPI::VIEW_MODE_2D_XY );

        // set the object type to building
        document->setObjectMode( IVPI::OBJECT_SOURCE );
      }
        break;

      case Qt::Key_Period:
      {
        document->setViewMode2D( IVPI::VIEW_MODE_2D_XZ );
      }
        break;

      case Qt::Key_C:
      {
        document->setBuildMode2D( IVPI::BUILD_MODE_CRACK_PLACE1 );
        std::cout << "Setting edit mode to foundation cracks.  Left and right clicks will add/remove cracks" << std::endl;
      }
        break;
      case Qt::Key_G:
      {
        document->setBuildMode2D( IVPI::BUILD_MODE_GRID_EDIT );
        std::cout << "Setting edit mode to grid lines.  Left and right clicks will add/remove grid lines" << std::endl;
      }
        break;
      case Qt::Key_Return:
      {
        // toggle the mode
        if( buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY )
        {
          document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_SIZE_XY );
        }

        // toggle the mode
        if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XY )
        {
          // clear the temp object
          document->clearObject();

          // get the object mode
          int objectMode ( document->getObjectMode() );

          // change the view mode and provide feedback to the user
          document->setViewMode2D( IVPI::VIEW_MODE_2D_XZ );
          std::cout << "Setting 2D Grid Domain Mode to the Z (Basement/Soil) Grid" << std::endl;

          // if this is a building
          if( objectMode == IVPI::OBJECT_BUILDING )
          {
            document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_SIZE_XZ );
          }
          // if it is not
          else
          {
            document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XZ );
          }

					// rebuild the temp object
					document->rebuildObject();

        }

        // toggle the mode
        if( buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XZ )
        {
          document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_SIZE_XZ );
        }

        // tell the document to create the object
        if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XZ )
        {
          document->handleNewObject();
          document->setBuildMode2D( IVPI::BUILD_MODE_NONE );

          document->setViewMode2D( IVPI::VIEW_MODE_2D_XY );

          // clear the temp
          document->clearObject();

          // set the object type to nothing
          document->setObjectMode( IVPI::OBJECT_NOTHING );
        }
        
        std::cout << "Setting 2D Grid Size Object Mode" << std::endl;
      }
        break;

			case Qt::Key_Backspace:
      {
        // toggle the mode
        if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XY )
        {
					// go back to placement mode
          document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XY );          
        }

        // toggle the mode
        if( buildMode == IVPI::BUILD_MODE_OBJECT_PLACEMENT_XZ )
        {
					// clear the temp object
          document->clearObject();

          // change the view mode and provide feedback to the user
          document->setViewMode2D( IVPI::VIEW_MODE_2D_XY );
         
					// go back to the xy view
          document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_SIZE_XY );

					// rebuild the temp object
					document->rebuildObject();
        }

        // tell the document to create the object
        if( buildMode == IVPI::BUILD_MODE_OBJECT_SIZE_XZ )
        {
					// get the object mode
          int objectMode ( document->getObjectMode() );

					// if this is a building
          if( objectMode == IVPI::OBJECT_BUILDING )
          {
						// clear the temp object
						document->clearObject();

						// set the build mode to size xy
            document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_SIZE_XY );

						// set the view mode to xy
						document->setViewMode2D( IVPI::VIEW_MODE_2D_XY );

						// rebuild the temp object
						document->rebuildObject();
          }
          // if it is not
          else
          {
            document->setBuildMode2D( IVPI::BUILD_MODE_OBJECT_PLACEMENT_XZ );
          }


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

