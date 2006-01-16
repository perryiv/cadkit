
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for creating the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Actions/CreateMenuBar.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Windows/MainWindow.h"

#include <iostream>

using namespace AFW::Actions;

USUL_IMPLEMENT_TYPE_ID ( CreateMenuBar );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CreateMenuBar::CreateMenuBar() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CreateMenuBar::~CreateMenuBar()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform the action.
//
///////////////////////////////////////////////////////////////////////////////

void CreateMenuBar::execute ( AFW::Core::Object * )
{
  Guard guard ( this->mutex() );

  // Check program state.
  if ( false == AFW::Core::Program::valid() )
    return;

  // Lock program.
  AFW_GUARD_PROGRAM;

  // Check application.
  if ( 0x0 == AFW::Core::Program::instance().app() )
    return;

  // Get the main window.
  AFW::Windows::MainWindow::RefPtr mw ( AFW::Core::Program::instance().app()->mainWindow() );
  if ( false == mw.valid() )
    return;

  // Remove the menu bar.
  mw->menuBar ( 0x0 );

  // Make new menu bar.
  AFW::Menus::MenuBar::RefPtr bar ( AFW::Core::Program::instance().newObject<AFW::Menus::MenuBar>() );
  if ( false == bar.valid() )
    return;

  // Initialize it.
  bar->init();

  // Add it to the main window.
  mw->menuBar ( bar.get() );

  // Report any errors.
  std::cout << AFW::Core::Program::instance().errors() << std::flush;
}
