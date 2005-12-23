
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu-bar class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Menus/MenuBar.h"
#include "AppFrameWork/Menus/Button.h"
#include "AppFrameWork/Menus/MenuGroup.h"
#include "AppFrameWork/Core/BaseVisitor.h"
#include "AppFrameWork/Core/Define.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Conditions/HasNewModelPlugin.h"
#include "AppFrameWork/Conditions/HasOpenModelPlugin.h"
#include "AppFrameWork/Conditions/HasActiveEditor.h"
#include "AppFrameWork/Actions/Enable.h"
#include "AppFrameWork/Actions/NewModelAction.h"
#include "AppFrameWork/Actions/OpenModelAction.h"
#include "AppFrameWork/Actions/QuitProgramAction.h"
#include "AppFrameWork/Actions/CloseActiveEditor.h"

#include "Usul/Bits/Bits.h"

#include <iostream>

using namespace AFW::Menus;

AFW_IMPLEMENT_OBJECT ( MenuBar );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuBar::MenuBar() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MenuBar::~MenuBar()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a default GUI.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::init()
{
  Guard guard ( this->mutex() );

  // Clear existing menu.
  this->removeAll();

  // Actions reused below.
  AFW::Actions::Enable::RefPtr enable  ( new AFW::Actions::Enable ( true  ) );
  AFW::Actions::Enable::RefPtr disable ( new AFW::Actions::Enable ( false ) );

  // For convenience.
  AFW::Core::Program &factory ( AFW::Core::Program::instance() );

  // File menu.
  {
    AFW::Menus::MenuGroup::RefPtr menu ( factory.newObject<MenuGroup>() );
    if ( menu.valid() )
    {
      menu->textSet ( "File" );
      menu->underline ( 0 );
      {
        Button::RefPtr button ( Button::createButton ( "New...", "afw_new" ) );
        if ( button.valid() )
        {
          button->append ( new AFW::Conditions::HasNewModelPlugin ( true  ), enable  );
          button->append ( new AFW::Conditions::HasNewModelPlugin ( false ), disable );
          button->append ( new AFW::Actions::NewModelAction );
          menu->append ( button.get() );
        }
      }
      {
        Button::RefPtr button ( Button::createButton ( "Open...", "afw_open" ) );
        if ( button.valid() )
        {
          button->append ( new AFW::Conditions::HasOpenModelPlugin ( true  ), enable  );
          button->append ( new AFW::Conditions::HasOpenModelPlugin ( false ), disable );
          button->append ( new AFW::Actions::OpenModelAction );
          menu->append ( button.get() );
        }
      }
      {
        Button::RefPtr button ( Button::createButton ( "Close", "afw_close" ) );
        if ( button.valid() )
        {
          button->append ( new AFW::Conditions::HasActiveEditor ( true  ), enable  );
          button->append ( new AFW::Conditions::HasActiveEditor ( false ), disable );
          button->append ( new AFW::Actions::CloseActiveEditor );
          menu->append ( button.get() );
        }
      }
      {
        Button::RefPtr button ( Button::createSeparator() );
        if ( button.valid() )
        {
          menu->append ( button.get() );
        }
      }
      {
        Button::RefPtr button ( Button::createButton ( "Quit" ) );
        if ( button.valid() )
        {
          button->append ( new AFW::Actions::QuitProgramAction );
          menu->append ( button.get() );
        }
      }
    }
    this->append ( menu.get() );
  }

  // Edit menu.
  {
    AFW::Menus::MenuGroup::RefPtr menu ( factory.newObject<MenuGroup>() );
    if ( menu.valid() )
    {
      menu->textSet ( "Edit" );
      menu->underline ( 0 );
      {
      }
      this->append ( menu.get() );
    }
  }

  // View menu.
  {
    AFW::Menus::MenuGroup::RefPtr menu ( factory.newObject<MenuGroup>() );
    if ( menu.valid() )
    {
      menu->textSet ( "View" );
      menu->underline ( 0 );
      {
      }
      this->append ( menu.get() );
    }
  }

  // Tools menu.
  {
    AFW::Menus::MenuGroup::RefPtr menu ( factory.newObject<MenuGroup>() );
    if ( menu.valid() )
    {
      menu->textSet ( "Tools" );
      menu->underline ( 0 );
      {
      }
      this->append ( menu.get() );
    }
  }

  // Window menu.
  {
    AFW::Menus::MenuGroup::RefPtr menu ( factory.newObject<MenuGroup>() );
    if ( menu.valid() )
    {
      menu->textSet ( "Window" );
      menu->underline ( 0 );
      {
        Button::RefPtr button ( Button::createButton ( "Close", "afw_close" ) );
        if ( button.valid() )
        {
          button->append ( new AFW::Conditions::HasActiveEditor ( true  ), enable  );
          button->append ( new AFW::Conditions::HasActiveEditor ( false ), disable );
          button->append ( new AFW::Actions::CloseActiveEditor );
          menu->append ( button.get() );
        }
      }
      this->append ( menu.get() );
    }
  }

  // Help menu.
  {
    AFW::Menus::MenuGroup::RefPtr menu ( factory.newObject<MenuGroup>() );
    if ( menu.valid() )
    {
      menu->textSet ( "Help" );
      menu->underline ( 0 );
      {
      }
      this->append ( menu.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::accept ( AFW::Core::BaseVisitor *v )
{
  Guard guard ( this->mutex() );
  if ( v )
    v->visit ( this );
}
