
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Mutex.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Functions/SafeCall.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Set mutex factory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
}


int main( int argc, char** argv )
{
  // Add some plugin extensions.
#ifdef _DEBUG
  Usul::Components::Manager::instance().addPluginExtension ( "plugd" );
#else
  Usul::Components::Manager::instance().addPluginExtension ( "plug" );
#endif

  // Get the directory where the application lives.
  const std::string dir ( Usul::CommandLine::Arguments::instance().directory() );

  Usul::Components::Manager::instance().addDirectory( dir );

  // Load all plugins.
  Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID );

  // Feedback about plugins.
  const Usul::Components::Manager::Strings names ( Usul::Components::Manager::instance().names() );
  std::cout << "Found " << names.size() << " plugins: ";
  std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( std::cout, "; " ) );
  std::cout << std::endl;

  // Run the test
  Usul::Functions::safeCall ( _run,   "2358076732" );

  return 0;
}
