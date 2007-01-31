
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Demo program that shows how to combine FOX and OSG.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgFox/Core/MdiMainWindow.h"

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/CommandLine/Arguments.h"

#include "FoxTools/App/Application.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Headers/Icon.h"

#include "osgDB/Registry"


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
#ifdef _DEBUG
  // Output debug information.
  FX::fxTraceLevel = 0;
#endif

  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Declare the application.
  FoxTools::App::Application application ( "Helios", "CadKit" );

  // Initialize the application.
  application.init ( argc, argv );

  // Application icon.
  typedef FoxTools::Icons::Factory Icons;
  std::auto_ptr<FX::FXIcon> icon ( Icons::instance()->icon ( Icons::ICON_LOGO ) );

  // Declare the main window.
  OsgFox::Core::MdiMainWindow *window ( new OsgFox::Core::MdiMainWindow ( &application, application.getAppName(), icon.get(), icon.get() ) );

  // Create the application.
  application.create();

  // Show the main window.
  window->show ( FX::PLACEMENT_DEFAULT );

  // Load any command line files.
  window->loadCommandLineFiles();

  // Run the application.
  FX::FXint result ( application.run() );

  osgDB::Registry::instance()->clearObjectCache();
  osgDB::Registry::instance()->clearArchiveCache();
  // Release OSG plugins.
  osgDB::Registry::instance()->closeAllLibraries();

  return result;
}
