
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Demo program that shows how to combime FOX GUI toolkit and CadKit.
//
///////////////////////////////////////////////////////////////////////////////

#include "MainWindow.h"

using namespace CadKit;


int main ( int argc, char **argv )
{
  // Declare the application.
  FXApp application ( "CadKit Demo Cube", "CadKit" );

  // Initialize the application.
  application.init ( argc, argv );

  // Declare the main window.
  MainWindow *window = new MainWindow ( &application, application.getAppName(), 800, 600 );

  // Initialize the main window.
  window->init();

  // Create the application.
  application.create();

  // Show the main window.
  window->show ( PLACEMENT_SCREEN );

  // Run the application.
  return application.run();
}
