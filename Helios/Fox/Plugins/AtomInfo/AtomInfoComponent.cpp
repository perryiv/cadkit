
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

#include "AtomInfoComponent.h"

#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Functions/App.h"

#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/MenuPane.h"
#include "FoxTools/Headers/CheckButton.h"

#include "osgUtil/IntersectVisitor"

#include "osgPlugins/pdb/Atom.h"

#include "Usul/Cast/Cast.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ISetTool.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/INotify.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"

#include <sstream>

using namespace OsgFox;
using namespace OsgFox::Plugins;
using namespace OsgFox::Plugins::AtomInfo;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Component , Component::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( Component ) MessageMap[] = 
{
  //          Message Type     ID                      Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, Component::ID_FIX_ATOM, Component::onCommandFixAtom  ),
};

FOX_TOOLS_IMPLEMENT ( Component, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::Component() : BaseClass(),
_caller( )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::~Component()
{
  _caller = static_cast< Usul::Interfaces::IUnknown* > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Component::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFoxEvent::IID:
    return static_cast < Usul::Interfaces::IFoxEvent*>(this);
  case Usul::Interfaces::IMenuEntry::IID:
    return static_cast < Usul::Interfaces::IMenuEntry*>(this);
  case Usul::Interfaces::ICommand::IID:
    return static_cast < Usul::Interfaces::ICommand*>(this);
#ifdef _DEBUG
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
#endif
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add atom info tool to the active view.
//
///////////////////////////////////////////////////////////////////////////////

void Component::execute ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::IActiveView::QueryPtr activeView( caller );
  
  if( activeView.valid() )
  {
    Usul::Interfaces::ISetTool::ValidQueryPtr setTool ( activeView->getActiveView() );

    setTool->setTool ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see what happened and take action
//
///////////////////////////////////////////////////////////////////////////////

bool Component::execute( Usul::Interfaces::IUnknown* caller, const FX::FXEvent &event)
{
  //was the left button clicked?
  if ( event.type == FX::SEL_LEFTBUTTONPRESS )
    return this->_handleLeftClick( caller, event );

  //Is the mouse moving?
  if ( event.type == FX::SEL_MOTION )
    return this->_handleMouseMove( caller, event );

  //was the right button clicked?
  if( event.type == FX::SEL_RIGHTBUTTONPRESS )
    return this->_handleRightClick( caller, event );

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::menuText() const
{
  return "Atom Info";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::statusBarText() const
{
  return "Get information on selected atom.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::menuGroup() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Left mouse button was clicked.
//
///////////////////////////////////////////////////////////////////////////////

bool Component::_handleLeftClick  ( Unknown *caller, const FX::FXEvent &event )
{
  Usul::Interfaces::ISceneIntersect::ValidQueryPtr sceneIntersect ( caller );

  osgUtil::Hit hit;
  if ( !sceneIntersect->intersect ( event, hit ) )
    return false;

  //Display the information in a pop up
  Usul::Interfaces::INotifyString::ValidQueryPtr notify ( _caller );
  notify->notify( this->queryInterface( Usul::Interfaces::IUnknown::IID ), this->_getAtomInfo( hit ) );

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse was moved
//
///////////////////////////////////////////////////////////////////////////////

bool Component::_handleMouseMove  ( Unknown *caller, const FX::FXEvent &event )
{ 
  Usul::Interfaces::ISceneIntersect::ValidQueryPtr sceneIntersect ( caller );

  osgUtil::Hit hit;
  if ( !sceneIntersect->intersect ( event, hit ) )
    return false;

  //Display the information on the status bar
  Usul::Interfaces::IStatusBar::ValidQueryPtr statusBar ( _caller );
  statusBar->setStatusBarText( this->_getAtomInfo( hit ), true );

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Right mouse button was clicked.
//
///////////////////////////////////////////////////////////////////////////////

bool Component::_handleRightClick ( Unknown *caller, const FX::FXEvent &event )
{
  Usul::Interfaces::ISceneIntersect::ValidQueryPtr sceneIntersect ( caller );

  osgUtil::Hit hit;
  if ( !sceneIntersect->intersect ( event, hit ) )
    return false;

  
  if( osgPlugins::pdb::Atom* atom = this->_getAtom( hit._geode.get() ) )
  {
    //TODO.  This may not work anymore.  Test once Molecule Model is made
    FX::FXMenuPane menu ( reinterpret_cast < FX::FXWindow* > ( FoxTools::Functions::mainWindow() ) );

    FX::FXCheckButton check ( &menu, "Fix Atom", this, Component::ID_FIX_ATOM );

    check.setCheck( ( atom->fixed() ) ? 1 : 0 );
    check.setUserData( atom );

    menu.create();
    menu.popup ( 0x0, event.root_x, event.root_y );

    // Run a model loop until the menu is dismissed.
    FoxTools::Functions::application()->runModalWhileShown ( &menu );
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the atom info as a string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::_getAtomInfo( osgUtil::Hit& hit )
{
  std::ostringstream os;
  if( osgPlugins::pdb::Atom *atom = this->_getAtom( hit._geode.get() ) )
    os << "ID: " << atom->getId() << "   Element: " << atom->getSymbol() << "   Residue: " << atom->getResidueName() << "   Position: " << atom->getCenter()[0] << " " << atom->getCenter()[1] << " "<< atom->getCenter()[2];
  else
    os << "This does not appear to be a molecule" << std::endl;

  return os.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the atom from geode.
//
///////////////////////////////////////////////////////////////////////////////

osgPlugins::pdb::Atom* Component::_getAtom( osg::Geode* geode )
{
  if( osg::Referenced *userdata = geode->getUserData() )
    return dynamic_cast< osgPlugins::pdb::Atom * > ( userdata );
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Component::_getScene( Unknown *caller )
{
  Usul::Interfaces::IOpenSceneGraph::ValidQueryPtr osg ( caller );

  // Get the scene.
  osg::ref_ptr<osg::Node> model ( dynamic_cast < osg::Node * > ( osg->osgReferenced() ) );
  return model.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle when fixed atom is clicked.
//
///////////////////////////////////////////////////////////////////////////////

long Component::onCommandFixAtom  ( FX::FXObject *object, FX::FXSelector, void *data )
{
  FX::FXCheckButton *check = SAFE_CAST_FOX ( FX::FXCheckButton, object );
  if ( 0x0 == check )
    return 0;

  if( void *userdata = check->getUserData() )
  {
    osgPlugins::pdb::Atom *atom = static_cast< osgPlugins::pdb::Atom* > ( userdata );
    if( data )
      atom->fixed( true );
    else
      atom->fixed( false );

    Usul::Interfaces::IActiveView::ValidQueryPtr view ( _caller );
    Usul::Interfaces::IGetDocument::ValidQueryPtr getDocument ( view->getActiveView() );
    Usul::Interfaces::IDocument::ValidQueryPtr document ( getDocument->getDocument() );

    document->modified( true );

  }

  return 1;
}
