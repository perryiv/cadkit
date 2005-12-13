
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/Headers/ToolBarGrip.h"
#include "FoxTools/Headers/MenuBar.h"

#include "Usul/Errors/Assert.h"

#include <algorithm>
#include <functional>

using namespace FoxTools::ToolBar;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Bar::Bar ( FX::FXComposite *dockedSite, FX::FXComposite *undockedSite ) : 
  BaseClass(), 
  _commands     (),
  _dockedSite   ( dockedSite ),
  _undockedSite ( undockedSite ),
  _bar( 0x0 ),
  _layout       ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Bar::Bar ( const std::string& name, FX::FXComposite *dockedSite, FX::FXComposite *undockedSite ) : BaseClass( name ), 
  _commands     (),
  _dockedSite   ( dockedSite ),
  _undockedSite ( undockedSite ),
  _bar          ( 0x0 ),
  _layout       ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Bar::~Bar()
{
  // Call "clear" for every group before we delete the FOX menu-bar.
  this->clear();

  if( _bar )
    delete _bar;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::clear()
{
#if 0
  // Save each toolbar's information
  for ( Toolbars::const_iterator i = _toolbars.begin(); i != _toolbars.end(); ++ i )
  {
    std::string name         ( i->first );
    std::string nameShown    ( name + "_shown" );
    std::string nameDocked   ( name + "_docked" );
    std::string nameLocation ( name + "_location" );
    std::string nameX        ( name + "_x" );
    std::string nameY        ( name + "_y" );
    std::string nameHeight   ( name + "_height" );
    std::string nameWidth    ( name + "_width" ); 
    std::string nameLayout   ( name + "_layout" );
    std::string nameShell    ( name + "_shell" );

    // for convience
    FX::FXToolBar *toolBar ( i->second );

    // Is the toolbar currently shown?
    int shown ( 0 );
    if( toolBar->shown() )
      shown = 1;

    // Write whether or not the toolbar is shown
    reg.writeIntEntry ( section, nameShown.c_str(), shown );
    
    int docked ( 1 );
    int x ( toolBar->getX() );
    int y ( toolBar->getY() );
    
    // If the toolbar currently not docked...
    if( !toolBar->isDocked() )
    {
      docked = 0;
      x = toolBar->getParent()->getX();
      y = toolBar->getParent()->getY();

      unsigned int shellLayout ( toolBar->getParent()->getLayoutHints() );
      reg.writeUnsignedEntry( section, nameShell.c_str(), shellLayout );
    }
    else
    {
      // Find what dockbar it's on...
      FX::FXWindow *dock ( toolBar->getParent() );

      int location ( OsgFox::Enums::TOP_DOCK );

      typedef FoxTools::ToolBar::Factory Factory;

      if ( Factory::instance().leftDock()->dockSite() == dock )
        location = OsgFox::Enums::LEFT_DOCK;
      else if ( Factory::instance().rightDock()->dockSite() == dock )
        location = OsgFox::Enums::RIGHT_DOCK;
      else if ( Factory::instance().bottomDock()->dockSite() == dock )
        location = OsgFox::Enums::BOTTOM_DOCK;

      //Write it's loaction
      reg.writeIntEntry ( section, nameLocation.c_str(), location );
    }
    
    //Write whether or not the toolbar is docked
    reg.writeIntEntry( section, nameDocked.c_str(), docked );

    //Write the current X position
    reg.writeIntEntry ( section, nameX.c_str(), x );

    //Write the current Y position
    reg.writeIntEntry ( section, nameY.c_str(), y );

    //Get height and width
    int height ( toolBar->getHeight() );
    int width ( toolBar->getWidth() );

    //Write the current Heigth position
    reg.writeIntEntry ( section, nameHeight.c_str(), height );

    //Write the current Width position
    reg.writeIntEntry ( section, nameWidth.c_str(), width );

    //Write the layout
    unsigned int layout ( toolBar->getLayoutHints() );
    reg.writeUnsignedEntry( section, nameLayout.c_str(), layout );

  }
#endif
  // Call "clear" for every group before we delete the FOX menu-bar.
  std::for_each ( _commands.begin(), _commands.end(), std::mem_fun ( &Command::clear ) );

  // Now empty the sequence.
  _commands.clear();

  // Delete the toolbar
  if( _bar )
    delete _bar;
  _bar = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tool-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::build ( bool shown, int x, int y, int width, int height )
{
  USUL_ASSERT ( _dockedSite );
  USUL_ASSERT ( _undockedSite );

  // Make sure we have a toolbar bar.
  if ( !_bar )
  {
    // Use internal layout if it exists.
    _bar = new FX::FXToolBar ( _dockedSite, _undockedSite, _layout, x, y, width, height );
    new FX::FXToolBarGrip ( _bar, _bar, FX::FXMenuBar::ID_TOOLBARGRIP, FX::TOOLBARGRIP_DOUBLE );
  }

  // Build every command.
  for ( Commands::iterator i = _commands.begin(); i != _commands.end(); ++i )
    (*i)->_build ( _bar );

  shown ? _bar->show() : _bar->hide();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the command.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::append ( Command *c )
{
  _commands.push_back ( c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the windows.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::create()
{
  // Create the tool-bar if we can.
  if ( _bar && !_bar->id() )
  {
    _bar->create();
    _bar->setUserData( this );
  }

  // Create the undocked site if we can.
  if( _undockedSite && !_undockedSite->id( ) )
    _undockedSite->create();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Find command by name.
//
///////////////////////////////////////////////////////////////////////////////

Bar::Command* Bar::find ( const std::string& name )
{
  Item::IsEqual equal ( name );

  Commands::iterator i = std::find_if ( _commands.begin(), _commands.end(), equal );

  return ( i == _commands.end() ? 0x0 : i->get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this toolbar showing
//
///////////////////////////////////////////////////////////////////////////////

bool Bar::shown() const
{
  if( _bar->isDocked() )
    return ( _bar->shown() ? true : false );
  
  return ( _bar->getParent()->shown() ? true : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show
//
///////////////////////////////////////////////////////////////////////////////

void Bar::show()
{
  //If the toolbar is docked then we want to hide the toolbar
  //If the toolbar is not docked then we want to hide it's parent
  FX::FXWindow* window ( ( _bar->isDocked() ) ? _bar : _bar->getParent() );

  window->show();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide
//
///////////////////////////////////////////////////////////////////////////////

void Bar::hide()
{
  //If the toolbar is docked then we want to hide the toolbar
  //If the toolbar is not docked then we want to hide it's parent
  FX::FXWindow* window ( ( _bar->isDocked() ) ? _bar : _bar->getParent() );

  window->hide();
}


void Bar::layout ( unsigned int lo ) 
{ 
  _layout = lo; 
}

unsigned int Bar::layout () const
{ 
  return _layout; 
}

