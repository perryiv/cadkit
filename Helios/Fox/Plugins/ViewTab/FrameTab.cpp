
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FrameTab.h"

#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/RealSpinner.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/GroupBox.h"

#include "Usul/Interfaces/Fox/IFoxTabBook.h"
#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Interfaces/IRedraw.h"

#include "Usul/Cast/Cast.h"

#include "osg/Vec3"
#include "osg/Vec4"
#include "osg/Quat"

#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FrameTab ) MessageMap[] = 
{
  //          Message Type     ID                                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_CENTER_X,     FrameTab::onCommandCenterX     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_CENTER_X,     FrameTab::onUpdateCenterX       ),
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_CENTER_Y,     FrameTab::onCommandCenterY     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_CENTER_Y,     FrameTab::onUpdateCenterY       ),
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_CENTER_Z,     FrameTab::onCommandCenterZ     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_CENTER_Z,     FrameTab::onUpdateCenterZ       ),
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_DISTANCE,     FrameTab::onCommandDistance     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_DISTANCE,     FrameTab::onUpdateDistance       ),
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_ROTATION_X,   FrameTab::onCommandRotationX     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_ROTATION_X,   FrameTab::onUpdateRotationX       ),
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_ROTATION_Y,   FrameTab::onCommandRotationY     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_ROTATION_Y,   FrameTab::onUpdateRotationY       ),
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_ROTATION_Z,   FrameTab::onCommandRotationZ     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_ROTATION_Z,   FrameTab::onUpdateRotationZ       ),
  FXMAPFUNC ( FX::SEL_COMMAND, FrameTab::ID_ROTATION_W,   FrameTab::onCommandRotationW     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  FrameTab::ID_ROTATION_W,   FrameTab::onUpdateRotationW       ),
};

