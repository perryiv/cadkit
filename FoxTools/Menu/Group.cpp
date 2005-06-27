
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Menu/Group.h"
#include "FoxTools/Items/Command.h"
#include "FoxTools/Headers/MenuPane.h"
#include "FoxTools/Headers/MenuTitle.h"
#include "FoxTools/Headers/MenuCascade.h"
#include "FoxTools/Headers/MenuBar.h"
#include "FoxTools/Headers/ScrollPane.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Functions/MainWindow.h"

#include "Usul/Cast/Cast.h"
#include "Usul/Errors/Assert.h"

#include <algorithm>
#include <functional>

using namespace FoxTools;
using namespace FoxTools::Menu;

// Typedefs
typedef FoxTools::Items::Command Command;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group() : BaseClass(), 
  _items    (),
  _pane     ( 0x0 ),
  _caption  ( 0x0 ),
  _scroll   ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group ( const std::string &n ) : BaseClass ( n ), 
  _items    (),
  _pane     ( 0x0 ),
  _caption  ( 0x0 ),
  _scroll   ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
  // Call "clear" for every item before we delete the FOX menu.
  std::for_each ( _items.begin(), _items.end(), std::mem_fun ( &Item::clear ) );

  // Delete the windows.
  this->_deleteWindows();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Group::clear()
{
  // Call "clear" for every item before we delete the FOX menu.
  std::for_each ( _items.begin(), _items.end(), std::mem_fun ( &Command::clear ) );

  // Now empty the sequence.
  _items.clear();

  // Delete the windows.
  this->_deleteWindows();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the windows.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_deleteWindows()
{
  if( _pane )
    delete _pane;
  _pane = 0x0;
  if( _caption )
    delete _caption;
  _caption = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_build ( FX::FXWindow *paneParent, FX::FXComposite *captionParent )
{
  // Make the pane if we should.
  if ( !_pane )
  {
    // Make it scroll if we should.
    if ( this->scroll() )
      _pane = new FX::FXScrollPane ( paneParent, 10, FX::PACK_UNIFORM_HEIGHT );
    else
      _pane = new FX::FXMenuPane ( paneParent );
  }

  // If there is no caption...
  if ( !_caption  )
  {
    // Try to cast the pointers.
    FX::FXMenuPane *pane ( SAFE_CAST_FOX ( FX::FXMenuPane, captionParent ) );
    FX::FXMenuBar * bar  ( SAFE_CAST_FOX ( FX::FXMenuBar,  captionParent ) );

    // If we were given a pane then make a cascade.
    if ( pane )
      _caption = new FX::FXMenuCascade ( pane, this->name().c_str(), 0x0, _pane );

    // Or, if we were given a menu-bar, make a title.
    else if ( bar )
      _caption = new FX::FXMenuTitle ( bar, this->name().c_str(), 0x0, _pane );

    // Otherwise, punt.
    else
      throw std::invalid_argument ( "Error 3554345192: caption parent is not a pane or menu bar" );
  }

  // Loop through the items.
  for ( Items::iterator i = _items.begin(); i != _items.end(); ++i )
  {
    // Cast.
    Command::RefPtr command ( dynamic_cast < Command * > ( i->get() ) );
    Group::RefPtr   group   ( dynamic_cast < Group *   > ( i->get() ) );

    // If we have a command...
    if ( command.valid() )
      command->_build ( _pane );

    // Or, if we have a group...
    else if ( group.valid() )
      group->_build ( paneParent, _pane );

    // Otherwise, punt.
    else
      throw std::runtime_error ( "Error 3523358927: menu item is not a command or group" );
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functor to test for token.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct HasToken
  {
    typedef FoxTools::Items::Command MenuCommand;
    HasToken ( unsigned int token ) : _token ( token ){}
    template < class ItemPtr > bool operator () ( ItemPtr item )
    {
      MenuCommand::RefPtr command ( dynamic_cast < MenuCommand * > ( item.get() ) );
      return ( ( command.valid() ) ? ( command->token() == _token ) : false );
    }
  private:
    unsigned int _token;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functor to test for empty group.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct IsEmpty
  {
    typedef FoxTools::Menu::Group MenuGroup;
    IsEmpty(){}
    template < class ItemPtr > bool operator () ( ItemPtr item )
    {
      MenuGroup::RefPtr group ( dynamic_cast < MenuGroup * > ( item.get() ) );
      return ( ( group.valid() ) ? group->empty() : false );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge the group.
//
///////////////////////////////////////////////////////////////////////////////

void Group::purge ( unsigned int token )
{
  // Remove items that have given token.
  Items::iterator end ( std::remove_if ( _items.begin(), _items.end(), Detail::HasToken ( token ) ) );
  _items.erase ( end, _items.end() );

  // Purge items that are groups.
  for ( Items::iterator i = _items.begin(); i != _items.end(); ++i )
  {
    Group::RefPtr group ( dynamic_cast < Group * > ( i->get() ) );
    if ( group.valid() )
      group->purge ( token );
  }

  // Remove items that are empty groups.
  end = std::remove_if ( _items.begin(), _items.end(), Detail::IsEmpty() );
  _items.erase ( end, _items.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group empty?
//
///////////////////////////////////////////////////////////////////////////////

bool Group::empty() const
{
  return _items.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the size of the group.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Group::size() const
{
  return _items.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the item.
//
///////////////////////////////////////////////////////////////////////////////

void Group::append ( Item *item )
{
  _items.push_back ( item );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the windows.
//
///////////////////////////////////////////////////////////////////////////////

void Group::create()
{
  // Create the pane if we can.
  if ( _pane && !_pane->id() )
    _pane->create();

  // Create the caption if we can.
  if ( _caption && !_caption->id() )
    _caption->create();

  // Loop through the items.
  for ( Items::iterator i = _items.begin(); i != _items.end(); ++i )
  {
    // Cast.
    Command::RefPtr command ( dynamic_cast < Command * > ( i->get() ) );
    Group::RefPtr   group   ( dynamic_cast < Group *   > ( i->get() ) );

    // If we have a command...
    if ( command.valid() )
      command->_create ();

    // Or, if we have a group...
    else if ( group.valid() )
      group->create();

    // Otherwise, punt.
    else
      throw std::runtime_error ( "Error 3523358927: menu item is not a command or group" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scroll flag.
//
///////////////////////////////////////////////////////////////////////////////

void Group::scroll ( bool s )
{
  _scroll = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scroll flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Group::scroll() const
{
  return _scroll;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the group.  Does nothing if not already built.
//
///////////////////////////////////////////////////////////////////////////////

void Group::rebuild()
{
  // Return now if no pane.
  if( 0x0 == _pane )
    return;

  // Loop through the items.
  for ( Items::iterator i = _items.begin(); i != _items.end(); ++i )
  {
    // Cast.
    Command::RefPtr command ( dynamic_cast < Command * > ( i->get() ) );
    Group::RefPtr   group   ( dynamic_cast < Group *   > ( i->get() ) );

    // If we have a command...
    if ( command.valid() )
      command->_build ( _pane );

    // Or, if we have a group...
    else if ( group.valid() )
      group->_build( FoxTools::Functions::mainWindow(), _pane);

    // Otherwise, punt.
    else
      throw std::runtime_error ( "Error 3523358927: menu item is not a command or group" );
  }
}

