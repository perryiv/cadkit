
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

#include "MenuKit/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Commands/Command.h"

#include <string>
#include <map>


namespace MenuKit {

class Visitor;
class Menu;

class MENUKIT_EXPORT Item : public Usul::Base::Object
{
public:

  // Typedefs and smart-pointers.
  typedef Usul::Base::Object       BaseClass;
  typedef Usul::Commands::Command  Command;
  USUL_DECLARE_REF_POINTERS ( Item );

  // Possible flags.
  enum
  {
    ENABLED     = 0x00000001, // Default. Without this it is 'greyed' out, unusable
    CHECKED     = 0x00000002, // fill in the toggle/radio
    TOGGLE      = 0x00000004, // button may be turned on/off
    RADIO       = 0x00000008, // button may be turned on/off but only one should be 'on'
    SEPARATOR   = 0x00000010, // could be used to draw separation lines
    EXPANDED    = 0x00000020, // flag for Menus, here for ease, opens/closes the menu
    MARKED      = 0x00000040, // flag shows an item is 'current' or should be 'highlighted'
  };

  // Construction.
  Item();
  Item ( const std::string &name, const std::string &icon_file );
  Item ( const Item & );
  Item &operator = ( const Item & );

  // Accept the visitor.
  virtual void          accept ( Visitor & ) = 0;

  /// Get/Set the command.
  void                  command ( Command * command );
  Command *             command ();
  const Command *       command () const;

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

  // Set/get the current flag.
  bool                  marked() const;
  void                  marked ( bool e );

protected:

  friend class Menu;

  // Use reference counting.
  virtual ~Item();

  // Set the parent.
  void                  _setParent ( Menu *m ) { _parent = m; }

private:

  Command::RefPtr _command;
  std::string _text;
  std::string _info;
  std::string _iconFile;
  Menu *_parent;
  unsigned int _flags;
};


} // namespace MenuKit


#endif // _MENUKIT_ITEM_
