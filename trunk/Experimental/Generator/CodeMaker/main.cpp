
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Quick-and-dirty app to generate code.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Application.h"

#include "Usul/CommandLine/Arguments.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Register command-line arguments.
  typedef Usul::CommandLine::Arguments Arguments;
  Arguments &arguments ( Arguments::instance() );
  arguments.set ( argc, argv );

  // Set the global instance.
  wxApp::SetInstance ( new Application );

  // This is vital. Without it, we get an infinite loop when we exit.
  ::wxSetInstance ( ::GetModuleHandle ( arguments.program().c_str() ) );

  // This makes image handlers other than BMP available.
  ::wxInitAllImageHandlers();

  // Library entry and event loop. Have to pass copy of command-line 
  // arguments because wxWindows will delete them.
  return wxEntry ( arguments.argc(), arguments.copy() );
}
