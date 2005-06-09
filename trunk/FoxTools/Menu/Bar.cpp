
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu-bar class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Menu/Bar.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Headers/MenuBar.h"
#include "FoxTools/Headers/ToolBarGrip.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/MDIButton.h"
#include "FoxTools/Headers/MDIChild.h"

#include "Usul/Errors/Assert.h"

#include <algorithm>
#include <functional>

using namespace FoxTools;
using namespace FoxTools::Menu;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Bar::Bar ( FX::FXComposite *dockedSite, FX::FXComposite *undockedSite ) : BaseClass(), 
  _groups       (),
  _bar          ( 0x0 ),
  _dockedSite   ( dockedSite ),
  _undockedSite ( undockedSite ),
  _clientArea   ( 0x0 ),
  _mdiMenu      ( 0x0 )
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
  std::for_each ( _groups.begin(), _groups.end(), std::mem_fun ( &Group::clear ) );

  // Delete the menu bar.
  this->_deleteMenuBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::clear()
{
  // Call "clear" for every group before we delete the FOX menu-bar.
  std::for_each ( _groups.begin(), _groups.end(), std::mem_fun ( &Group::clear ) );

  // Now empty the sequence.
  _groups.clear();

  // Delete the menu bar.
  this->_deleteMenuBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::_deleteMenuBar()
{
  delete _bar;
  _bar = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::build()
{
  USUL_ASSERT ( _dockedSite );
  USUL_ASSERT ( _undockedSite );

  // Make sure we have a menu bar.
  if ( !_bar )
  {
    _bar = new FX::FXMenuBar ( _dockedSite, _undockedSite, FX::LAYOUT_SIDE_TOP | FX::LAYOUT_FILL_X | FX::FRAME_RAISED );
    new FX::FXToolBarGrip ( _bar, _bar, FX::FXMenuBar::ID_TOOLBARGRIP, FX::TOOLBARGRIP_DOUBLE );
  }

  // Build every group.
  for ( Groups::iterator i = _groups.begin(); i != _groups.end(); ++i )
    (*i)->_build ( FoxTools::Functions::mainWindow(), _bar );

  if( _mdiMenu && _clientArea )
  {
    // MDI buttons in menu.
    new FX::FXMDIWindowButton   ( _bar, _mdiMenu, _clientArea, FXMDIClient::ID_MDI_MENUWINDOW, LAYOUT_LEFT );
    new FX::FXMDIDeleteButton   ( _bar, _clientArea, FXMDIClient::ID_MDI_MENUCLOSE,    FRAME_RAISED | LAYOUT_RIGHT );
    new FX::FXMDIRestoreButton  ( _bar, _clientArea, FXMDIClient::ID_MDI_MENURESTORE,  FRAME_RAISED | LAYOUT_RIGHT );
    new FX::FXMDIMinimizeButton ( _bar, _clientArea, FXMDIClient::ID_MDI_MENUMINIMIZE, FRAME_RAISED | LAYOUT_RIGHT );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::purge ( unsigned int token )
{
  // Purge all the groups.
  std::for_each ( _groups.begin(), _groups.end(), std::bind2nd ( std::mem_fun ( &Group::purge ), token ) );

  // Remove empty groups.
  Groups::iterator end ( std::remove_if ( _groups.begin(), _groups.end(), std::mem_fun ( &Group::empty ) ) );
  _groups.erase ( end, _groups.end() );

  // Remove the menu-bar if empty.
  if ( this->empty() )
    this->_deleteMenuBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the menu bar empty?
//
///////////////////////////////////////////////////////////////////////////////

bool Bar::empty() const
{
  return _groups.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the size of the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Bar::size() const
{
  return _groups.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the group.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::append ( Group *g )
{
  _groups.push_back ( g );
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



  // Create all the groups.
  std::for_each ( _groups.begin(), _groups.end(), std::mem_fun ( &Group::create ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert group before given group name.
//
///////////////////////////////////////////////////////////////////////////////

void Bar::insert ( const std::string& name, Group *group )
{
  Item::IsEqual equal ( name );

  Groups::iterator i = std::find_if ( _groups.begin(), _groups.end(), equal );

  _groups.insert( i, group );
}
