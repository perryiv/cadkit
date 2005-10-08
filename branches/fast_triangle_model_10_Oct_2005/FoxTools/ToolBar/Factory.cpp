
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/ToolBar/Factory.h"

#include "FoxTools/Headers/ToolBarShell.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/DockSite.h"

#include "FoxTools/Functions/MainWindow.h"

using namespace FoxTools::ToolBar;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Factory* Factory::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance for this singleton.
//
///////////////////////////////////////////////////////////////////////////////

Factory& Factory::instance()
{
  if ( !_instance )
    _instance = new Factory ( );
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the instance.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::release()
{
  if ( _instance )
    delete _instance;
  _instance = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : 
  _toolbars(),
  _topDock(),
  _bottomDock(),
  _leftDock(),
  _rightDock()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{
  _toolbars.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get toolbar by name
//
///////////////////////////////////////////////////////////////////////////////

Bar* Factory::toolbar( const std::string& name )
{
  Toolbars::iterator iter = _toolbars.find( name );
  if( iter != _toolbars.end() )
    return iter->second.get();
  return this->create( name, TOP );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create toolbar at given site
//
///////////////////////////////////////////////////////////////////////////////

Bar* Factory::create ( const std::string& name, Site site )
{
  Bar::RefPtr bar ( 0x0 );

  FX::FXToolBarShell* toolShell ( new FX::FXToolBarShell ( FoxTools::Functions::mainWindow(), FX::FRAME_RAISED ) );

  switch ( site )
  {
  case TOP:
    bar = new Bar( name, this->topDock()->dockSite(), toolShell );
    this->topDock()->append( bar );
    break;
  case BOTTOM:
    bar = new Bar( name, this->bottomDock()->dockSite(), toolShell );
    this->bottomDock()->append( bar );
    break;
  case LEFT:
    bar = new Bar( name, this->leftDock()->dockSite(), toolShell );
    this->leftDock()->append( bar );
    break;
  case RIGHT:
    bar = new Bar( name, this->rightDock()->dockSite(), toolShell );
    this->rightDock()->append( bar );
    break;
  default:
    throw std::runtime_error( "Error 3155132567: Invalid dock site specified." );
  }

  _toolbars.insert( Toolbars::value_type ( name, bar.get() ) );

  return bar.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build all toolbars
//
///////////////////////////////////////////////////////////////////////////////

void Factory::build()
{
  _topDock->build();
  _bottomDock->build();
  _leftDock->build();
  _rightDock->build();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all toolbars
//
///////////////////////////////////////////////////////////////////////////////

void Factory::clear()
{
  _topDock->clear();
  _bottomDock->clear();
  _leftDock->clear();
  _rightDock->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Top dock
//
///////////////////////////////////////////////////////////////////////////////

DockSite* Factory::topDock() 
{
  if( !_topDock )
  {
    _topDock = new DockSite ( FoxTools::Functions::mainWindow(), LAYOUT_SIDE_TOP|LAYOUT_FILL_X  );
  }

  return _topDock.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bottom dock
//
///////////////////////////////////////////////////////////////////////////////

DockSite* Factory::bottomDock()  
{ 
  if( !_bottomDock )
  {
    _bottomDock = new DockSite ( FoxTools::Functions::mainWindow(), LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X  );
  }

  return _bottomDock.get(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Right dock
//
///////////////////////////////////////////////////////////////////////////////

DockSite* Factory::rightDock()   
{ 
  if( !_rightDock )
  {
    _rightDock = new DockSite ( FoxTools::Functions::mainWindow(), LAYOUT_SIDE_RIGHT|LAYOUT_FILL_Y  );
  }

  return _rightDock.get();  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Left dock
//
///////////////////////////////////////////////////////////////////////////////

DockSite* Factory::leftDock()    
{ 
  if( !_leftDock )
  {
    _leftDock = new DockSite ( FoxTools::Functions::mainWindow(), LAYOUT_SIDE_LEFT|LAYOUT_FILL_Y  );
  }

  return _leftDock.get();   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide all
//
///////////////////////////////////////////////////////////////////////////////

void Factory::hide()
{
  this->topDock()->hide();
  this->bottomDock()->hide();
  this->rightDock()->hide();
  this->leftDock()->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show all
//
///////////////////////////////////////////////////////////////////////////////

void Factory::show()
{
  this->topDock()->show();
  this->bottomDock()->show();
  this->rightDock()->show();
  this->leftDock()->show();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform layout on the dock sites.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::layout()
{
  this->topDock()->dockSite()->layout();
  this->bottomDock()->dockSite()->layout();
  this->rightDock()->dockSite()->layout();
  this->leftDock()->dockSite()->layout();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the toolbar with the given name.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::remove ( const std::string& name )
{
  Toolbars::iterator iter = _toolbars.find( name );
  if( iter != _toolbars.end() )
  {
    this->topDock()->remove    ( name );
    this->bottomDock()->remove ( name );
    this->rightDock()->remove  ( name );
    this->leftDock()->remove   ( name );

    _toolbars.erase( iter );
  }
}

