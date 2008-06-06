
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The menu class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Menu.h"
#include "GFX.h"

#include "MenuKit/Errors.h"
#include "MenuKit/UpdateVisitor.h"

using namespace MenuKit::OSG; // Just this one or name clashing occurs.

//  Constructor.
Menu::Menu() : BaseClass()
{
}


//  Destructor.
Menu::~Menu()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  If the menu is showing, move the focused item down one.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::moveFocused ( MenuKit::Behavior::Direction d )
{
  // If we have a menu showing...
  if ( this->menu()->expanded() )
  {
    // Move the focused item.
    this->behavior()->moveFocused ( d );

    // Rebuild the scene.
    this->updateScene();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Select the focused menu-item, if any.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::selectFocused()
{
  // If we have a menu showing then select the focused item.
  if ( this->menu()->expanded() )
  {
    // Select the focused item.
    this->behavior()->selectFocused();

    // Rebuild the scene.
    this->updateScene();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the menu. Hide it if showing. Show it if hidden.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::toggleVisible()
{
  // Clear the scene.
  Menu::_removeAllChildren ( this->scene() );

  // If we are not showing...
  if ( !this->menu()->expanded() && !this->menu()->empty() )
  {
    // Show it.
    this->menu()->expanded ( true );

    // Tell the menu to update.
    this->updateMenu();
  }

  // Otherwise, close the focused submenu.
  else
    this->behavior()->closeFocusedMenu();

  // Rebuild the scene.
  this->updateScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Is the menu visible?
//
///////////////////////////////////////////////////////////////////////////////

bool Menu::isVisible () const
{
  return this->menu()->expanded();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu scene.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::updateScene()
{
  typedef MenuKit::OSG::GFX GFX;
  typedef osg::ref_ptr<osg::Node> NodePtr;

  // Clear the scene.
  Menu::_removeAllChildren ( this->scene() );

  // Handle trivial cases.
  if ( !this->menu()->expanded() )
    return;

  // Make the scene-builder.
  GFX::RefPtr gfx ( new MenuKit::OSG::GFX ( this->skin(), this->behavior()->focus() ) );
  MENUKIT_CHECK_POINTER ( 2151962067u, gfx.get() );

  // Generate a scene from the menu.
  GFX &functor = *gfx;
  NodePtr menu ( functor ( this->menu() ) );

  // Add the new scene.
  this->scene()->addChild ( menu.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the menu scene.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::updateMenu()
{
  MenuKit::UpdateVisitor::RefPtr visitor ( new MenuKit::UpdateVisitor );
  this->menu()->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Place the menu using the given matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::place ( const osg::Matrix &m )
{
  this->scene()->setMatrix ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the children from the group.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::_removeAllChildren ( osg::Group *group )
{
  if ( group )
  {
    while ( group->getNumChildren() )
    {
      // Since the children are held in an std::vector it should be faster to
      // remove them off the end.
      group->removeChild ( group->getNumChildren() - 1 );
    }
  }
}
