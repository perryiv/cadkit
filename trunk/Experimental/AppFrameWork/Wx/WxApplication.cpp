
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "WxPrecompiled.h"
#include "WxApplication.h"
#include "WxInternalApp.h"
#include "WxInit.h"
#include "WxObjectMap.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Define.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Errors/Stack.h"

AFW_IMPLEMENT_OBJECT ( WxApplication );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

WxApplication::WxApplication() : BaseClass(),
  _docManager   ( new wxDocManager )
{
  // Set the global instance.
  wxApp::SetInstance ( new WxInternalApp );

  // Add us to the map.
  WxObjectMap::set ( this, wxTheApp );

  // Initialize wxWindows.
  WxInit::init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

WxApplication::~WxApplication()
{
  try
  {
    // Remove us from the map.
    WxObjectMap::remove ( this );

    // Have to delete our objects first.
    this->mainWindow ( 0x0 );

    // Let the app know we are exiting.
    if ( 0x0 != wxTheApp )
      wxTheApp->OnExit();

    // Use default cleanup function.
    ::wxEntryCleanup();
  }

  // Catch exceptions.
  AFW_CATCH_BLOCK ( "2787109888", "3059977611" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void WxApplication::run ( RunCommand command )
{
  Guard ( this->mutex() );

  // Are we supposed to run the main loop?
  if ( 0x0 != wxTheApp && WxApplication::RUN_MAIN_EVENT_LOOP == command )
  {
    // If we are already in the main loop then punt.
    if ( wxTheApp->IsMainLoopRunning() )
    {
      Usul::Errors::Stack::instance().push ( "Error 1648610151: nested calls to main loop" );
      return;
    }

    // Run the main loop.
    const int result ( wxTheApp->OnRun() );

    // Check result.
    if ( 0 != result )
    {
      std::ostringstream out;
      out << "Error 3536768902: main loop returned " << result;
      Usul::Errors::Stack::instance().push ( out.str() );
    }
  }

  // Are we supposed to run run while events?
  if ( 0x0 != wxTheApp && WxApplication::RUN_WHILE_EVENTS == command )
  {
    // Dispatch events until there are no more.
    while ( wxTheApp->Pending() )
      wxTheApp->Dispatch();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get document manager.
//
///////////////////////////////////////////////////////////////////////////////

wxDocManager *WxApplication::docManager()
{
  Guard ( this->mutex() );
  return _docManager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get document manager.
//
///////////////////////////////////////////////////////////////////////////////

const wxDocManager *WxApplication::docManager() const
{
  Guard ( this->mutex() );
  return _docManager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the application pointer.
//
///////////////////////////////////////////////////////////////////////////////

WxApplication *WxApplication::get()
{
  if ( AFW::Core::Program::valid() )
    return ( dynamic_cast < WxApplication * > ( AFW::Core::Program::instance().app() ) );
  else
    return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the application pointer.
//
///////////////////////////////////////////////////////////////////////////////

void WxApplication::quit()
{
  if ( wxTheApp && wxTheApp->GetTopWindow() )
    wxTheApp->GetTopWindow()->Close ( false );
}
