
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

#ifndef _MENUKIT_MENU_
#define _MENUKIT_MENU_

#include "MenuKit/Item.h"

#include <vector>


namespace MenuKit {

class Visitor;

class MENUKIT_EXPORT Menu : public Item
{
public:

  // Typedefs and smart-pointers.
  typedef Item                              BaseClass;
  typedef std::vector<Item::RefPtr>         Items;
  typedef Items::iterator                   iterator;
  typedef Items::const_iterator             const_iterator;
  typedef Items::reverse_iterator           reverse_iterator;
  typedef Items::const_reverse_iterator     const_reverse_iterator;
  typedef Items::size_type                  size_type;
  USUL_DECLARE_REF_POINTERS ( Menu );

  // Possible layout.
  enum Layout { VERTICAL, HORIZONTAL };

  // Construction.
  Menu();
  Menu ( const std::string &name, Layout layout = VERTICAL );
  Menu ( const Menu & );

  // Assignment.
  Menu& operator = ( const Menu & );

  // Clear the menu.
  void                      clear();

  // Get the children.
  const Items &             items() const { return(_items); }

  // Iterators.
  iterator                  begin() { return ( _items.begin() ); }
  iterator                  end()   { return ( _items.end() ); }
  const_iterator            begin() const { return ( _items.begin() ); }
  const_iterator            end()   const { return ( _items.end() ); }

  // Reverse iterators.
  reverse_iterator          rbegin() { return ( _items.rbegin() ); }
  reverse_iterator          rend()   { return ( _items.rend() ); }
  const_reverse_iterator    rbegin() const { return ( _items.rbegin() ); }
  const_reverse_iterator    rend()   const { return ( _items.rend() ); }

  // Is the menu empty?
  bool                      empty() const;

  // Get the number of items.
  size_type                 numItems() const;

  // Add the items.
  void                      append ( Item * );
  void                      prepend ( Item * );
  void                      insert ( iterator, Item * );

  // Find the menu. Create it if we should.
  Menu*                     find ( const std::string& name, bool create );

  // Set/get the layout.
  void                      layout ( Layout l ) { _layout = l; }
  Layout                    layout() const { return _layout; }

  // Accept the visitor.
  virtual void              accept ( Visitor & );

  // Traverse the children.
  virtual void              traverse ( Visitor & );

  // Add a separator to the end.
  void                      addSeparator();

  // Set/get the text string.
  void                      text ( const std::string &t );
  std::string               text ( char strip = '\0' ) const;

protected:

  // Use reference counting.
  virtual ~Menu();

private:

  std::string _text;
  Items _items;
  Layout _layout;
};


}; // namespace MenuKit


#endif // _MENUKIT_MENU_
