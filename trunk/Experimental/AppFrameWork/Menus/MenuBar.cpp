
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
#include "AppFrameWork/Core/Group.h"
#include "AppFrameWork/Core/BaseVisitor.h"
#include "AppFrameWork/Core/Define.h"
#include "AppFrameWork/Conditions/HasNewModelPlugin.h"
#include "AppFrameWork/Conditions/HasOpenModelPlugin.h"
#include "AppFrameWork/Conditions/HasActiveEditor.h"
#include "AppFrameWork/Actions/Enable.h"
#include "AppFrameWork/Actions/NewModelAction.h"
#include "AppFrameWork/Actions/OpenModelAction.h"
#include "AppFrameWork/Actions/CloseActiveEditor.h"

#include "Usul/Bits/Bits.h"

#include <iostream>

using namespace AFW::Menus;


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

void MenuBar::buildDefault()
{
  // Safely...
  try
  {
    this->_buildDefault();
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( "3280844121", "3720564149" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a default GUI.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::_buildDefault()
{
  // Actions reused below.
  AFW::Actions::Enable::RefPtr enable  ( new AFW::Actions::Enable ( true  ) );
  AFW::Actions::Enable::RefPtr disable ( new AFW::Actions::Enable ( false ) );

  // File menu.
  AFW::Core::Group::ValidRefPtr fileMenu ( new AFW::Core::Group ( "File" ) );
  {
    {
      Button::ValidRefPtr button ( new Button ( "New...",  new AFW::Core::Icon ( "new" ) ) );
      button->append ( new AFW::Conditions::HasNewModelPlugin ( true  ), enable  );
      button->append ( new AFW::Conditions::HasNewModelPlugin ( false ), disable );
      button->append ( new AFW::Actions::NewModelAction );
      fileMenu->append ( button.get() );
    }
    {
      Button::ValidRefPtr button ( new Button ( "Open...", new AFW::Core::Icon ( "open" ) ) );
      button->append ( new AFW::Conditions::HasOpenModelPlugin ( true  ), enable  );
      button->append ( new AFW::Conditions::HasOpenModelPlugin ( false ), disable );
      button->append ( new AFW::Actions::OpenModelAction );
      fileMenu->append ( button.get() );
    }
    {
      Button::ValidRefPtr button ( new Button ( "Close",   new AFW::Core::Icon ( "close" ) ) );
      button->append ( new AFW::Conditions::HasActiveEditor ( true  ), enable  );
      button->append ( new AFW::Conditions::HasActiveEditor ( false ), disable );
      button->append ( new AFW::Actions::CloseActiveEditor );
      fileMenu->append ( button.get() );
    }
  }
  this->append ( fileMenu.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void MenuBar::accept ( AFW::Core::BaseVisitor *v )
{
  if ( v )
    v->visit ( this );
}
