
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/ToolBar/DockSite.h"

#include "FoxTools/Headers/DockSite.h"

#include <algorithm>
#include <functional>

using namespace FoxTools::ToolBar;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

DockSite::DockSite( FX::FXComposite *parent, unsigned int layout ) :
BaseClass ( "" ),
_bars(),
_docksite( new FX::FXDockSite( parent, layout ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

DockSite::~DockSite()
{
  this->clear();

  delete _docksite;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the FOX DockSite
//
///////////////////////////////////////////////////////////////////////////////

FX::FXDockSite* DockSite::dockSite()
{
  return _docksite;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append
//
///////////////////////////////////////////////////////////////////////////////

void DockSite::append( Bar *bar )
{
  _bars.push_back( bar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build
//
///////////////////////////////////////////////////////////////////////////////

void DockSite::build()
{
  // Return now if bars is empty
  if( _bars.empty() )
    return;

  unsigned int layout ( FX::LAYOUT_SIDE_TOP | FX::FRAME_RAISED );

  // We know we have at least one
  _bars.front()->layout ( _bars.front()->layout() | layout | FX::LAYOUT_DOCK_NEXT );
  _bars.front()->build( true );

  // Go through the rest
  for( Bars::iterator i = _bars.begin() + 1; i != _bars.end(); ++i )
  {
#if 0
    //Get the registry
  FX::FXRegistry &reg = this->getApp()->reg();
  const char *section = Helios::Registry::MAIN_WINDOW;

  //Get the registry key names
  std::string nameShown    ( name + "_shown" );
  std::string nameDocked   ( name + "_docked" );
  std::string nameLocation ( name + "_location" );
  std::string nameX        ( name + "_x" );
  std::string nameY        ( name + "_y" );
  std::string nameHeight   ( name + "_height" );
  std::string nameWidth    ( name + "_width" );
  std::string nameLayout   ( name + "_layout" );
  std::string nameShell    ( name + "_shell" );

  //Read from the registry
  bool shown   ( ( reg.readIntEntry ( section, nameShown.c_str(),    1 ) ? true : false  ) );
  bool docked  ( ( reg.readIntEntry ( section, nameDocked.c_str(),   1 ) ? true : false  ) );
  int location ( reg.readIntEntry ( section, nameLocation.c_str(), Helios::Enums::TOP_DOCK ) );
  int x        ( reg.readIntEntry ( section, nameX.c_str(),        0 ) );
  int y        ( reg.readIntEntry ( section, nameY.c_str(),        0 ) );
  int height   ( reg.readIntEntry ( section, nameHeight.c_str(),   0 ) );
  int width    ( reg.readIntEntry ( section, nameWidth.c_str(),    0 ) );
  
  //Default Layout for the toolbar
  unsigned int defaultLayout ( FX::LAYOUT_SIDE_TOP | FX::FRAME_RAISED | FX::LAYOUT_DOCK_SAME );

  //If we haven't added one that's docked and shown...
  if( !_toolbarNext && docked && shown )
  {
    Usul::Bits::remove< unsigned int, unsigned int > ( defaultLayout, FX::LAYOUT_DOCK_SAME );
    defaultLayout |= FX::LAYOUT_DOCK_NEXT;
    _toolbarNext = true;
  }

  //get the toolbar layout
  unsigned int layout ( reg.readUnsignedEntry ( section, nameLayout.c_str(), defaultLayout ) );

  //Default Layout for the toolbar shell
  unsigned int defaultShellLayout ( FX::FRAME_RAISED );

  //Get the shell layout
  unsigned int shellLayout( reg.readUnsignedEntry ( section, nameShell.c_str(), defaultShellLayout ) );

  //Toolbas's docksite
  FX::FXDockSite *parent ( 0x0 );

  //Get the dock location
  if( Helios::Enums::TOP_DOCK == location )
    parent = FoxTools::ToolBar::Factory::instance().topDock()->dockSite();
  else if ( Helios::Enums::LEFT_DOCK == location )
    parent = FoxTools::ToolBar::Factory::instance().leftDock()->dockSite();
  else if ( Helios::Enums::BOTTOM_DOCK == location )
    parent = FoxTools::ToolBar::Factory::instance().bottomDock()->dockSite();
  else if ( Helios::Enums::RIGHT_DOCK == location )
    parent = FoxTools::ToolBar::Factory::instance().rightDock()->dockSite();
  else
    throw std::runtime_error ( "Error 3968942484: Dock site location is not valid." );

  //Make the toolbar
  FX::FXToolBarShell* toolShell ( new FX::FXToolBarShell ( this, shellLayout ) ); //, x, y, width, height ) );
  FX::FXToolBar* toolBar ( new FX::FXToolBar ( parent, toolShell, layout | FX::FRAME_RAISED, x, y, width, height ) );
  new FX::FXToolBarGrip ( toolBar, toolBar, FX::FXMenuBar::ID_TOOLBARGRIP, FX::TOOLBARGRIP_DOUBLE );

  //Add the toolbar to the map
  _toolbars[name] = toolBar;

  //Hide the toolbar if was hidding last
  if( !shown )
    toolBar->hide();

  //Undock the toolbar if was undocked last
  if( !docked )
  {
    parent->undockToolBar( toolBar );
    toolBar->undock( x, y );
  }
#endif
    (*i)->layout ( (*i)->layout() | layout | FX::LAYOUT_DOCK_SAME );
    (*i)->build( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear
//
///////////////////////////////////////////////////////////////////////////////

void DockSite::clear()
{
  // Call "clear" for every tool-bar
  std::for_each ( _bars.begin(), _bars.end(), std::mem_fun ( &Bar::clear ) );

  // Now empty the sequence.
  _bars.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide
//
///////////////////////////////////////////////////////////////////////////////

void DockSite::hide()
{
  _docksite->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show
//
///////////////////////////////////////////////////////////////////////////////

void DockSite::show()
{
  _docksite->show();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove toolbar with given name.
//
///////////////////////////////////////////////////////////////////////////////

void DockSite::remove ( const std::string& name )
{
  Bars::iterator i = std::find_if ( _bars.begin(), _bars.end(), Item::IsEqual ( name ) );

  if( i != _bars.end() )
    _bars.erase ( i );
}

