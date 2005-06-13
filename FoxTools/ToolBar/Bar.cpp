
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

Bar::Bar ( FX::FXComposite *dockedSite, FX::FXComposite *undockedSite ) : BaseClass(), 
  _commands     (),
  _bar          ( 0x0 ),
  _dockedSite   ( dockedSite ),
  _undockedSite ( undockedSite )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Bar::Bar ( const std::string& name, FX::FXComposite *dockedSite, FX::FXComposite *undockedSite ) : BaseClass( name ), 
  _commands     (),
  _bar          ( 0x0 ),
  _dockedSite   ( dockedSite ),
  _undockedSite ( undockedSite )
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

void Bar::build( bool shown, unsigned int layout, int x, int y, int width, int height )
{
  USUL_ASSERT ( _dockedSite );
  USUL_ASSERT ( _undockedSite );

  // Make sure we have a toolbar bar.
  if ( !_bar )
  {
    _bar = new FX::FXToolBar ( _dockedSite, _undockedSite, layout, x, y, width, height );
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
  // Create the menu-bar if we can.
  if ( _bar && !_bar->id() )
  {
    _bar->create();
    _bar->setUserData( this );
  }

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
