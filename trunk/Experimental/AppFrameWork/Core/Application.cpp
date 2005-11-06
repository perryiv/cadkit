
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
#include "AppFrameWork/Core/Define.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Components/Manager.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/File/Find.h"
#include "Usul/Bits/Bits.h"

#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/INotifyClose.h"

#include <stdexcept>
#include <iostream>

using namespace AFW::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Components::Manager PluginManager;
typedef PluginManager::UnknownSet PluginSet;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::Application *AFW::Core::Application::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application() : 
  _name       ( "Default Application Name" ),
  _vendor     ( "CadKit" ),
  _plugExts   (),
  _type       ( Application::MULTIPLE_DOCUMENT_INTERFACE ),
  _mainWindow ( 0x0 ),
  _gui        (),
  _flags      ( State::DIRTY )
{
#ifdef _DEBUG
  _plugExts.insert ( "plugd" );
#else
  _plugExts.insert ( "plug" );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the one instance.
//
///////////////////////////////////////////////////////////////////////////////

Application &Application::instance()
{
  if ( 0x0 == _instance )
    _instance = new Application;
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::run()
{
  // Safely...
  try
  {
    this->_run();
    return true;
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( "1577337362", "3787854746" );

  // We did not run successfully.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up all that we can.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::cleanup()
{
  // Safely...
  try
  {
    this->_cleanup();
    return true;
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( "3679062750", "2028712905" );

  // Did not work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify all plugins they are about to close.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_notifyClose()
{
  // Loop through the plugins.
  PluginSet plugins ( PluginManager::instance().getInterfaces ( Usul::Interfaces::INotifyClose::IID ) );
  for ( PluginSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    Usul::Interfaces::INotifyClose::QueryPtr notify ( (*i).get() );
    if ( notify.valid() )
      notify->notifyClose ( 0x0 );
  }

  // Explicitely notify our GUI server.
  {
    Usul::Interfaces::INotifyClose::QueryPtr notify ( _gui );
    if ( notify.valid() )
      notify->notifyClose ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read values from the registry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_registryRead()
{
  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write values to the registry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_registryWrite()
{
  // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up all that we can.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_cleanup()
{
  // Notify plugins that we are closing.
  this->_notifyClose();

  // Save values in the registry.
  this->_registryWrite();

  // Destroy the GUI-server.
  if ( _gui.valid() )
  {
    _gui->destroyApplication();
    _gui = static_cast < IGUIServer *> ( 0x0 );
  }

  // This should delete the main window and all dependents.
  _mainWindow = 0x0;

  // Release all our plugins
  PluginManager::instance().clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadPlugins()
{
  // Get the directory where the application lives.
  const std::string dir ( Usul::CommandLine::Arguments::instance().directory() );
   
  // Get a list of all the potential plugins.
  PluginManager::Strings plugins;
  for ( PluginExtensions::const_iterator j = _plugExts.begin(); j != _plugExts.end(); ++j )
    Usul::File::find ( dir, *j, plugins );

  // Load all plugins.
  PluginManager::instance().load ( Usul::Interfaces::IPlugin::IID, plugins );

  // Feedback about plugins.
  const PluginManager::Strings names ( PluginManager::instance().names() );
  std::cout << "Found " << names.size() << " plugins: ";
  std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( std::cout, "; " ) );
  std::cout << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_run()
{
  // Load all plugins first.
  this->_loadPlugins();

  // Read the registry.
  this->_registryRead();

  // Look for the GUI factory.
  this->_initGuiFactory();

  // Build a default application if there is no main window.
  if ( false == _mainWindow.valid() )
    this->buildDefault();

  // Run the GUI application.
  if ( _gui.valid() )
    _gui->runApplication();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the GUI factory.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initGuiFactory()
{
  // Look for the GUI server.
  _gui = PluginManager::instance().getInterface ( IGUIServer::IID );
  if ( false == _gui.valid() )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 3387005042: No GUI server found" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a default GUI.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::buildDefault()
{
  // Safely...
  try
  {
    this->_buildDefault();
    return true;
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( "2380330731", "1439245355" );

  // Did not work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a default GUI.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_buildDefault()
{
  this->mainWindow ( new MainWindow );
  this->mainWindow()->buildDefault();
  if ( _gui.valid() )
    _gui->buildApplication();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Application::dirty ( bool state )
{
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
  const unsigned int bit ( State::DIRTY );
  return Usul::Bits::has ( _flags, bit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform the action.
//
///////////////////////////////////////////////////////////////////////////////

void Application::enableWindow ( bool state, Window *window )
{
  if ( _gui.valid() && 0x0 != window )
    _gui->enableWindow ( state, window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true of the window is enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::isWindowEnabled ( const Window *window )
{
  return ( ( _gui.valid() && 0x0 != window ) ? _gui->isWindowEnabled ( window ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that the object is being removed from the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::removeNotify ( Window *window )
{
  if ( _gui.valid() && 0x0 != window )
    _gui->removeNotify ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that the object is being destroyed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::destroyNotify ( Window *window )
{
  if ( _gui.valid() && 0x0 != window )
    _gui->destroyNotify ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Application::accept ( AFW::Core::BaseVisitor *v )
{
  if ( v )
    v->visit ( this );
}