FOX_TOOLS_IMPLEMENT ( FrameTab, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

FrameTab::FrameTab() :
_caller(),
_centerIncrement( 10 ),
_distanceIncrement ( 10 ),
_rotationIncrement ( .01 ),
_contents ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

FrameTab::~FrameTab()
{
  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( _caller );

  if( tabItem.valid( ) )
    tabItem->deleteTab( "Camera" );

  if( 0x0 != _contents )
    delete _contents;

  _caller = static_cast < Usul::Interfaces::IUnknown*> ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the tab control
//
///////////////////////////////////////////////////////////////////////////////

void FrameTab::initialize( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr foxTabBook ( caller );

  if( tabItem.valid() && foxTabBook.valid() )
  {
    _caller = caller;
    tabItem->addTab ( "Camera" );
    FX::FXTabItem *cameraTab ( tabItem->tabItem( "Camera" ) );

    FX::FXTabBook *tabBook ( foxTabBook->tabBook() );

    _contents = new FX::FXVerticalFrame ( tabBook, LAYOUT_FILL_X|LAYOUT_FILL_Y );

    FX::FXGroupBox *centerGroup ( new FX::FXGroupBox( _contents, "Center",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X ) );

    FX::FXHorizontalFrame*  frame ( new FX::FXHorizontalFrame ( centerGroup, LAYOUT_FILL_X ) );
    
    const unsigned int spinnerLayout ( REALSPIN_CYCLIC|FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|FX::LAYOUT_RIGHT );

    new FX::FXLabel  ( frame,  "X:", 0x0, FX::LAYOUT_LEFT  );
    FX::FXRealSpinner *centerX  ( new FX::FXRealSpinner ( frame, 10, this, FrameTab::ID_CENTER_X, spinnerLayout  ) );
    centerX->setRange( -100000,  std::numeric_limits< float >::max() );

    frame = new FX::FXHorizontalFrame ( centerGroup, LAYOUT_FILL_X );
    new FX::FXLabel  ( frame,  "Y:", 0x0, FX::LAYOUT_LEFT  );
    FX::FXRealSpinner *centerY ( new FX::FXRealSpinner ( frame, 10, this, FrameTab::ID_CENTER_Y, spinnerLayout  ) );
    centerY->setRange( -100000,  std::numeric_limits< float >::max() );
    
    frame = new FX::FXHorizontalFrame ( centerGroup, LAYOUT_FILL_X );
    new FX::FXLabel  ( frame,  "Z:", 0x0, FX::LAYOUT_LEFT  );
    FX::FXRealSpinner *centerZ ( new FX::FXRealSpinner ( frame, 10, this, FrameTab::ID_CENTER_Z, spinnerLayout  ) );
    centerZ->setRange( -100000,  std::numeric_limits< float >::max() );
    
    FX::FXGroupBox *distanceGroup ( new FX::FXGroupBox( _contents, "Distance", GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X ) );
    FX::FXRealSpinner *distance ( new FX::FXRealSpinner ( distanceGroup, 10, this, FrameTab::ID_DISTANCE, spinnerLayout ) );
    distance->setRange( -100000, std::numeric_limits< float >::max() );

    FX::FXGroupBox *rotationGroup ( new FX::FXGroupBox( _contents, "Rotation",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X ) );

    frame = new FX::FXHorizontalFrame ( rotationGroup, LAYOUT_FILL_X );
    new FX::FXLabel  ( frame,  "X:", 0x0, FX::LAYOUT_LEFT  );
    FX::FXRealSpinner *rotationX ( new FX::FXRealSpinner ( frame, 10, this, FrameTab::ID_ROTATION_X, spinnerLayout ) );
    rotationX->setRange( -1, 1 );
    rotationX->setIncrement ( 0.01 );

    frame = new FX::FXHorizontalFrame ( rotationGroup, LAYOUT_FILL_X );
    new FX::FXLabel  ( frame,  "Y:", 0x0, FX::LAYOUT_LEFT  );
    FX::FXRealSpinner *rotationY ( new FX::FXRealSpinner ( frame, 10, this, FrameTab::ID_ROTATION_Y, spinnerLayout  ) );
    rotationY->setRange( -1, 1 );
    rotationY->setIncrement ( 0.01 );

    frame = new FX::FXHorizontalFrame ( rotationGroup, LAYOUT_FILL_X );
    new FX::FXLabel  ( frame,  "Z:", 0x0, FX::LAYOUT_LEFT  );    
    FX::FXRealSpinner *rotationZ ( new FX::FXRealSpinner ( frame, 10, this, FrameTab::ID_ROTATION_Z, spinnerLayout ) );
    rotationZ->setRange( -1, 1 );
    rotationZ->setIncrement ( 0.01 );

    frame = new FX::FXHorizontalFrame ( rotationGroup, LAYOUT_FILL_X );
    new FX::FXLabel  ( frame,  "W:", 0x0, FX::LAYOUT_LEFT  );
    FX::FXRealSpinner* rotationW ( new FX::FXRealSpinner ( frame, 10, this, FrameTab::ID_ROTATION_W, spinnerLayout  ) );
    rotationW->setRange( -1, 1 );
    rotationW->setIncrement ( 0.01 );

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center's X value
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandCenterX   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Get the current value
  osg::Vec3 v ( trackball->getCenter() );

  //Set the X value
  v[0] = spinner->getValue();

  //Set the center
  trackball->setCenter( v );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center's Y value
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandCenterY   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Get the current value
  osg::Vec3 v ( trackball->getCenter() );

  //Set the Y value
  v[1] = spinner->getValue();

  //Set the center
  trackball->setCenter( v );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center's Z value
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandCenterZ   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Get the current value
  osg::Vec3 v ( trackball->getCenter() );

  //Set the Z value
  v[2] = spinner->getValue();

  //Set the center 
  trackball->setCenter( v );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the distance
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandDistance   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Set the distance
  trackball->setDistance( spinner->getValue() );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation's X value
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandRotationX   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Get the current value
  osg::Vec4 v ( trackball->getRotation().asVec4() );

  //Set the X value
  v[0] = spinner->getValue();

  //Normalize.  Needed to rotate properly
  v.normalize();

  //Convert to Quat
  osg::Quat rot ( v );

  //Set the rotation
  trackball->setRotation( rot );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation's Y value
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandRotationY   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Get the current value
  osg::Vec4 v ( trackball->getRotation().asVec4() );

  //Set the Y value
  v[1] = spinner->getValue();

  //Normalize.  Needed to rotate properly
  v.normalize();

  //Convert to Quat
  osg::Quat rot ( v );

  //Set the rotation
  trackball->setRotation( rot );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}
 

///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation's Z value
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandRotationZ   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Get the current value
  osg::Vec4 v ( trackball->getRotation().asVec4() );

  //Set the Z value
  v[2] = spinner->getValue();

  //Normalize.  Needed to rotate properly
  v.normalize();

  //Convert to Quat
  osg::Quat rot ( v );

  //Set the rotation
  trackball->setRotation( rot );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation's Z value
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onCommandRotationW   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  //Get needed interfaces
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr  trackball  ( activeView->getActiveView() );
  Usul::Interfaces::IRedraw::ValidQueryPtr     redraw     ( activeView->getActiveView() );

  //Get the current value
  osg::Vec4 v ( trackball->getRotation().asVec4() );

  //Set the W value
  v[3] = spinner->getValue();

  //Normalize.  Needed to rotate properly
  v.normalize();

  //Convert to Quat
  osg::Quat rot ( v );

  //Set the rotation
  trackball->setRotation( rot );

  //Redraw the scene
  redraw->redraw();

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the center X spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateCenterX   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getCenter().x() );
    }
  }
  else
    spinner->disable();
  
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the center Y spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateCenterY   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getCenter().y() );
    }
  }
  else
    spinner->disable();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the center Z spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateCenterZ   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getCenter().z() );
    }
  }
  else
    spinner->disable();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the distance spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateDistance   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getDistance() );
    }
  }
  else
    spinner->disable();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the rotation X spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateRotationX   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getRotation().x() );
    }
  }
  else
    spinner->disable();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the rotation Y spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateRotationY   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getRotation().y() );
    }
  }
  else
    spinner->disable();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the rotation Z spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateRotationZ   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getRotation().z() );
    }
  }
  else
    spinner->disable();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the rotation W spinner
//
///////////////////////////////////////////////////////////////////////////////

long FrameTab::onUpdateRotationW   ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXRealSpinner *spinner = SAFE_CAST_FOX ( FX::FXRealSpinner, object );
  if( 0x0 == spinner )
    return 0;

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );

  Usul::Interfaces::ITrackball::QueryPtr trackball ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( trackball.valid() )
  {
    spinner->enable();

    if( !spinner->hasFocus() )
    {
      spinner->setValue ( trackball->getRotation().w() );
    }
  }
  else
    spinner->disable();

  return 1;
}


