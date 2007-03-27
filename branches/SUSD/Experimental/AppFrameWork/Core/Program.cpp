
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenience class for managing the program life-cycle.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Windows/MainWindow.h"
#include "AppFrameWork/Windows/TextWindow.h"
#include "AppFrameWork/Windows/LogWindow.h"
#include "AppFrameWork/Core/Registry.h"
#include "AppFrameWork/Core/StreamSink.h"
#include "AppFrameWork/Core/InitFunctor.h"
#include "AppFrameWork/Core/LifeCycle.h"
#include "AppFrameWork/Core/PluginRegistry.h"
#include "AppFrameWork/Menus/Button.h"
#include "AppFrameWork/Menus/MenuGroup.h"

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/DLL/Library.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Errors/Stack.h"
#include "Usul/File/Find.h"
#include "Usul/File/Path.h"
#include "Usul/File/Temp.h"
#include "Usul/Interfaces/INotifyClose.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Predicates/CanWrite.h"
#include "Usul/System/DateTime.h"
#include "Usul/System/LastError.h"

#include <sstream>
#include <iostream>

using namespace AFW::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

namespace AFW
{
  namespace Core
  {
    namespace Detail
    {
      Usul::Threads::SetMutexFactory mutexFactory ( &Threads::OT::newOpenThreadsMutex );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Components::Manager PluginManager;
typedef PluginManager::UnknownSet PluginSet;


///////////////////////////////////////////////////////////////////////////////
//
//  Namespace member(s).
//
///////////////////////////////////////////////////////////////////////////////

namespace AFW { namespace Core { namespace Detail
{
  AFW::Core::Program *_instance ( 0x0 );
  Usul::Threads::Mutex *_mutex ( Usul::Threads::Mutex::create() );
} } }


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to help with loading initial factories.
//
///////////////////////////////////////////////////////////////////////////////

#define TYPE_ID(type) ( ( typeid ( type ) ).name() )
#define FACTORY_TYPE(name_space,class_name)\
  _factories[TYPE_ID(name_space::class_name)] = new name_space::class_name::New##class_name


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::Program() : 
  _mutex       (),
  _factories   (),
  _stdout      (),
  _plugExts    (),
  _pluginCB    ( new PluginListener ),
  _redirect    (),
  _vendor      ( "CadKit" ),
  _app         ( 0x0 )
{
  // Register plugin extension.
  #ifdef _DEBUG
  _plugExts.insert ( "plugd" );
  #else
  _plugExts.insert ( "plug" );
  #endif

  // Add listener for plugin activity.
  Usul::DLL::Library::append ( _pluginCB.get() );

  // Load up the factories.
  FACTORY_TYPE ( AFW::Core,     Application );
  FACTORY_TYPE ( AFW::Windows,  Frame );
  FACTORY_TYPE ( AFW::Windows,  Group );
  FACTORY_TYPE ( AFW::Windows,  MainWindow );
  FACTORY_TYPE ( AFW::Windows,  StatusBar );
  FACTORY_TYPE ( AFW::Windows,  TextWindow );
  FACTORY_TYPE ( AFW::Windows,  LogWindow );
  FACTORY_TYPE ( AFW::Core,     Registry );
  FACTORY_TYPE ( AFW::Menus,    Button );
  FACTORY_TYPE ( AFW::Menus,    MenuBar );
  FACTORY_TYPE ( AFW::Menus,    MenuGroup );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::~Program()
{
  // Safely...
  try
  {
    // Cleanup these members.
    _factories.clear();
    _stdout.clear();
    _plugExts.clear();
    _redirect.clear();
    _vendor.clear();
    Usul::Pointers::unreference ( _app );
    _app = 0x0;

    // Remove and delete listener.
    USUL_ASSERT ( true == Usul::DLL::Library::remove ( _pluginCB.get() ) );
    USUL_ASSERT ( 1 == _pluginCB->refCount() );
    _pluginCB = 0x0;
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( 3382795029ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the one instance.
//
///////////////////////////////////////////////////////////////////////////////

Program &Program::instance()
{
  Usul::Threads::Guard<Usul::Threads::Mutex> guard ( *Detail::_mutex );
  if ( 0x0 == Detail::_instance )
    Detail::_instance = new Program;
  return *Detail::_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the one instance.
//
///////////////////////////////////////////////////////////////////////////////

void Program::destroy()
{
  Usul::Threads::Guard<Usul::Threads::Mutex> guard ( *Detail::_mutex );
  if ( Detail::_instance )
  {
    delete Detail::_instance;
    Detail::_instance = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the single instance is valid.
//
///////////////////////////////////////////////////////////////////////////////

bool Program::valid()
{
  Usul::Threads::Guard<Usul::Threads::Mutex> guard ( *Detail::_mutex );
  return ( 0x0 != Detail::_instance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print errors.
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::errors ( bool system ) const
{
  // One thread at a time.
  Program::Guard guard ( _mutex );

  // Write to this stream.
  std::ostringstream out;

  // Print error-stack if any.
  if ( false == Usul::Errors::Stack::instance().empty() )
    out << Usul::Errors::Stack::instance().format();

  // Print system errors if any.
  if ( system && Usul::System::LastError::has() )
    out << "System Error: " << Usul::System::LastError::message() << std::endl;

  // Initialize errors.
  Usul::Errors::Stack::instance().clear();
  Usul::System::LastError::init();

  // Return string.
  return out.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to run and cleanup the application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::execute ( int argc, char **argv, InitFunctor *init, const std::string &name )
{
  try { this->_execute ( argc, argv, init, name ); }
  AFW_CATCH_BLOCK ( 4187008777ul );
  std::cout << this->errors() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to run and cleanup the application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_execute ( int argc, char **argv, InitFunctor *init, const std::string &name )
{
  // Print any errors.
  std::ostringstream out;
  out << this->errors() << std::flush;

  // One thread at a time.
  Program::Guard guard ( _mutex );

  // Initialize errors.
  Usul::Errors::Stack::instance().clear();
  Usul::System::LastError::init();

  // Set command-line arguments.
  typedef Usul::CommandLine::Arguments Args;
  Args::instance().set ( argc, argv );

  // Print any errors.
  out << this->errors() << std::flush;

  // Determine program name.
  const std::string program ( ( name.empty() ) ? Usul::File::base ( Args::instance().program() ) : name );

  // Set default redirect file.
  this->redirect ( this->_defaultRedirect ( program ) );

  // Hook up output streams. This has to stay in scope.
  StreamSink::RefPtr sink ( new StreamSink ( this->redirect() ) );

  // Now that we have output redirected, give the user some feedback.
  std::cout << program << " started: " << Usul::System::DateTime::now() << std::endl;
  std::cout << "Output file: " << this->redirect() << std::endl;

  // Print any errors.
  std::cout << out.str() << std::flush;
  std::cout << this->errors() << std::flush;

  // Delegate.
  try { this->_execute ( init, program ); }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( 3701296428ul );

  // Print any errors.
  std::cout << this->errors() << std::flush;

  // Feedback
  std::cout << program << " stopped: " << Usul::System::DateTime::now() << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to run and cleanup the application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_execute ( InitFunctor *init, const std::string &name )
{
  // Asserts if there is a system error.
  Usul::System::LastError::Assert ea;

  // Local scope
  {
    // Load all plugins now. Release in destructor.
    AFW::Core::PluginsLifeCycle plugins;

    // Local scope
    {
      // Create app in constructor, set to null in destructor.
      AFW::Core::AppLifeCycle app;

      // Create default application.
      this->appInit ( init, name );

      // Print any errors.
      std::cout << this->errors() << std::flush;

      // Run the application.
      this->appRun();
    }
  }

  // Print any errors.
  std::cout << this->errors() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create default application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::appInit ( InitFunctor *init, const std::string &name )
{
  // Allocate the application if we need to.
  if ( 0x0 == this->app() )
    this->app ( this->newObject<Application>() );

  // Print any errors.
  std::cout << this->errors() << std::flush;

  // Set name.
  this->app()->name ( name );

  // Call initialization functor.
  if ( init )
    (*init) ( this->app() );

  // Initialize the application.
  this->app()->init();

  // Print any errors.
  std::cout << this->errors() << std::flush;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::appRun()
{
  if ( this->app() )
    this->app()->run ( Application::RUN_MAIN_EVENT_LOOP );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Program::pluginsLoad()
{
  // One thread at a time.
  Guard guard ( _mutex );

  // Get the directory where the application lives.
  const std::string dir ( Usul::CommandLine::Arguments::instance().directory() );

  // Find all potential plugins.
  PluginManager::Strings plugins ( this->pluginsFind() );

  // Load all plugins.
  PluginManager::instance().load ( Usul::Interfaces::IPlugin::IID, plugins );

  // Feedback about plugins.
  const PluginManager::Strings names ( PluginManager::instance().names() );
  if ( false == names.empty() )
  {
    const std::string what ( ( names.size() > 1 ) ? " plugins: " : " plugin: " );
    std::cout << "Found " << names.size() << what;
    std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( std::cout, "; " ) );
    std::cout << std::endl;
  }
  else
  {
    std::cout << "No plugins found" << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all potential plugins.
//
///////////////////////////////////////////////////////////////////////////////

PluginManager::Strings Program::pluginsFind() const
{
  // One thread at a time.
  Guard guard ( _mutex );

  // Initialize list.
  PluginManager::Strings plugins;

  // Get plugins from registry file. A missing file, or an error, will result 
  // in false. Otherwise, we get true, even if the file was found but was 
  // empty. This behavior facilitates no plugins being loaded.
  if ( true == this->_readPluginRegistry ( plugins ) )
    return plugins;

  // Load all plugins in the directory.
  this->_readPluginDir ( plugins );

  // Return what we have.
  return plugins;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all potential plugins.
//
///////////////////////////////////////////////////////////////////////////////

bool Program::_readPluginRegistry ( PluginManager::Strings &plugins ) const
{
  // One thread at a time.
  Guard guard ( _mutex );

  // Read plugins from registry file.
  PluginRegistry registry;
  return registry.read ( plugins );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find all potential plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_readPluginDir ( PluginManager::Strings &plugins ) const
{
  // One thread at a time.
  Guard guard ( _mutex );

  // Get the directory where the application lives.
  const std::string dir ( Usul::CommandLine::Arguments::instance().directory() );

  // Get a list of all the potential plugins.
  for ( PluginExtensions::const_iterator j = _plugExts.begin(); j != _plugExts.end(); ++j )
    Usul::File::find ( dir, *j, plugins );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release all plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Program::pluginsRelease()
{
  // One thread at a time.
  Guard guard ( _mutex );

  // Feedback.
  const PluginManager::Strings names ( PluginManager::instance().names() );
  const std::string what ( ( names.size() > 1 ) ? " plugins" : " plugin" );
  std::cout << "Releasing " << names.size() << what << std::endl;

  // Loop through the plugins.
  PluginSet plugins ( PluginManager::instance().getInterfaces ( Usul::Interfaces::INotifyClose::IID ) );
  for ( PluginSet::iterator i = plugins.begin(); i != plugins.end(); ++i )
  {
    Usul::Interfaces::INotifyClose::QueryPtr notify ( (*i).get() );
    if ( notify.valid() )
      notify->notifyClose ( 0x0 );
  }

  // This releases the plugins.
  PluginManager::instance().clear ( &(std::cout) );

  // Feedback.
  std::cout << "Done releasing" << what << std::endl;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Called when std::cout and std::cerr are used.
//
///////////////////////////////////////////////////////////////////////////////

void Program::standardOutAppend ( const char *s, std::streamsize n )
{
  // One thread at a time.
  Guard guard ( _mutex );

  // Handle bad input.
  if ( 0x0 == s || n <= 0 )
    return;

  // Append the string.
  std::string temp ( s, n );
  _stdout.push_back ( temp );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Get standard out.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Program::standardOutGet ( unsigned int start, std::string &s )
{
  // One thread at a time.
  Guard guard ( _mutex );

  // Handle bad input.
  if ( _stdout.size() <= start )
    return start;

  // Append the strings.
  const unsigned int num ( _stdout.size() );
  for ( unsigned int i = start; i < num; ++i )
    s += _stdout[i];

  // Return the number of lines.
  return num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new window.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::Object *Program::newObject ( const std::string &name )
{
  Guard guard ( _mutex );

  // Look for factory.
  AFW::Core::NewObjectFunctor::RefPtr factory ( this->factory ( name ) );

  // Check factory.
  if ( false == factory.valid() )
  {
    const std::string type ( ( name.empty() ) ? name : "unknown type" );
    Usul::Errors::Stack::instance().push ( "Error 1883894975: Failed to find factory for type: " + type );
    return 0x0;
  }

  // Return new object.
  return ( (*factory)() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the factory.
//
///////////////////////////////////////////////////////////////////////////////

void Program::factory ( const std::string &name, AFW::Core::NewObjectFunctor *functor )
{
  Guard guard ( _mutex );
  _factories[name] = functor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the factory.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::NewObjectFunctor *Program::factory ( const std::string &name )
{
  Guard guard ( _mutex );
  Factories::iterator i ( _factories.find ( name ) );
  return ( ( _factories.end() == i ) ? 0x0 : i->second.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the plugin extensions.
//
///////////////////////////////////////////////////////////////////////////////

Program::PluginExtensions Program::pluginExtensions() const
{
  Guard guard ( _mutex );
  PluginExtensions p ( _plugExts );
  return p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the plugin extensions.
//
///////////////////////////////////////////////////////////////////////////////

void Program::pluginExtensions ( const PluginExtensions &p )
{
  Guard guard ( _mutex );
  _plugExts = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the plugin listener. This function is not thread-safe.
//
///////////////////////////////////////////////////////////////////////////////

PluginListener *Program::pluginListener()
{
  Guard guard ( _mutex );
  return _pluginCB.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name of the file to send stdio and stderr.
//
///////////////////////////////////////////////////////////////////////////////

void Program::redirect ( const std::string &r )
{
  Guard guard ( _mutex );
  _redirect = r;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the file to send stdio and stderr.
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::redirect() const
{
  Program::Guard guard ( _mutex );
  return std::string ( _redirect );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the file to send stdio and stderr.
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::_defaultRedirect ( const std::string &name ) const
{
  typedef Usul::CommandLine::Arguments Args;

  // One thread at a time.
  Program::Guard guard ( _mutex );

  // Get program name and log name.
  const std::string program ( ( name.empty() ) ? Usul::File::base ( Args::instance().program() ) : name );
  const std::string log ( program + ".log" );

  // First attempt at redirect file is the full path of the log name.
  std::string output ( Usul::File::fullPath ( log ) );

  // Change location to temporary directory if necessary.
  if ( false == Usul::Predicates::CanWrite::test ( output ) )
    output = Usul::File::Temp::directory() + '/' + log;

  // Return what we have.
  return output;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vendor name.
//
///////////////////////////////////////////////////////////////////////////////

void Program::vendor ( const std::string &v )
{
  Guard guard ( _mutex );
  _vendor = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vendor name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::vendor() const
{
  Guard guard ( _mutex );
  const std::string v ( _vendor );
  return v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::app ( Application *a )
{
  Guard guard ( _mutex );
  Usul::Pointers::unreference ( _app );
  _app = a;
  Usul::Pointers::reference ( _app );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the application.
//
///////////////////////////////////////////////////////////////////////////////

const Application *Program::app() const
{
  Guard guard ( _mutex );
  return _app;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the application.
//
///////////////////////////////////////////////////////////////////////////////

Application *Program::app()
{
  Guard guard ( _mutex );
  return _app;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the mutex.
//
///////////////////////////////////////////////////////////////////////////////

Program::Mutex &Program::mutex() const
{
  return _mutex;
}
