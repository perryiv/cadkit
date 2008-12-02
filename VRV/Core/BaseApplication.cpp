
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Core/BaseApplication.h"
#include "VRV/Core/JugglerFunctors.h"
#include "VRV/Core/Exceptions.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/Components/Manager.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

#include "vrj/Kernel/Kernel.h"

using namespace VRV::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::BaseApplication() : 
  BaseClass ( vrj::Kernel::instance() ),
  _mutex(),
  _timerServer ( new TimerServer )
{
  // Add the timer server as a plugin.
  Usul::Components::Manager::instance().addPlugin ( Usul::Interfaces::IUnknown::QueryPtr ( _timerServer ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::~BaseApplication()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::run()
{
  USUL_TRACE_SCOPE;

  // Get the kernel.
  vrj::Kernel* kernel ( vrj::Kernel::instance() );

  // Tell the kernel that we are the app.
  kernel->setApplication ( this );

  // Start the kernel.
  kernel->start();

  // Wait for events.
  kernel->waitForKernelStop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the application.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::quit()
{
  USUL_TRACE_SCOPE;
  vrj::Kernel::instance()->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this instance.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::init()
{
  bool stop ( false );

  try
  {
    this->_init();
  }

  catch ( const std::exception &e )
  {
    std::cout << "Error 1082603967: "
              << "Exception generated while initializing."
              << "\n\tWhat: " << e.what()
              << std::endl;
    stop = true;
  }
  catch ( ... )
  {
    std::cout << "Error 1082603859: "
              << "Unknown exception generated while initializing." 
              << std::endl;
    stop = true;
  }

  // Are you supposed to stop?
  if ( stop )
    vrj::Kernel::instance()->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the application.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_init()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::preFrame()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BaseApplication::_preFrame ), "3669056808" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_preFrame()
{
  USUL_TRACE_SCOPE;

  // Update the timer server.
  _timerServer->update();

  // Purge the timer server.
  _timerServer->purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after preFrame, but before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::latePreFrame()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BaseApplication::_latePreFrame ), "1897051329" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after preFrame, but before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_latePreFrame()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is done.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::postFrame()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BaseApplication::_postFrame ), "2751540437" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is done.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_postFrame()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default configuration filename path.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_setDefaultPath ( std::string& filename, const std::string& name )
{
  // Get the vendor and the program.
  const std::string vendor  ( Usul::App::Application::instance().vendor()  );
  const std::string program ( Usul::App::Application::instance().program() );

  // Set default file path.
  filename = Usul::Strings::format ( Usul::User::Directory::program ( vendor, program ), "/", name, ".xml" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all the config files.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_loadConfigFiles ( const std::vector < std::string > &configs )
{
  USUL_TRACE_SCOPE;

  // See if there is at least one config file. Do not use the ErrorChecker.
  if ( configs.empty() )
  {
    std::cout << "No VRJuggler config-files specified."
              << "\n\tAttempting to use a sim-mode configuration."
              << std::endl;
    this->_loadSimConfigs();
    return;
  }

  // Load the config files.
  std::for_each ( configs.begin(), configs.end(), VRV::Core::Detail::LoadConfigFile() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_loadSimConfigs()
{
  USUL_TRACE_SCOPE;

  std::cout << "Looking for environment variable VJ_CONFIG_DIR" << std::endl;

  // Try this environment variable.
  const char *dir = ::getenv ( "VJ_CONFIG_DIR" );
  if ( dir )
  {
    // Load default config files from this directory.
    this->_loadSimConfigs ( dir );
    return;
  }

  std::cout << "Environment variable VJ_CONFIG_DIR not found." << std::endl;
  std::cout << "Looking for environment variable VJ_BASE_DIR" << std::endl;

  // Try this environment variable.
  dir = ::getenv ( "VJ_BASE_DIR" );
  if ( dir )
  {
    // Make sure there is a slash.
    std::string d ( dir );
    std::string::size_type last ( d.size() - 1 );
    if ( '/' != d[last] || '\\' != d[last] )
      d += '/';

    // Add the sub-directory.
    d += "share/vrjuggler/data/configFiles";

    // Load default config files from this directory.
    this->_loadSimConfigs ( d );
    return;
  }

  std::cout << "Environment variable VJ_BASE_DIR not found." << std::endl;

  // If we get this far then we failed.
  Usul::Exceptions::Thrower < VRV::Core::Exceptions::UserInput > 
    ( "No default VRJuggler config-files found." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files from the given directory.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_loadSimConfigs ( const std::string& d )
{
  USUL_TRACE_SCOPE;

  // Return if string is empty.
  if ( d.empty() )
    return;

  // Make a copy.
  std::string dir ( d );

  // Make sure there is a slash.
  std::string::size_type last ( dir.size() - 1 );
  if ( '/' != dir[last] || '\\' != dir[last] )
    dir += '/';

  // The config-file loader.
  VRV::Core::Detail::LoadConfigFile loader;

  // Load the config files.
  loader ( dir + "sim.base.jconf" );
  loader ( dir + "sim.wand.mixin.jconf" );
  loader ( dir + "sim.analog.mixin.jconf" );
}
