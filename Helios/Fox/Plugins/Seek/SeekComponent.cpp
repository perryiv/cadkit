
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "SeekComponent.h"

#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/Functions/Enable.h"
#include "FoxTools/Functions/Check.h"
#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/Cursor.h"

#include "osgUtil/IntersectVisitor"

#include "Usul/Components/Manager.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ISetTool.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/IAnimate.h"

#include "osg/Vec3"
#include "osg/Quat"

FXDEFMAP ( SeekComponent ) MessageMap[] = 
{
  //          Message Type     ID                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, SeekComponent::ID_SEEK,  SeekComponent::onCommandSeek       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  SeekComponent::ID_SEEK,  SeekComponent::onUpdateSeek       ),
};

FOX_TOOLS_IMPLEMENT ( SeekComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SeekComponent , SeekComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SeekComponent::SeekComponent() : BaseClass(),
  _mainWindow ( 0x0 ),
  _seekCursor ( 0x0 ),
  _seekButton ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SeekComponent::~SeekComponent()
{
  // Make sure seek button doesn't recieve messages anymore
  if ( _seekButton.valid() )
  {
    _seekButton->target ( 0x0 );
    _seekButton->selector ( 0 );
  }
  _seekButton = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SeekComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITool::IID:
    return static_cast < Usul::Interfaces::ITool*>(this);
  case Usul::Interfaces::IFoxToolbar::IID:
    return static_cast < Usul::Interfaces::IFoxToolbar*>(this);
  case Usul::Interfaces::ISetCursor::IID:
    return static_cast < Usul::Interfaces::ISetCursor*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the seek event. See:
//  http://dburns.dhs.org/osgarchiver/archives/March2003/att-0469/PickVisitor.cpp
//
///////////////////////////////////////////////////////////////////////////////

bool SeekComponent::execute( Usul::Interfaces::IUnknown* caller, bool left, bool middle, bool right, bool motion, float x, float y, float z )
{
  if ( left )
  {
    Usul::Interfaces::ISceneIntersect::QueryPtr sceneIntersect ( caller );

    // Return now if we don't have a valid scene intersect interface.
    if ( false == sceneIntersect.valid() )
      return false;

    Usul::Interfaces::ITrackball::QueryPtr tb ( caller );

    // Return now if we don't have a valid trackball interface .
    if( false == tb.valid() )
      return false;

    //Find interface to animate, if one exists
    Usul::Interfaces::IAnimate::QueryPtr animate ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IAnimate::IID ) );

    osgUtil::Hit hit;

    //Return if the click didn't intersect the scene
    if ( !sceneIntersect->intersect ( x, y, hit ) )
      return false;

    //Make copy of trackball's current rotation
    const osg::Quat rot ( tb->getRotation() );

    // Get the new center and distance.
    const osg::Vec3 c2 ( hit.getWorldIntersectPoint() );
    const float d2 ( tb->getDistance() * 1.00f ); // TODO, make this scale-factor a preference...

    //Use the animation interface if we found a valid one
    if( animate.valid() )
      animate->animate ( c2, d2, rot );

    //If no animation interface exists, just set the trackball
    else
    {
      tb->setTrackball( c2, d2, rot, true, true );

      Usul::Interfaces::IRedraw::QueryPtr redraw ( caller );

      if( redraw.valid() )
        redraw->redraw();
    }

    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add seek button to the toolbar
//
///////////////////////////////////////////////////////////////////////////////

void SeekComponent::addButtons( Usul::Interfaces::IUnknown* caller, FoxTools::ToolBar::Bar* toolbar)
{
  _mainWindow = caller;
  _seekCursor = std::auto_ptr< FX::FXCursor > ( FoxTools::Cursors::Factory::create ( FoxTools::Cursors::Factory::CURSOR_SEEK ) );

  typedef FoxTools::Icons::Factory Icons;
  _seekButton = new FoxTools::ToolBar::ToggleButton ( "Seek", "Seek Tool.",  Icons::ICON_SEEK, Icons::ICON_SEEK, this, SeekComponent::ID_SEEK );
  toolbar->append ( _seekButton );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the seek cursor
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor* SeekComponent::getCursor()
{
  return _seekCursor.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek button pressed
//
///////////////////////////////////////////////////////////////////////////////

long SeekComponent::onCommandSeek  ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _mainWindow );

  if ( activeView.valid() &&  activeView->getActiveView() )
  {
    Usul::Interfaces::ISetTool::ValidQueryPtr setTool ( activeView->getActiveView() );
    bool mode ( setTool->getTool() == this );
    mode ? setTool->doneTool() : setTool->setTool( this );
  }
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the seek button
//
///////////////////////////////////////////////////////////////////////////////

long SeekComponent::onUpdateSeek   ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _mainWindow );
  Usul::Interfaces::ISetTool::QueryPtr setTool ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  FoxTools::Functions::enable ( activeView.valid() && activeView->getActiveView() && setTool.valid(), object );
  FoxTools::Functions::check  ( setTool.valid() && setTool->getTool() == this, object );

  //Handled
  return 1;
}


