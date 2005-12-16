
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Fox/FoxApplication.h"

#include "FoxTools/App/Application.h"

#include <iostream>

AFW_IMPLEMENT_OBJECT ( FoxApplication );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxApplication::FoxApplication() : BaseClass(),
  _app   ( 0x0 ),
  _mutex ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxApplication::~FoxApplication()
{
  try
  {
    this->destroy();
  }
  AFW_CATCH_BLOCK ( "1773236956", "2559949247" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxApplication::destroy()
{
  Guard guard ( _mutex );

  if ( 0x0 == _app )
    return;

  try
  {
    delete _app;
  }

  catch ( ... )
  {
    _app = 0x0;
    throw;
  }

  _app = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxApplication::run ( AFW::Core::Application::RunCommand command )
{
  Guard guard ( _mutex );
  if ( AFW::Core::Application::RUN_WHILE_EVENTS == command )
    this->_runWhileEvents();
  else
    this->_runMainEventLoop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxApplication::_runMainEventLoop()
{
  Guard guard ( _mutex );

  // Make sure the application is built.
  this->_build();

  // Handle no application.
  if ( 0x0 == _app )
    return;

  _app->run();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxApplication::_runWhileEvents()
{
  Guard guard ( _mutex );

  // Handle no application.
  if ( 0x0 == _app )
    return;

  _app->runWhileEvents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the application.
//
///////////////////////////////////////////////////////////////////////////////

void FoxApplication::_build()
{
  Guard guard ( _mutex );

  // Handle existing application.
  if ( 0x0 != _app )
    return;

  AFW::Core::Application &app ( AFW::Core::Application::instance() );
  _app = new FoxTools::App::Application ( app.name(), app.vendor() );
}
