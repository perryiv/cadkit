
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

#include "Behavior.h"
#include "Menu.h"
#include "Button.h"
#include "Errors.h"
#include "FlagsVisitor.h"
#include "Algorithms.h"

#include "Usul/Bits/Bits.h"

#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace MenuKit;

///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Behavior::Behavior() : BaseClass(),
  _flags ( HIDE_ALL_IF_BUTTON_SELECTED ),
  _root  (), 
  _focus ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Behavior::Behavior ( Menu *root ) : BaseClass(),
  _flags ( HIDE_ALL_IF_BUTTON_SELECTED ),
  _root  ( root ), 
  _focus ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Behavior::Behavior ( const Behavior &b ): BaseClass ( b ),
  _flags ( b._flags  ),
  _root  ( b._root   ), 
  _focus ( b._focus )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Behavior& Behavior::operator = ( const Behavior &b )
{
  BaseClass::operator = ( b );
  _flags = b._flags;
  _root  = b._root;
  _focus = b._focus;
  return ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Behavior::~Behavior()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

const Menu *Behavior::root() const
{
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the root.
//
///////////////////////////////////////////////////////////////////////////////

Menu *Behavior::root()
{
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the root.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::root ( Menu *menu )
{
  _root = menu;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the item with focus.
//
///////////////////////////////////////////////////////////////////////////////

const Item *Behavior::focus() const
{
  return _focus.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the item with focus.
//
///////////////////////////////////////////////////////////////////////////////

Item *Behavior::focus()
{
  return _focus.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the item with focus. It cannot be the root.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::focus ( Item *item )
{
  // Set the new item.
  _focus = ( this->root() == item ) ? 0x0 : item;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Behavior::flags() const
{
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::flags ( unsigned int f )
{
  _flags = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the item with focus.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::moveFocused ( Direction d )
{
  MENUKIT_CHECK_POINTER ( 3203295262u, this->root() );

  // Get the item with focus.
  Item::RefPtr focus ( this->focus() );

  // Handle specific case.
  if ( 0x0 == focus.get() )
  {
    // Make sure we have an focused item.
    this->ensureFocus();
    return;
  }

  _focus->marked( false );  // remove the marking

  // -- change the focus item -- //
  // See if the focused item is a menu.
  Menu::RefPtr menu ( dynamic_cast < Menu * > ( focus.get() ) );
  if ( menu.get() )
  {
    _moveFocused ( d, menu.get() );
    _focus->marked( true );  // add the marking to the new focus
    return;
  }

  // See if the focused item is a button.
  Button::RefPtr button ( dynamic_cast < Button * > ( focus.get() ) );
  if ( button.get() )
  {
    _moveFocused ( d, button.get() );
    _focus->marked( true );  // add the marking to the new focus
    return;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the next item focused.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_moveFocused ( Direction d, Menu *menu )
{
  MENUKIT_CHECK_POINTER ( 2531740683u, menu );
  switch ( menu->layout() )
  {
  case Menu::HORIZONTAL:
    this->_moveFocusedHorizontal ( d, menu );
    break;
  case Menu::VERTICAL:
    this->_moveFocusedVertical ( d, menu );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the next item focused.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_moveFocusedHorizontal ( Direction d, Menu *menu )
{
  MENUKIT_CHECK_POINTER ( 1367710191u, menu );

  switch ( d )
  {
    case DOWN:

      if ( this->_hasSiblings ( menu ) )
        this->_makeNextSiblingFocused ( menu );
      else
        this->_makeFirstChildFocused ( menu );
      break;

    case UP:

      if ( this->_hasSiblings ( menu ) )
        this->_makePrevSiblingFocused ( menu );
      else
        this->_makeFirstChildFocused ( menu );
      break;

    case RIGHT:

      if ( this->_hasChildren ( menu ) )
        this->_makeFirstChildFocused ( menu );
      else
        this->_moveRightFromLeaf ( menu );
      break;

    case LEFT:

      this->_moveLeft ( menu );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the next item focused.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_moveFocusedVertical ( Direction d, Menu *menu )
{
  MENUKIT_CHECK_POINTER ( 1891812190u, menu );

  switch ( d )
  {
    case DOWN:

      if ( this->_hasHorizontalParent ( menu ) )
        this->_makeFirstChildFocused ( menu );
      else
        this->_makeNextSiblingFocused ( menu );
      break;

    case UP:

      if ( this->_hasSiblings ( menu ) )
        this->_makePrevSiblingFocused ( menu );
      else
        this->_makeLastChildFocused ( menu );
      break;

    case RIGHT:

      if ( this->_hasHorizontalParent ( menu ) )
        this->_makeNextSiblingFocused ( menu );
      else if ( this->_hasChildren ( menu ) )
        this->_makeFirstChildFocused ( menu );
      else
        this->_moveRightFromLeaf ( menu );
      break;

    case LEFT:

      this->_moveLeft ( menu );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the next item focused.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_moveFocused ( Direction d, Button *button )
{
  MENUKIT_CHECK_POINTER ( 4103117035u, button );

  switch ( d )
  {
    case DOWN:

      if ( button->parent() && this->_hasHorizontalParent ( button ) )
        this->_makeNextSiblingFocused ( button->parent() );
      else
        this->_makeNextSiblingFocused ( button );
      break;

    case UP:

      if ( button->parent() && this->_hasHorizontalParent ( button ) )
        this->_makePrevSiblingFocused ( button->parent() );
      else
        this->_makePrevSiblingFocused ( button );
      break;

    case RIGHT:

      if ( this->_hasHorizontalParent ( button ) )
        this->_makeNextSiblingFocused ( button );
      else
        this->_moveRightFromLeaf ( button );
      break;

    case LEFT:

      this->_moveLeft ( button );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move to the right assuming the given item is a leaf.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_moveRightFromLeaf ( Item *item )
{
  MENUKIT_CHECK_POINTER ( 4208315504u, item );

  Menu::RefPtr menu ( this->_findFirstParentWithHorizontalParent ( item ) );
  if ( menu.get() )
  {
    this->_makeNextSiblingFocused ( menu.get() );
    menu = ( dynamic_cast < Menu * > ( this->focus() ) );
    if ( menu.get() )
      this->_makeFirstChildFocused ( menu.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the item left.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_moveLeft ( Item *item )
{
  MENUKIT_CHECK_POINTER ( 2771991229u, item );

  if ( this->_hasHorizontalParent ( item ) )
  {
    this->_makePrevSiblingFocused ( item );
    return;
  }

  Menu::RefPtr parent ( item->parent() );
  if ( 0x0 == parent.get() )
    return;

  if ( this->_hasHorizontalParent ( parent.get() ) )
  {
    this->_makePrevSiblingFocused ( parent.get() );
    Menu::RefPtr focus ( ( dynamic_cast < Menu * > ( this->focus() ) ) );
    if ( focus.get() )
      this->_makeFirstChildFocused ( focus.get() );
  }

  else
  {
    this->focus ( parent.get() );
    Behavior::close ( parent.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the next sibling focused.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_makeNextSiblingFocused ( Item *item )
{
  MENUKIT_CHECK_POINTER ( 3752332794u, item );

  // Get the parent.
  Menu::RefPtr parent ( item->parent() );
  if ( 0x0 == parent.get() )
    return;

  // Find the item in the parent's list. This should always work.
  Menu::iterator ii = std::find_if ( parent->begin(), parent->end(), Item::RefPtr::IsEqual ( item ) );
  if ( parent->end() == ii )
    return;

  // Get the next child (the sibling).
  Item::RefPtr next ( MenuKit::Algorithms::nextItem ( ii, parent->begin(), parent->end() ) );
  if ( 0x0 == next.get() )
    return;

  // Make the next child (the sibling) focused. 
  this->focus ( next.get() );

  // Handle the transfer of the expansion-state.
  this->_transferExpansionState ( item, next.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the previous sibling focused.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_makePrevSiblingFocused ( Item *item )
{
  MENUKIT_CHECK_POINTER ( 2568630261u, item );

  // Get the parent.
  Menu::RefPtr parent ( item->parent() );
  if ( 0x0 == parent.get() )
    return;

  // Find the item in the parent's list. This should always work.
  Menu::reverse_iterator ii = std::find_if ( parent->rbegin(), parent->rend(), Item::RefPtr::IsEqual ( item ) );
  if ( parent->rend() == ii )
    return;

  // Get the next child (the sibling).
  Item::RefPtr next ( MenuKit::Algorithms::nextItem ( ii, parent->rbegin(), parent->rend() ) );
  if ( 0x0 == next.get() )
    return;

  // Make the next child (the sibling) focused. 
  this->focus ( next.get() );

  // Handle the transfer of the expansion-state.
  this->_transferExpansionState ( item, next.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the expansion transfer.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_transferExpansionState ( Item *iold, Item *inew )
{
  MENUKIT_CHECK_POINTER ( 2078991125u, iold );
  MENUKIT_CHECK_POINTER ( 1002744088u, inew );

  // Handle bad input.
  if ( 0x0 == iold || 0x0 == inew )
    return;

  // If the old and new items are both menus...
  if ( dynamic_cast < Menu * > ( inew ) &&
       dynamic_cast < Menu * > ( iold ) )
  {
    // The new focused item should have the same expanded-state.
    inew->expanded ( iold->expanded() );

    // The old item should not be expanded.
    Behavior::close ( iold );
  }

  // Otherwiae...
  else
  {
    // Both items should not be expanded.
    Behavior::close ( iold );
    Behavior::close ( inew );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the focused menu's first child the new focused item.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_makeFirstChildFocused ( Menu *menu )
{
  MENUKIT_CHECK_POINTER ( 1441576380u, menu );

  // Make sure there are children.
  if ( menu->empty() )
    return;

  // Make the first child focused. 
  this->focus ( menu->begin()->get() );

  // Show this menu.
  menu->expanded ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the focused menu's last child the new focused item.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::_makeLastChildFocused ( Menu *menu )
{
  MENUKIT_CHECK_POINTER ( 1120715478u, menu );

  // Make sure there are children.
  if ( menu->empty() )
    return;

  // Make the last child focused.
  Menu::iterator last ( menu->begin() + menu->numItems() - 1 );
  this->focus ( last->get() );

  // Show this menu.
  menu->expanded ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Select the focused item.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::selectFocused()
{
  MENUKIT_CHECK_POINTER ( 1283245209u, this->root() );

  // Get the focused item.
  Item::RefPtr focus ( this->focus() );

  // Handle specific case.
  if ( 0x0 == focus.get() )
  {
    // Make sure we have an focused item.
    this->ensureFocus();
    return;
  }

  // Is the focused item a menu?
  Menu::RefPtr menu ( dynamic_cast < Menu * > ( focus.get() ) );
  if ( menu.get() )
  {
    // If the menu is already expanded then just un-expand it.
    if ( menu->expanded() )
    {
      menu->expanded ( false );
      return;
    }

    // Move the focused item in the appropriate direction.
    this->moveFocused ( ( Menu::HORIZONTAL == menu->layout() ) ? RIGHT : DOWN );
  }

  // If the focused item is a button and we are supposed to hide...
  Button::RefPtr button ( dynamic_cast < Button * > ( focus.get() ) );
  if ( button.get() && Usul::Bits::has <unsigned int, unsigned int > ( this->flags(), HIDE_ALL_IF_BUTTON_SELECTED ) )
  {
    // Hide the whole tree.
    this->close ( this->root() );

    // No more focused item.
    this->focus ( 0x0 );
  }

  // Execute the command.
  Usul::Commands::Command::RefPtr command ( button.valid () ? button->command () : 0x0 );
  if ( command.valid () )
    command->execute ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the focused submenu.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::closeFocusedMenu()
{
  MENUKIT_CHECK_POINTER ( 3166814741u, this->root() );

  // Get the focused item.
  Item::RefPtr focus ( this->focus() );

  // Handle special case.
  if ( 0x0 == focus.get() )
  {
    // Hide the whole tree.
    this->close ( this->root() );
    return;
  }

  // un-expand the focused item's parent. 
  focus->parent()->expanded ( false );

  // This is probably unnecessary, but it cannot hurt.
  focus->expanded ( false );

  // Make the parent be focused unless it is the root.
  this->focus ( ( focus->parent() == this->root() ) ? 0x0 : focus->parent() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the menu and all its children.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::close ( Item *item )
{
  // Handle special case.
  if ( 0x0 == item )
    return;

  FlagsVisitor::RefPtr v ( new FlagsVisitor ( FlagsVisitor::REMOVE, Item::EXPANDED ) );
  item->accept ( *v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  If the focused item is null then set it to the root's first child.
//
///////////////////////////////////////////////////////////////////////////////

void Behavior::ensureFocus()
{
  MENUKIT_CHECK_POINTER ( 1429505883u, this->root() );

  // Get the focused item.
  Item::RefPtr focus ( this->focus() );

  // If we do not have an acvite item...
  if ( 0x0 == focus.get() )
    this->_makeFirstChildFocused ( this->root() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the menu has children.
//
///////////////////////////////////////////////////////////////////////////////

bool Behavior::_hasChildren ( Menu *menu )
{
  return ( ( menu ) ? ( false == menu->empty() ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the menu has siblings.
//
///////////////////////////////////////////////////////////////////////////////

bool Behavior::_hasSiblings ( Item *item )
{
  return ( item && item->parent() && false == item->parent()->empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the item's parent is a horizontal menu.
//
///////////////////////////////////////////////////////////////////////////////

bool Behavior::_hasHorizontalParent ( Item *item )
{
  return ( item && item->parent() && Menu::HORIZONTAL == item->parent()->layout() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the first parent that itself has a horizontal parent.
//
///////////////////////////////////////////////////////////////////////////////

Menu *Behavior::_findFirstParentWithHorizontalParent ( Item *item )
{
  // Get the parent menu.
  Menu *menu = item->parent();

  // Loop as longas we havea menu.
  while ( menu )
  {
    // Get the menu's parent.
    Menu *parent = menu->parent();

    // If the parent has a horizontal layout then return the menu.
    if ( parent && Menu::HORIZONTAL == parent->layout() )
      return menu;

    // Go up one level.
    menu = parent;
  }

  // If we get to here then there was no horizontal menu in the hierarchy.
  return 0x0;
}
