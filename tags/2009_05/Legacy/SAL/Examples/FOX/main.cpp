
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Demo program that shows how to combine FOX and SAL.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "MainWindow.h"

using namespace GSG;


int main ( int argc, char **argv )
{
  // Declare the application.
  FXApp application ( "GSG FOX Viewer", "GSG" );

  // Initialize the application.
  application.init ( argc, argv );

  // Declare the main window.
  MainWindow *window = new MainWindow ( &application, application.getAppName(), 800, 600 );

  // Create the application.
  application.create();

  // Show the main window.
  window->show ( PLACEMENT_SCREEN );

  // Run the application.
  return application.run();
}
