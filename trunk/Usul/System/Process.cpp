
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Process classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Process.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"
#include "Usul/Trace/Trace.h"

#include "windows.h"

#include <stdexcept>
#include <vector>

using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the process information.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  PROCESS_INFORMATION *processInfo ( void *data )
  {
    PROCESS_INFORMATION *pi ( reinterpret_cast < PROCESS_INFORMATION * > ( data ) );
    if ( 0x0 == pi )
    {
      throw std::runtime_error ( "Error 3581526434: Process information in null" );
    }
    return pi;
  }
}

#else
TODO
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Process::Process ( const std::string &executable, const std::string &arguments, const std::string &dir ) : BaseClass(),
  _data ( 0x0 ),
  _hasBeenStopped ( false )
{
  USUL_TRACE_SCOPE;

#ifdef _MSC_VER

  STARTUPINFO si;
  ::ZeroMemory ( &si, sizeof ( STARTUPINFO ) );

  PROCESS_INFORMATION *pi ( new PROCESS_INFORMATION );
  ::ZeroMemory ( pi, sizeof ( PROCESS_INFORMATION ) );

  // Initialize system error before we start the process.
  Usul::System::LastError::init();

  // Need a writable buffer for command-line arguments.
  // http://msdn.microsoft.com/en-us/library/ms682425(VS.85).aspx
  std::vector<char> args ( arguments.begin(), arguments.end() );
  args.push_back ( '\0' );

  // Start the process. This immediately returns and does not 
  // wait for the new process to finish initialization.
  const BOOL result ( ::CreateProcessA ( executable.c_str(), &args[0], 
                                        0x0, 0x0, FALSE, CREATE_NO_WINDOW, 0x0, 
                                        ( ( true == dir.empty() ) ? 0x0 : dir.c_str() ),
                                        &si, pi ) );
  if ( FALSE == result )
  {
    throw std::runtime_error ( Usul::Strings::format ( 
      "Error 1816237620: Failed to create process '", executable, ' ', arguments, "'",
      ( ( true == Usul::System::LastError::has() ) ? 
      ( std::string ( "System error: " ) + Usul::System::LastError::message() ) : std::string() ) ) );
  }

  // Keep track of the process information.
  _data = pi;

#else
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Process::~Process()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Usul::System::Process::_destroy ), "3380342559" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Process::_destroy()
{
  USUL_TRACE_SCOPE;
  this->stop();
  delete Helper::processInfo ( _data );
  _data = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the system's process id.
//
///////////////////////////////////////////////////////////////////////////////

Process::ID Process::id() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
#ifdef _MSC_VER
  return ( Helper::processInfo ( _data )->dwProcessId );
#else
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the process running?
//
///////////////////////////////////////////////////////////////////////////////

bool Process::isRunning() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _hasBeenStopped )
    return false;

#ifdef _MSC_VER

  PROCESS_INFORMATION *pi ( Helper::processInfo ( _data ) );
  DWORD exitCode ( 0 );
  if ( FALSE == ::GetExitCodeProcess ( pi->hProcess, &exitCode ) )
    throw std::runtime_error ( Usul::Strings::format ( "Error 3041975851: Unable to determine state of process ", pi->dwProcessId ) );

  // If the exit code is something other than "still active" it's not running.
  if ( STILL_ACTIVE != exitCode )
    return false;

  // If we get here then we don't know if it is really still active, or if it 
  // just happened to return with a numerical exit code equal to STILL_ACTIVE.

  // This will return immediately because of the zero timeout, and will tell 
  // us if the process is still running.
  const DWORD result ( ::WaitForSingleObject ( pi->hProcess, 0 ) );
  switch ( result )
  {
    case WAIT_OBJECT_0:
      return false;
    case WAIT_TIMEOUT:
      return true;
    default:
      throw std::runtime_error ( Usul::Strings::format ( "Error 1676419807: Unable to determine state of process ", pi->dwProcessId ) );
  }

#else
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the process.
//
///////////////////////////////////////////////////////////////////////////////

void Process::stop()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( false == this->isRunning() )
    return;

  // Set this now.
  _hasBeenStopped = true;

#ifdef _MSC_VER

  PROCESS_INFORMATION *pi ( Helper::processInfo ( _data ) );

  // This is the hammer.
  ::TerminateProcess ( pi->hProcess, 1 );

  // Done with these handles.
  ::CloseHandle ( pi->hProcess );
  ::CloseHandle ( pi->hThread );

#else
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for the process.
//
///////////////////////////////////////////////////////////////////////////////

void Process::wait ( unsigned long milliseconds )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _hasBeenStopped )
    return;

#ifdef _MSC_VER

  PROCESS_INFORMATION *pi ( Helper::processInfo ( _data ) );
  ::WaitForSingleObject ( pi->hProcess, milliseconds );

#else
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current process's id.
//
///////////////////////////////////////////////////////////////////////////////

Process::ID Process::currentProcessId()
{
  USUL_TRACE_SCOPE;
#ifdef _MSC_VER
  return GetCurrentProcessId();
#else
  TODO
#endif
}
