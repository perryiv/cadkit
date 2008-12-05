
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
#include "Usul/Convert/Convert.h"
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
//  Helper function to close a handle.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  void closeHandle ( HANDLE &h )
  {
    if ( 0x0 != h )
    {
      ::CloseHandle ( h );
      h = 0x0;
    }
  }
}

#else
TODO
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to keep track of data.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  struct Data
  {
    Data() : _pi(), _stdoutRead ( 0x0 ), _stdoutWrite ( 0x0 )
    {
      ::ZeroMemory ( &_pi, sizeof ( PROCESS_INFORMATION ) );
    }
    ~Data()
    {
      Helper::closeHandle ( _pi.hProcess );
      Helper::closeHandle ( _pi.hThread );
      Helper::closeHandle ( _stdoutRead );
      Helper::closeHandle ( _stdoutWrite );
    }
    PROCESS_INFORMATION &processInfo()
    {
      return _pi;
    }
    void standardOut ( HANDLE stdoutRead, HANDLE stdoutWrite )
    {
      _stdoutRead  = stdoutRead;
      _stdoutWrite = stdoutWrite;
    }
    HANDLE standardOutRead()
    {
      return _stdoutRead;
    }
  private:
    PROCESS_INFORMATION _pi;
    HANDLE _stdoutRead;
    HANDLE _stdoutWrite;
  };
}

#else
TODO
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the process information.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  PROCESS_INFORMATION &getProcessInfo ( void *ptr )
  {
    Helper::Data *data ( reinterpret_cast < Helper::Data * > ( ptr ) );
    if ( 0x0 == data )
    {
      throw std::runtime_error ( "Error 3581526434: Process information in null" );
    }
    return data->processInfo();
  }
}

#else
TODO
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to return the process's stdout handle.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  HANDLE getProcessStdout ( void *ptr )
  {
    Helper::Data *data ( reinterpret_cast < Helper::Data * > ( ptr ) );
    if ( 0x0 == data )
    {
      throw std::runtime_error ( "Error 3446102113: Process information in null" );
    }
    return data->standardOutRead();
  }
}

#else
TODO
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to delete the process information.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  void deleteProcessInfo ( void *ptr )
  {
    Helper::Data *data ( reinterpret_cast < Helper::Data * > ( ptr ) );
    if ( 0x0 != data )
    {
      delete data;
    }
  }
}

