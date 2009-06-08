
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu interaction behavior.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_INTERACTION_BEHAVIOR_
#define _MENUKIT_INTERACTION_BEHAVIOR_

#include "MenuKit/Menu.h"

#include "Usul/Base/Object.h"

namespace MenuKit {

class Button;

class MENUKIT_EXPORT Behavior : public Usul::Base::Object
{
public:

  // Typedefs and smart-pointers.
  typedef Usul::Base::Object BaseClass;
  USUL_DECLARE_REF_POINTERS ( Behavior );

  // Possible directions.
  enum Direction { RIGHT, LEFT, UP, DOWN };

  // Options.
  enum
  {
    HIDE_ALL_IF_BUTTON_SELECTED = 0x00000001 // Default.
  };

  // Construction.
  Behavior();
  Behavior ( Menu *root );
  Behavior ( const Behavior & );
  Behavior &operator = ( const Behavior & );

  // Close the menu with focus.
  void            closeFocusedMenu();

  // Close the item. Does nothing if it is a button.
  static void     close ( Item *item );

  // If the focused item is null then set it to the root's first child.
  void            ensureFocus();

  // Move the item with focus.
  void            moveFocused ( Direction d );

  // Select the item with focus.
  void            selectFocused();

  // Set/get the menu root.
  const Menu *    root() const;
  Menu *          root();
  void            root ( Menu *menu );

  // Set/get the item with focus.
  const Item *    focus() const;
  Item *          focus();
  void            focus ( Item *item );

  // Set/get the flags.
  unsigned int    flags() const;
  void            flags ( unsigned int f );

protected:

  // Use reference counting.
  virtual ~Behavior();

  static Menu *   _findFirstParentWithHorizontalParent ( Item *item );

  static bool     _hasChildren ( Menu *menu );
  static bool     _hasSiblings ( Item *item );
  static bool     _hasHorizontalParent ( Item *item );

  void            _moveFocused ( Direction d, Button *button );
  void            _moveFocused ( Direction d, Menu *menu );
  void            _moveFocusedHorizontal ( Direction d, Menu *menu );
  void            _moveFocusedVertical   ( Direction d, Menu *menu );
  void            _moveLeft ( Item *item );
  void            _moveRightFromLeaf ( Item *item );

  void            _makeFirstChildFocused  ( Menu *menu );
  void            _makeLastChildFocused   ( Menu *menu );
  void            _makeNextSiblingFocused ( Item *item );
  void            _makePrevSiblingFocused ( Item *item );

  static void     _transferExpansionState ( Item *iold, Item *inew );

private:

  unsigned int _flags;
  Menu::RefPtr _root;
  Item::RefPtr _focus;
};


}; // namespace MenuKit


#endif // _MENUKIT_INTERACTION_BEHAVIOR_
