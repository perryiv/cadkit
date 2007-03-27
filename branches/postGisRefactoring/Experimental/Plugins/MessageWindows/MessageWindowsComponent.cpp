
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Plugin for the text-message window.
//
///////////////////////////////////////////////////////////////////////////////

#include "MessageWindowsComponent.h"

#include "AppFrameWork/Actions/SetTextFromStdout.h"
#include "AppFrameWork/Actions/SetTextFromPluginActivity.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Windows/MainWindow.h"
#include "AppFrameWork/Windows/TextWindow.h"
#include "AppFrameWork/Windows/LogWindow.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MessageWindowsComponent , MessageWindowsComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

MessageWindowsComponent::MessageWindowsComponent() : BaseClass(),
  _mutex(),
  _textWindow ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

MessageWindowsComponent::~MessageWindowsComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MessageWindowsComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IChangeMenuBar::IID:
    return static_cast < Usul::Interfaces::IChangeMenuBar * > ( this );
  case Usul::Interfaces::IGuiInit::IID:
    return static_cast < Usul::Interfaces::IGuiInit * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string MessageWindowsComponent::getPluginName() const 
{
  return "Message Windows";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return name of plugin.
//
///////////////////////////////////////////////////////////////////////////////

void MessageWindowsComponent::changeMenuBar ( Usul::Interfaces::IUnknown *caller )
{
  Guard guard ( _mutex );
}


#if 0

AFW::Core::Frame should have a flags for "put a button in the Windows menu", and another one for "add this toolbar button that toggles the frame", and have 
everything manage itself. The implementation (the WxGui plugin details) can decide if hide/show really means delete/create.

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the gui.
//
///////////////////////////////////////////////////////////////////////////////

void MessageWindowsComponent::guiInit ( Usul::Interfaces::IUnknown * )
{
  Guard guard ( _mutex );

  // Don't instantiate.
  if ( false == AFW::Core::Program::valid() )
    return;

  // Get menu bar.
  AFW::Core::Application::RefPtr app ( AFW::Core::Program::instance().app() );
  AFW::Windows::MainWindow::RefPtr mw ( ( app.valid() ) ? app->mainWindow() : 0x0 );
  if ( false == mw.valid() )
    return;

  // Make text window.
  {
    AFW::Windows::TextWindow::ValidRefPtr window ( AFW::Core::Program::instance().newObject<AFW::Windows::TextWindow>() );
    if ( window.valid() )
    {
      window->dockState ( AFW::Windows::Window::DockState ( AFW::Core::DockSite::BOTTOM, 1 ) );
      window->persistentName ( "standard_output_window" );
      window->append ( new AFW::Actions::SetTextFromStdout );
      window->title ( "Text Messages" );
      window->icon ( AFW::Core::Icon ( "afw_text_output_16x16" ) );
      mw->append ( window.get() );
    }
  }

  // Add a log window.
  {
    AFW::Windows::LogWindow::ValidRefPtr window ( AFW::Core::Program::instance().newObject<AFW::Windows::LogWindow>() );
    if ( window.valid() )
    {
      window->dockState ( AFW::Windows::Window::DockState ( AFW::Core::DockSite::BOTTOM, 1 ) );
      window->persistentName ( "standard_log_window" );
      window->append ( new AFW::Actions::SetTextFromStdout );
      window->title ( "Log Window" );
      window->icon ( AFW::Core::Icon ( "afw_text_output_16x16" ) );
      mw->append ( window.get() );
    }
  }

  // Add a window that displays plugin information.
  {  
    AFW::Windows::TextWindow::ValidRefPtr window ( AFW::Core::Program::instance().newObject<AFW::Windows::TextWindow>() );
    if ( window.valid() )
    {
      window->dockState ( AFW::Windows::Window::DockState ( AFW::Core::DockSite::BOTTOM, 1 ) );
      window->persistentName ( "plugin_activity_window" );
      window->append ( new AFW::Actions::SetTextFromPluginActivity );
      window->title ( "Plugin Activity" );
      window->icon ( AFW::Core::Icon ( "afw_plugins_16x16" ) );
      mw->append ( window.get() );
    }
  }
}
