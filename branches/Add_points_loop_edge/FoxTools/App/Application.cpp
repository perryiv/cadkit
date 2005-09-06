
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
#define _CPP_CMATH 1
#endif

#ifdef _MSC_VER                     // VC++
# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.
# include <windows.h>               // Base Windows header file.
# include <shellapi.h>              // For DragAcceptFiles().
#endif // _MSC_VER

#include "FoxTools/App/Application.h"

#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/Window.h"

#include "Usul/Scope/Reset.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/Resources/ReportErrors.h"
#include "Usul/Interfaces/IReportErrors.h"

using namespace FoxTools::App;


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXIMPLEMENT ( Application, Application::BaseClass, 0x0, 0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( const std::string &name, const std::string &vendor ) : 
  BaseClass ( name.c_str(), vendor.c_str() ),
  _eventQueueLevel ( 0 )
{
  // Set this pointer for convenient access elsewhere.
  FoxTools::Functions::application ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  // Should be true.
  USUL_ASSERT ( 0 == _eventQueueLevel );

  // Unset this pointer.
  FoxTools::Functions::application ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called for every event.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
long Application::dispatchEvent ( FX::FXID hwnd, unsigned int message, unsigned int wParam, long lParam )
#else
FX::FXbool Application::dispatchEvent ( FXRawEvent& ev )
#endif
{
  // Safely...
  try
  {
    // Set the level. Makes sure it resets.
    Usul::Scope::Reset < unsigned int > reset ( _eventQueueLevel, this->eventQueueLevel() + 1, this->eventQueueLevel() );

    // Delegate.
    #ifdef _MSC_VER
    return this->_dispatchEvent ( hwnd, message, wParam, lParam );
    #else
    return this->_dispatchEvent ( ev );
    #endif
  }

  // Catch cancels.
  catch ( const Usul::Exceptions::Canceled & )
  {
    // If we are nested then re-throw.
    if ( this->eventQueueLevel() > 0 )
      throw;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    std::string message ( e.what() ? e.what() : "Error 3283152463: Standard exception caught while dispatching event" );
    Usul::Errors::Stack::instance().push ( message );
    this->_reportErrors();
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    Usul::Errors::Stack::instance().push ( "Error 2085332292: Unknown exception caught while dispatching event" );
    this->_reportErrors();
  }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called for every event.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
long Application::_dispatchEvent ( FX::FXID hwnd, unsigned int message, unsigned int wParam, long lParam )
#else
FX::FXbool Application::_dispatchEvent ( FXRawEvent& ev )
#endif
{
#ifdef _MSC_VER

  // Is this a drop-event?
  if ( WM_DROPFILES == message )
    return this->_handleDropEvent ( hwnd, message, wParam, lParam );

  // Delegate.
  return BaseClass::dispatchEvent ( hwnd, message, wParam, lParam );

#else

  // Delegate.
  return BaseClass::dispatchEvent ( ev );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the drag-and-drop event.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

long Application::_handleDropEvent ( FX::FXID id, unsigned int message, unsigned int wParam, long lParam )
{
  USUL_ASSERT ( WM_DROPFILES == message );

  // Get handle to drop event info.
  HDROP info ( reinterpret_cast < HDROP > ( wParam ) );

  // Get the window.
  FX::FXWindow *window = this->findWindowWithId ( id );

  // Make sure we have everything.
  if ( 0x0 == info || 0x0 == window )
    return ::DefWindowProc ( reinterpret_cast < HWND > ( id ), message, wParam, lParam );

  // Get the number of files dropped.
	unsigned int num ( ::DragQueryFile ( info, (unsigned int)-1, 0x0, 0 ) );

  // Loop through the files.
	for ( unsigned int i = 0; i < num; ++i )
	{
    // Get the file name.
		char filename[_MAX_PATH];
		::DragQueryFile ( info, i, filename, _MAX_PATH );

    // Send the event.
    window->handle ( this, FXSEL ( FX::SEL_COMMAND, Application::ID_DROP_FILE ), filename );
	}

  // All done.
	::DragFinish ( info );

  // It worked.
  return 0;
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Report any errors.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_reportErrors() const
{
  if ( false == Usul::Errors::Stack::instance().empty() )
  {
    Usul::Interfaces::IReportErrors::QueryPtr report ( Usul::Resources::reportErrors() );

    if( report.valid() )
    {
      typedef Usul::Interfaces::IReportErrors ReportErrors;
      const unsigned int options ( ReportErrors::ERROR_DIALOG | ReportErrors::STANDARD_OUT | ReportErrors::LOG_FILE );

      report->reportErrors ( options, true );
    }
  }
}
