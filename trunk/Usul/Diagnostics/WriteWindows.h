
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility functions for writing the program's state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DIAGNOSTICS_WRITE_WINDOWS_H_
#define _USUL_DIAGNOSTICS_WRITE_WINDOWS_H_

#include "Usul/Adaptors/Bind.h"
#include "Usul/Scope/Caller.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>


namespace Usul {
namespace Diagnostics {
namespace Helper {


/////////////////////////////////////////////////////////////////////////////
//
//  Write the dump file.
//
/////////////////////////////////////////////////////////////////////////////

int _writeDumpFile ( HANDLE fileHandle, EXCEPTION_POINTERS *exceptionPointers )
{
  MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
  exceptionInfo.ThreadId = ::GetCurrentThreadId();
  exceptionInfo.ExceptionPointers = exceptionPointers;
  exceptionInfo.ClientPointers = FALSE;
  ::MiniDumpWriteDump ( ::GetCurrentProcess(), ::GetCurrentProcessId(), fileHandle, MiniDumpNormal, &exceptionInfo, 0x0, 0x0 );
  return EXCEPTION_EXECUTE_HANDLER;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Write the dump file.
//
/////////////////////////////////////////////////////////////////////////////

inline void _writeDumpFile ( HANDLE fileHandle )
{
  __try
  {
    ::RaiseException ( 1, 0, 0, NULL );
  }
  __except ( Usul::Diagnostics::Helper::_writeDumpFile ( fileHandle, GetExceptionInformation() ), EXCEPTION_EXECUTE_HANDLER )
  {
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Write the state of the current process to file.
//
/////////////////////////////////////////////////////////////////////////////

void writeProcessState ( const std::string &fileName )
{
  HANDLE fileHandle ( ::CreateFileA ( fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0x0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0x0 ) );
  if ( 0x0 == fileHandle )
    return;
  Usul::Scope::Caller::RefPtr scopedFile ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( fileHandle, ::CloseHandle ) ) );
  Usul::Diagnostics::Helper::_writeDumpFile ( fileHandle );
}


} // namespace Helper
} // namespace Diagnostics
} // namespace Usul


#endif // _USUL_DIAGNOSTICS_WRITE_WINDOWS_H_
