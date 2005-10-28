
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu group class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Menus/MenuGroup.h"

#include "usul/Exceptions/Thrower.h"

using namespace AFW::Menus;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuGroup::MenuGroup() : BaseClass(),
  _items()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuGroup::MenuGroup ( const std::string &text, unsigned short underline ) : BaseClass ( text, underline ),
  _items()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuGroup::~MenuGroup()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the item.
//
///////////////////////////////////////////////////////////////////////////////

void MenuGroup::append ( MenuItem *item )
{
  // Ignore invalid items.
  if ( 0x0 == item )
    return;

  // Check for parent.
  if ( 0x0 != item->parent() )
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 2607246966: menu item '", this->text(), "' already has a parent" );

  // Append to our list.
  _items.push_back ( item );

  // Set the parent.
  item->_setParent ( this );

  // Set dirty flags.
  item->dirty ( true );
  this->dirty ( true );
}