#else
TODO
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to throw informative exception.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace Helper
{
  void throwException ( const std::string &message, const std::string &executable, const std::string &arguments, Usul::System::Process::ID id )
  {
    const std::string systemErrorMessage ( ( true == Usul::System::LastError::has() ) ? ( std::string ( ", System error: " ) + Usul::System::LastError::message() ) : std::string() );
    const std::string idString ( ( 0 != id ) ? ( std::string ( ", ID: " ) + Usul::Convert::Type<unsigned int,std::string>::convert ( static_cast<unsigned int> ( id ) ) ) : std::string() );
    throw std::runtime_error ( Usul::Strings::format ( message, ", Executable: ", executable, ", Arguments: ", arguments, idString, systemErrorMessage ) );
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

Process::Process ( const std::string &executable, 
                   const std::string &arguments, 
                   bool captureStdOut,
                   const std::string &dir ) : BaseClass(),
  _executable ( executable ),
  _arguments ( arguments ),
  _data ( 0x0 ),
  _hasBeenStopped ( false )
{
  USUL_TRACE_SCOPE;

#ifdef _MSC_VER

  // Initialize startup and process info.
  STARTUPINFO si;
  ::ZeroMemory ( &si, sizeof ( STARTUPINFO ) );
  Helper::Data *data ( new Helper::Data );

  // Are we piping the process's stdout to here?
  // See http://msdn.microsoft.com/en-us/library/ms682499(VS.85).aspx
  if ( true == captureStdOut )
  {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof ( SECURITY_ATTRIBUTES );
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = 0x0;

    HANDLE childStdoutRead  ( 0x0 );
    HANDLE childStdoutWrite ( 0x0 );

    // Make a read and write handle for a pipe.
    Usul::System::LastError::init();
    if ( FALSE == ::CreatePipe ( &childStdoutRead, &childStdoutWrite, &sa, 0 ) ) 
      Helper::throwException ( "Error 2712886592: Failed to create pipe for process", executable, arguments, 0 );

    // Make sure the read side of the pipe is not inherited by the process we will launch.
    Usul::System::LastError::init();
    if ( FALSE == ::SetHandleInformation ( childStdoutRead, HANDLE_FLAG_INHERIT, 0 ) )
      Helper::throwException ( "Error 3264436000: Failed to configure pipe for process", executable, arguments, 0 );

    // Set startup flags.
    si.hStdOutput = childStdoutWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Save handle.
    data->standardOut ( childStdoutRead, childStdoutWrite );
  }

  // Need a writable buffer for command-line arguments.
  // http://msdn.microsoft.com/en-us/library/ms682425(VS.85).aspx
  std::vector<char> args ( arguments.begin(), arguments.end() );
  args.push_back ( '\0' );

  // Make sure there is a space before the first argument.
  if ( ' ' != args.front() )
    args.insert ( args.begin(), ' ' );

  // Start the process. This immediately returns and does not 
  // wait for the new process to finish initialization.
  Usul::System::LastError::init();
  const BOOL result ( ::CreateProcessA ( executable.c_str(), &args[0], 
                                        0x0, 0x0, FALSE, CREATE_NO_WINDOW, 0x0, 
                                        ( ( true == dir.empty() ) ? 0x0 : dir.c_str() ),
                                        &si, &(data->processInfo()) ) );
  if ( FALSE == result )
    Helper::throwException ( "Error 1816237620: Failed to create process", executable, arguments, 0 );

  // Keep track of the process information.
  _data = data;

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
  Helper::deleteProcessInfo ( _data );
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
  return ( Helper::getProcessInfo ( _data ).dwProcessId );
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

  PROCESS_INFORMATION &pi ( Helper::getProcessInfo ( _data ) );
  DWORD exitCode ( 0 );

  Usul::System::LastError::init();

  if ( FALSE == ::GetExitCodeProcess ( pi.hProcess, &exitCode ) )
    Helper::throwException ( "Error 3041975851: Unable to determine state of process", _executable, _arguments, pi.dwProcessId );

  // If the exit code is something other than "still active" it's not running.
  if ( STILL_ACTIVE != exitCode )
    return false;

  Usul::System::LastError::init();

  // If we get here then we don't know if it is really still active, or if it 
  // just happened to return with a numerical exit code equal to STILL_ACTIVE.

  // This will return immediately because of the zero timeout, and will tell 
  // us if the process is still running.
  const DWORD result ( ::WaitForSingleObject ( pi.hProcess, 0 ) );
  switch ( result )
  {
    case WAIT_OBJECT_0:
      return false;
    case WAIT_TIMEOUT:
      return true;
    default:
      Helper::throwException ( "Error 1676419807: Unable to determine state of process", _executable, _arguments, pi.dwProcessId );
      return false; // Never gets here.
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

  PROCESS_INFORMATION &pi ( Helper::getProcessInfo ( _data ) );

  // Should not happen.
  if ( 0x0 == pi.hProcess )
    return;

  // This is the hammer.
  ::TerminateProcess ( pi.hProcess, 1 );

  // Done with these handles.
  Helper::closeHandle ( pi.hProcess );
  Helper::closeHandle ( pi.hThread );

  // Note: do not clean up standard output handles here because the data 
  // in the pipe could still be valid even though we just terminated the 
  // process that made it. Just a hunch...

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

  PROCESS_INFORMATION &pi ( Helper::getProcessInfo ( _data ) );
  ::WaitForSingleObject ( pi.hProcess, milliseconds );

#else
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send any accumulated output to the given stream.
//  Only has effect if process's stdout is being captured.
//
///////////////////////////////////////////////////////////////////////////////

void Process::output ( std::ostream &out, bool appendNewLine, bool flushStream )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

#ifdef _MSC_VER

  HANDLE stdoutHandle ( Helper::getProcessStdout ( _data ) );
  if ( 0x0 == stdoutHandle )
    return;

  DWORD available ( 0 );
  Usul::System::LastError::init();
  if ( FALSE == ::PeekNamedPipe ( stdoutHandle, 0x0, 0, 0x0, &available, 0x0 ) )
    Helper::throwException ( "Error 1610403124: Failed to see if pipe has data", _executable, _arguments, this->id() );

  // Is there anything in the pipe?
  if ( 0 == available )
    return;

  std::vector<char> buffer ( 1024, '\0' );
  DWORD read ( 0 );
  Usul::System::LastError::init();
  if ( FALSE == ::ReadFile ( stdoutHandle, &buffer[0], buffer.size() - 1, &read, 0x0 ) )
    Helper::throwException ( "Error 2905041806: Failed to read pipe", _executable, _arguments, this->id() );

  if ( 0 == read )
    return;

  out << &buffer[0];

  if ( true == appendNewLine )
    out << '\n';

  if ( true == flushStream )
    out << std::flush;

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