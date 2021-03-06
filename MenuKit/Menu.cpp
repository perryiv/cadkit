
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

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/Visitor.h"
#include "MenuKit/Errors.h"
#include "MenuKit/Separator.h"

#include "Usul/Strings/Trim.h"
#include "Usul/Threads/Safe.h"

#include <algorithm>

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu(): BaseClass(), 
  _text   (),
  _items  (), 
  _layout ( VERTICAL )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu ( const std::string &text, Layout layout ) :
  BaseClass(),
  _text   ( text ),
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
  _text   (),
  _items  (), 
  _layout ( VERTICAL )
{
  Guard guard ( &m );
  _text   = m._text;
  _items  = m._items;
  _layout = m._layout;
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

  Guard guard1 ( this );
  Guard guard2 ( &m );

  _text   = m._text;
  _items  = m._items;
  _layout = m._layout;

  return (*this);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::clear()
{
  Guard guard ( this );
  _items.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the item.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::append ( Item *item )
{
  if ( 0x0 != item )
  {
    item->_setParent ( this );
    {
      Guard guard ( this );
      _items.push_back ( item );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend the item.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::prepend ( Item *item )
{
  if ( 0x0 != item )
  {
    item->_setParent ( this );
    {
      Guard guard ( this );
      _items.insert ( _items.begin(), item );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the item.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::insert ( iterator iter, Item *item )
{
  if ( 0x0 != item )
  {
    item->_setParent ( this );
    {
      Guard guard ( this );
      _items.insert ( iter, item );
    }
  }
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
  switch ( v.mode() )
  {
    case Visitor::NONE:
    {
      break;
    }
    case Visitor::ALL:
    {
      Menu::Items items ( Usul::Threads::Safe::get ( this->mutex(), _items ) );
      for ( Menu::iterator iter = items.begin(); iter != items.end(); ++iter )
      {
        (*iter)->accept ( v );
      }
      break;
    }
    case Visitor::EXPANDED:
    default:
    {
      if ( this->expanded() )
      {
        Menu::Items items ( Usul::Threads::Safe::get ( this->mutex(), _items ) );
        for ( Menu::iterator iter = items.begin(); iter != items.end(); ++iter )
        {
          (*iter)->accept ( v );
        }
      }
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a separator to the end.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::addSeparator()
{
  this->append ( new MenuKit::Separator );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate to find menu.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct FindMenu
  {
    FindMenu ( const std::string& name ) : _name ( name )
    {
      Usul::Strings::trimLeft ( _name, '&' );
    }

    template < class T >
    bool operator() ( const T& t ) const
    {
      if ( Menu* m = dynamic_cast < Menu* > ( t.get() ) )
      {
        std::string name ( m->text() );
        Usul::Strings::trimLeft ( name, '&' );
        return ( _name == name );
      }
      return false;
    }

  private:
    std::string _name;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the menu. Create it if we should.
//
///////////////////////////////////////////////////////////////////////////////

Menu* Menu::find ( const std::string& name, bool create )
{
  // Try to find the item with the name.
  Menu::Items items ( Usul::Threads::Safe::get ( this->mutex(), _items ) );
  iterator iter = std::find_if ( items.begin(), items.end(), Detail::FindMenu ( name ) );
  if ( items.end() != iter )
  {
    return dynamic_cast < Menu* > ( iter->get() );
  }

  // Initialize.
  Menu::RefPtr menu ( 0x0 );

  // If we get to here then create one if we should.
  if ( true == create )
  {
    menu = new Menu;
    menu->text ( name );
    this->append ( menu );
  }

  // Return menu (which may be null).
  return menu.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text string.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::text ( const std::string &t )
{
  Guard guard ( this );
  _text = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the text string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Menu::text ( char strip ) const
{
  Guard guard ( this );
  return _text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the menu empty?
//
///////////////////////////////////////////////////////////////////////////////

bool Menu::empty() const
{
  Guard guard ( this );
  return _items.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of items.
//
///////////////////////////////////////////////////////////////////////////////

Menu::size_type Menu::numItems() const
{
  Guard guard ( this );
  return _items.size();
}
