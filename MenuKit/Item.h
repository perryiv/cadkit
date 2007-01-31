
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu items.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_ITEM_
#define _MENUKIT_ITEM_

#include "MenuKit/Callback.h"
#include "MenuKit/Messages.h"

#include <string>
#include <map>


namespace MenuKit {

class Visitor;
class Menu;

class MENUKIT_EXPORT Item : public Referenced
{
public:

  // Typedefs and smart-pointers.
  typedef Referenced BaseClass;
  MENUKIT_DECLARE_POINTER ( Item );

  // Possible flags.
  enum
  {
    ENABLED     = 0x00000001, // Default. Without this it is 'greyed' out, unusable
    CHECKED     = 0x00000002, // fill in the toggle/radio
    TOGGLE      = 0x00000004, // button may be turned on/off
    RADIO       = 0x00000008, // button may be turned on/off but only one should be 'on'
    SEPARATOR   = 0x00000010, // could be used to draw separation lines
    EXPANDED    = 0x00000020, // flag for Menus, here for ease, opens/closes the menu
  };

  // Construction.
  Item();
  Item ( const std::string &name, const std::string &icon_file, Callback *cb, MenuKit::Message m );
  Item ( const std::string &name, const std::string &icon_file );
  Item ( const Item & );
  Item &operator = ( const Item & );

  // Accept the visitor.
  virtual void          accept ( Visitor & ) = 0;

  // Set/get the callback.
  void                  callback ( MenuKit::Message m, Callback::Ptr c );
  Callback *            callback ( MenuKit::Message m );

  // Remove all occurances of the callback from the collection.
  void                  removeAll ( Callback::Ptr c );

  // Ste/get the icon filename.
  void                  iconFile ( const std::string &f ) { _iconFile = f; }
  const std::string &   iconFile() const { return _iconFile; }

  // Set/get the text string.
  void                  text ( const std::string &t ) { _text = t; }
  const std::string &   text() const { return _text; }

  // Set/get the information string.
  void                  info ( const std::string &s ) { _info = s; }
  const std::string &   info() const { return _info; }

  // Get the parent.
  const Menu *          parent() const { return _parent; }
  Menu *                parent()       { return _parent; }

  // Set/get the flags.
  unsigned int          flags() const;
  void                  flags ( unsigned int f );

  // Set/get the enabled flag.
  bool                  enabled() const;
  void                  enabled ( bool e );

  // Set/get the checked flag.
  bool                  checked() const;
  void                  checked ( bool e );

  // Set/get the toggle flag.
  bool                  toggle() const;
  void                  toggle ( bool e );

  // Set/get the radio flag.
  bool                  radio() const;
  void                  radio ( bool e );

  // Set/get the expansion flag.
  bool                  expanded() const;
  void                  expanded ( bool e );

  // Set/get the separator flag.
  bool                  separator() const;
  void                  separator ( bool e );

  // Send a message to the client.
  void                  sendMessage ( MenuKit::Message m );

protected:

  typedef std::map<MenuKit::Message,Callback::Ptr> Callbacks;
  friend class Menu;

  // Use reference counting.
  virtual ~Item();

  // Set the parent.
  void                  _setParent ( Menu *m ) { _parent = m; }

private:

  Callbacks _callbacks;
  std::string _text;
  std::string _info;
  std::string _iconFile;
  Menu *_parent;
  unsigned int _flags;
};


}; // namespace MenuKit


#endif // _MENUKIT_ITEM_
