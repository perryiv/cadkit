
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Menu.h"
#include "Button.h"
#include "Visitor.h"
#include "Errors.h"

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu(): BaseClass(), 
  _items  (), 
  _layout ( VERTICAL )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu ( const std::string &name, const std::string &icon_file, Layout layout ) :
  BaseClass ( name, icon_file ),
  _items  (), 
  _layout ( layout )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu ( const Menu &m ): BaseClass ( m ),
  _items  ( m._items ), 
  _layout ( m._layout )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::~Menu()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Menu& Menu::operator = ( const Menu &m )
{
  BaseClass::operator = ( m );
  _items  = m._items;
  _layout = m._layout;

  return (*this);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the item.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::append ( Item *item )
{
  MENUKIT_CHECK_POINTER      ( 3094399297u, item );
  MENUKIT_ENSURE_NULL_PARENT ( 1547689565u, item );
  item->_setParent ( this );
  _items.push_back ( item );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend the item.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::prepend ( Item *item )
{
  MENUKIT_CHECK_POINTER      ( 3117610494u, item );
  MENUKIT_ENSURE_NULL_PARENT ( 2578476072u, item );
  item->_setParent ( this );
  _items.insert ( _items.begin(), item );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the item.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::insert ( iterator iter, Item *item )
{
  MENUKIT_CHECK_POINTER      ( 3953040935u, item );
  MENUKIT_ENSURE_NULL_PARENT ( 2734640082u, item );
  item->_setParent ( this );
  _items.insert ( iter, item );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::accept ( Visitor &v )
{
  v.apply ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the tree.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::traverse ( Visitor &v )
{
  for ( Menu::iterator iter = _items.begin(); iter != _items.end(); ++iter )
    (*iter)->accept ( v );
}
