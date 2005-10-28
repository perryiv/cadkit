
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxComponent.h"

#include "FoxTools/App/Application.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Functions/Create.h"
#include "FoxTools/Functions/Delete.h"
#include "FoxTools/Registry/Registry.h"

#include "FoxTools/Headers/Icon.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/ToolTip.h"
#include "FoxTools/Headers/MenuBar.h"
#include "FoxTools/Headers/MenuTitle.h"
#include "FoxTools/Headers/MenuCommand.h"

#include "AppFrameWork/Core/Define.h"
#include "AppFrameWork/Core/Constants.h"
#include "AppFrameWork/Menus/MenuBar.h"
#include "AppFrameWork/Menus/MenuGroup.h"
#include "AppFrameWork/Menus/MenuButton.h"

#include "Usul/Errors/Assert.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/System/Screen.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Cast/Cast.h"

#include <stdexcept>
#include <iostream>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FoxComponent , FoxComponent::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( FoxComponent );


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trace level.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct SetTraceLevel
  {
    SetTraceLevel()
    {
      #ifdef _DEBUG
      FX::fxTraceLevel = 0;
      #endif
    }
  } _setTraceLevel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Message map.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( FoxComponent ) MessageMap[] = 
{
  FXMAPFUNC ( FX::SEL_CLOSE, 0, FoxComponent::onCommandClose ),
};
FOX_TOOLS_IMPLEMENT ( FoxComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FoxComponent() : BaseClass(),
  _app       ( AFW::Core::Application::instance() ),
  _foxApp    ( 0x0 ),
  _mainWin   ( 0x0 ),
  _menuBar   ( 0x0 ),
  _menuPanes ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::~FoxComponent()
{
  try
  {
    this->destroyApplication();
  }
  AFW_CATCH_BLOCK ( "3473786244", "4269789571" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FoxComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IGUIServer::IID:
    return static_cast < Usul::Interfaces::IGUIServer * > ( this );
  case Usul::Interfaces::INotifyClose::IID:
    return static_cast < Usul::Interfaces::INotifyClose * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify the component.
//
///////////////////////////////////////////////////////////////////////////////

bool FoxComponent::notifyClose ( Usul::Interfaces::IUnknown * )
{
  // Write to the registry.
  this->_registryWrite();

  // make sure we do not use this again.
  _mainWin = 0x0;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::buildApplication()
{
  // Initialize.
  const bool firstTime ( 0x0 == _foxApp );

  // If this is the first time...
  if ( firstTime )
  {
    // Make the application.
    _foxApp = new FoxTools::App::Application ( _app.name(), _app.vendor() );

    // Initialize it.
    Usul::CommandLine::Arguments &args ( Usul::CommandLine::Arguments::instance() );
    _foxApp->init ( args.argc(), args.argv() );

    // Make a tool tip.
    new FXToolTip ( _foxApp, 0 );
  }

  // Build the main window.
  this->_buildMainWindow();

  // If this is the first time...
  if ( firstTime )
  {
    // Create the application.
    _foxApp->create();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the main window.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildMainWindow()
{
  USUL_ASSERT ( 0x0 != _foxApp );

  // Punt if there is no main window.
  AFW::Core::MainWindow::RefPtr mw ( _app.mainWindow() );
  if ( false == mw.valid() )
    return;

  // If this is the first time...
  if ( 0x0 == _mainWin )
  {
    // Application icon.
    typedef FoxTools::Icons::Factory Icons;
    FX::FXIcon *icon ( Icons::instance()->icon ( Icons::ICON_LOGO ) );

    // Get appropriate size.
    FoxRect size ( this->_initialMainWindowSize() );

    // Make main window and create it if we should.
    _mainWin = new FX::FXMainWindow ( _foxApp, _foxApp->getAppName(), icon, icon, FX::DECOR_ALL, size[0], size[1], size[2], size[3] );
    FoxTools::Functions::create ( _mainWin );

    // Set target to this instance.
    _mainWin->setTarget ( this );
    _mainWin->setSelector ( 0 );
 }

  // Make these other components.
  this->_buildMenuBar();
#if 0
  this->_buildToolBar();
  this->_buildPanelLeft();
  this->_buildPanelRight();
  this->_buildPanelBottom();
  this->_buildClientArea();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu-bar.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildMenuBar()
{
  USUL_ASSERT ( 0x0 != _mainWin );
  USUL_ASSERT ( 0x0 != _app.mainWindow() );

  // Punt if there is no menu-bar or if not dirty.
  AFW::Menus::MenuBar::RefPtr menuBar ( _app.mainWindow()->menuBar() );
  if ( false == menuBar.valid() || false == menuBar->dirty() )
    return;

  // If this is the first time...
  if ( 0x0 == _menuBar )
  {
    // Menu bar.
    _menuBar = new FX::FXMenuBar ( _mainWin );
    FoxTools::Functions::create ( _menuBar );
  }

  // Make sure there are not any top-level menus.
  this->_cleanTopMenus();

  // Build all top-level menus.
  for ( AFW::Menus::MenuBar::Menus::iterator i = menuBar->menus().begin(); i != menuBar->menus().end(); ++i )
  {
    AFW::Menus::MenuItem::ValidRefPtr item ( *i );
    this->_buildTopMenu ( dynamic_cast < AFW::Menus::MenuGroup * > ( item.get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all top-level menus.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_cleanTopMenus()
{
  USUL_ASSERT ( 0x0 != _mainWin );

  // Make sure the menu-bar does not have any child windows.
  FoxTools::Functions::deleteChildren ( _menuBar );

  // This should delete all the menu panes that are children of the main 
  // window and are on the menu-bar.
  _menuPanes[MenuBarPaneKey(_mainWin,_menuBar)].clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the top-level menu.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildTopMenu ( AFW::Menus::MenuGroup *group )
{
  USUL_ASSERT ( 0x0 != _mainWin );

  // Punt if there is no group or if not dirty.
  if ( 0x0 == group || false == group->dirty() )
    return;

  // Make the menu pane.
  MenuPanePtr pane ( new FX::FXMenuPane ( _mainWin ) );
  _menuPanes[MenuBarPaneKey(_mainWin,_menuBar)].push_back ( pane );
  FoxTools::Functions::create ( pane.get() );

  // Make the menu title.
  const std::string text ( this->_formatItemText ( group ) );
  FX::FXMenuTitle *title ( new FX::FXMenuTitle ( _menuBar, text.c_str(), 0x0, pane.get() ) );
  FoxTools::Functions::create ( title );

  // Build all sub-menus.
  for ( AFW::Menus::MenuGroup::Items::iterator i = group->begin(); i != group->end(); ++i )
  {
    AFW::Menus::MenuItem::ValidRefPtr item ( *i );
    this->_buildSubMenu    ( pane.get(), dynamic_cast < AFW::Menus::MenuGroup  * > ( item.get() ) );
    this->_buildMenuButton ( pane.get(), dynamic_cast < AFW::Menus::MenuButton * > ( item.get() ) );
  }

  // Not dirty now.
  group->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::destroyApplication()
{
  if ( 0x0 == _foxApp )
    return;

  try
  {
    _menuPanes.clear();
    delete _foxApp;
  }

  catch ( ... )
  {
    _foxApp = 0x0;
    throw;
  }

  _foxApp = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::runApplication()
{
  // Make sure the application is built.
  this->buildApplication();

  // Show the main window if we have one.
  if ( 0x0 != _mainWin )
    _mainWin->show ( FX::PLACEMENT_DEFAULT );

  // Run the application.
  _foxApp->run();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::updateApplication()
{
  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write to the registry.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_registryWrite()
{
  // If we have a main window...
  if ( 0x0 != _mainWin )
  {
    // Is it maximized?
    FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::MAXIMIZED, _mainWin->isMaximized() );

    // If the main window is not maximized...
    if ( false == _mainWin->isMaximized() )
    {
      // Write the size and position to the registry.
      FX::FXint x ( Usul::Math::maximum ( ( _mainWin->getX() ), 0 ) );
      FX::FXint y ( Usul::Math::maximum ( ( _mainWin->getY() ), 0 ) );
      FX::FXint w ( _mainWin->getWidth() );
      FX::FXint h ( _mainWin->getHeight() );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::X,      x );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::Y,      y );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::WIDTH,  w );
      FoxTools::Registry::write ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::HEIGHT, h );
    }
  }

  // Flush the cached values and actually write them to the registry.
  _foxApp->reg().write();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main window is being closed.
//
///////////////////////////////////////////////////////////////////////////////

long FoxComponent::onCommandClose ( FXObject *object, FXSelector selector, void *data )
{
  // Close down.
  this->notifyClose ( 0x0 );

  // Continue propagation of message.
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the size to make the main window.
//
///////////////////////////////////////////////////////////////////////////////

FoxComponent::FoxRect FoxComponent::_initialMainWindowSize()
{
  // Get dimensions of screen.
  const FXint sw ( USUL_UNSAFE_CAST ( FXint, Usul::System::Screen::width()  ) );
  const FXint sh ( USUL_UNSAFE_CAST ( FXint, Usul::System::Screen::height() ) );

  // Default sizes. (They are correct!)
  const FX::FXint dx ( USUL_UNSAFE_CAST ( FXint, ( 0.1 * sw ) ) );
  const FX::FXint dy ( USUL_UNSAFE_CAST ( FXint, ( 0.1 * sh ) ) );
  const FX::FXint dw ( USUL_UNSAFE_CAST ( FXint, ( 0.8 * sw ) ) );
  const FX::FXint dh ( USUL_UNSAFE_CAST ( FXint, ( 0.8 * sh ) ) );

  // Get dimensions of main window.
  FX::FXint x ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::X,      dx ) );
  FX::FXint y ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::Y,      dy ) );
  FX::FXint w ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::WIDTH,  dw ) );
  FX::FXint h ( FoxTools::Registry::read ( AFW::Registry::Sections::MAIN_WINDOW, AFW::Registry::Keys::HEIGHT, dh ) );

  // Make sure it fits.
  const bool xFits ( ( x > 0  ) && ( x < ( sw - w ) ) );
  const bool yFits ( ( y > 10 ) && ( y < ( sh - h ) ) );
  const bool wFits ( w < sw );
  const bool hFits ( h < sh );
  const bool fits ( xFits && yFits && wFits && hFits );

  // Return proper rectangle.
  if ( fits )
    return FoxComponent::FoxRect ( x, y, w, h );
  else
    return FoxComponent::FoxRect ( dx, dy, dw, dh ) ;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu group.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildSubMenu ( FX::FXMenuPane *pane, AFW::Menus::MenuGroup *group )
{
  USUL_ASSERT ( 0x0 != _menuBar );

  // Return if group is null.
  if ( 0x0 == group )
    return;

  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu button.
//
///////////////////////////////////////////////////////////////////////////////

void FoxComponent::_buildMenuButton ( FX::FXMenuPane *pane, AFW::Menus::MenuButton *button )
{
  // Return if null or not dirty.
  if ( 0x0 == button || 0x0 == pane || false == button->dirty() )
    return;

  // Make the button.
  const std::string text ( this->_formatItemText ( button ) );
  FX::FXMenuCommand *command ( new FX::FXMenuCommand ( pane, text.c_str() ) );
  FoxTools::Functions::create ( command );

  // Not dirty now.
  button->dirty ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Form the text in the appropriate way.
//
///////////////////////////////////////////////////////////////////////////////

std::string FoxComponent::_formatItemText ( AFW::Menus::MenuItem *item ) const
{
  std::string s;
  if ( item )
  {
    s = item->text();
    const unsigned short u ( item->underline() );
    if ( u < s.length() )
    {
      s.insert ( u, "&" );
    }
  }
  return s;
}
