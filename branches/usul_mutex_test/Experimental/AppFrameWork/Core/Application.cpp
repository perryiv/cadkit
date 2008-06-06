
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/BaseVisitor.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Registry.h"
#include "AppFrameWork/Windows/MainWindow.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Components/Call.h"
#include "Usul/Interfaces/IGuiInit.h"

#include <stdexcept>
#include <iostream>

using namespace AFW::Core;

AFW_IMPLEMENT_OBJECT ( Application );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application() : BaseClass(),
  _name        ( "Default Name" ),
  _mainWindow  ( 0x0 ),
  _flags       ( State::DIRTY ),
  _splash      ( AFW::Core::Icon ( "afw_splash" ), 1000 ),
  _events      (),
  _models      (),
  _recentFiles (),
  _registry    ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  // Safely...
  try
  {
    // Cleanup these members.
    _name.clear();
    _events.clear();
    _models.clear();
    _recentFiles.clear();
    Usul::Pointers::unreference ( _mainWindow );
    _mainWindow = 0x0;
    Usul::Pointers::unreference ( _registry );
    _registry = 0x0;
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( 2237933076ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Application::accept ( AFW::Core::BaseVisitor *v )
{
  Guard guard ( this->mutex() );
  if ( v )
    v->visit ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Application::dirty ( bool state )
{
  Guard guard ( this->mutex() );
  const unsigned int bit ( State::DIRTY );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::dirty() const
{
  Guard guard ( this->mutex() );
  const unsigned int bit ( State::DIRTY );
  return Usul::Bits::has ( _flags, bit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init()
{
  // One thread at a time.
  Guard guard ( this->mutex() );

  // Initialize registry.
  this->_initRegistry();

  // Make main window.
  this->mainWindow ( Program::instance().newObject<AFW::Windows::MainWindow>() );
  if ( this->mainWindow() )
    this->mainWindow()->init();

  // Ask all plugins to initialize their gui.
  Usul::Components::call < Usul::Interfaces::IGuiInit > 
    ( 1742547681ul, std::cout, &Usul::Interfaces::IGuiInit::guiInit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the splash-screen image.
//
///////////////////////////////////////////////////////////////////////////////

Application::SplashData Application::splashScreen() const
{
  Guard guard ( this->mutex() );
  return SplashData ( _splash );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the splash-screen image.
//
///////////////////////////////////////////////////////////////////////////////

void Application::splashScreen ( const Icon &icon, unsigned int milliseconds )
{
  Guard guard ( this->mutex() );
  _splash.first  = icon;
  _splash.second = milliseconds;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append an event.
//
///////////////////////////////////////////////////////////////////////////////

void Application::eventAppend ( AFW::Actions::Action *action, Object *object )
{
  // One thread at a time.
  Guard guard ( this->mutex() );

  // Append the event.
  _events.push_back ( Event ( action, object ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flush the event queue.
//
///////////////////////////////////////////////////////////////////////////////

void Application::eventsFlush()
{
  // One thread at a time.
  Guard guard ( this->mutex() );

  // Loop through events.
  while ( false == _events.empty() )
  {
    // Get event.
    Event event ( _events.front() );
    AFW::Actions::Action::RefPtr action ( event.first.get() );
    AFW::Core::Object::RefPtr object ( event.second.get() );

    // Pop now in case we throw.
    _events.pop_front();

    // If the action is valid then execute it.
    if ( action.valid() )
      action->execute ( object.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append recent file.
//
///////////////////////////////////////////////////////////////////////////////

void Application::recentFileAppend ( const std::string &file )
{
  Guard guard ( this->mutex() );
  _recentFiles.push_back ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append recent file.
//
///////////////////////////////////////////////////////////////////////////////

void Application::recentFileRemove ( const std::string &file )
{
  Guard guard ( this->mutex() );
  _recentFiles.remove ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Application::mainWindow ( AFW::Windows::MainWindow *mw )
{
  Guard guard ( this->mutex() );
  Usul::Pointers::unreference ( _mainWindow );
  _mainWindow = mw;
  Usul::Pointers::reference ( _mainWindow );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Windows::MainWindow *Application::mainWindow()
{
  Guard guard ( this->mutex() );
  return _mainWindow;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

const AFW::Windows::MainWindow *Application::mainWindow() const
{
  Guard guard ( this->mutex() );
  return _mainWindow;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Application::name ( const std::string &s )
{
  Guard guard ( this->mutex() );
  _name = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

std::string Application::name() const
{
  Guard guard ( this->mutex() );
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default implementation does nothing.
//
///////////////////////////////////////////////////////////////////////////////

void Application::run ( RunCommand command )
{
  // One thread at a time.
  Guard guard ( this->mutex() );

  // Flush event queue.
  if ( command == Application::RUN_MAIN_EVENT_LOOP )
    this->eventsFlush();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default implementation does nothing.
//
///////////////////////////////////////////////////////////////////////////////

void Application::quit()
{
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access registry.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::Registry *Application::registry()
{
  Guard guard ( this->mutex() );
  return _registry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access registry.
//
///////////////////////////////////////////////////////////////////////////////

const AFW::Core::Registry *Application::registry() const
{
  Guard guard ( this->mutex() );
  return _registry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize registry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initRegistry()
{
  // One thread at a time.
  Guard guard ( this->mutex() );

  // Safely...
  try
  {
    // Make it possible to call this function multiple times.
    Usul::Pointers::unreference ( _registry );
    _registry = 0x0;

    // Make new registry.
    _registry = Program::instance().newObject<Registry>();
    Usul::Pointers::reference ( _registry );

    // Make proper name for registry. In debug we strip the last 'd' character.
    std::string name ( _name );
    #ifdef _DEBUG
    if ( 'd' == name.at ( name.size() - 1 ) )
      name.resize ( name.size() - 1 );
    #endif

    // Initialize registry.
    _registry->init ( Program::instance().vendor(), name );
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 3388568473ul );
}
