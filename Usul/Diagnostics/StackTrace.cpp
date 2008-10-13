
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility class for getting and printing the call stack.
//  See: http://www.ibm.com/developerworks/linux/library/l-cppexcep.html
//
//  Note: individual objects are not guarded because all the operations that 
//  write to the object happen in the constructor. The functions that do this 
//  writing are guarded by a single mutex that is shared by all instances.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Diagnostics/StackTrace.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"

#ifdef __GNUC__
# include <execinfo.h>
# include <unistd.h>
#elif _MSC_VER
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <dbghelp.h>
#endif

#include <iostream>
#include <list>
#include <sstream>
#include <stdio.h>

// Need stack frames or else RtlCaptureContext will cause a crash.
// http://msdn.microsoft.com/en-us/library/chh3fb0k(VS.71).aspx
// http://msdn.microsoft.com/en-us/library/2kxx5t2c(VS.71).aspx
// http://www.gamedev.net/community/forums/topic.asp?topic_id=455278
#ifdef _MSC_VER
# pragma optimize( "y", off )
#endif

using namespace Usul::Diagnostics;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::RecursiveMutex Mutex;
typedef Usul::Threads::Guard<Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize and clean up stack trace plumbing. 
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul { namespace Diagnostics { namespace Helper
{
  struct Manager
  {
    Manager() : _mutex()
    {
      #ifdef _MSC_VER
      ::SymInitialize ( ::GetCurrentProcess(), NULL, TRUE );
      DWORD options ( ::SymGetOptions() );
      options = Usul::Bits::add ( options, SYMOPT_LOAD_LINES );
      options = Usul::Bits::add ( options, SYMOPT_FAIL_CRITICAL_ERRORS );
      options = Usul::Bits::remove ( options, SYMOPT_UNDNAME );
      options = Usul::Bits::remove ( options, SYMOPT_DEFERRED_LOADS );
      ::SymSetOptions ( options );
      #endif
    }
    ~Manager()
    {
      #ifdef _MSC_VER
      ::SymCleanup ( ::GetCurrentProcess() );
      #endif
    }
    Mutex _mutex;
  } _manager;
}}}


/////////////////////////////////////////////////////////////////////////////
//
//  Default construction grabs the current function stack.
//
/////////////////////////////////////////////////////////////////////////////

StackTrace::StackTrace() : _c()
{
  StackTrace::_get ( _c );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

StackTrace::StackTrace ( const StackTrace &b ) : _c ( b._c )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

StackTrace::~StackTrace()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return internal container.
//
/////////////////////////////////////////////////////////////////////////////

StackTrace::Container StackTrace::get() const
{
  return _c;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return a string.
//
/////////////////////////////////////////////////////////////////////////////

std::string StackTrace::toString() const
{
  std::ostringstream out;
  for ( Container::const_iterator i = _c.begin(); i != _c.end(); ++i )
  {
    out << (*i) << '\n';
  }
  return out.str();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Fill the container with the function signatures.
//
/////////////////////////////////////////////////////////////////////////////

void StackTrace::_get ( Container &c )
{
  // Need to guard this function for all objects.
  Guard guard ( Usul::Diagnostics::Helper::_manager._mutex );

#ifdef __GNUC__

  // See http://www.ibm.com/developerworks/linux/library/l-cppexcep.html

  const int maxStackLevels ( 1024 );
  void * array[maxStackLevels];
  const int actualStackSize ( ::backtrace ( array, maxStackLevels - 1 ) );

  if ( actualStackSize >= 1 )
  {
    char **symbols ( ::backtrace_symbols ( array, actualStackSize ) );
    for ( int i = 0; i < actualStackSize; ++i )
    {
      c.insert ( c.end(), symbols[i] );
    }
    ::free ( symbols );
  }

#elif defined _MSC_VER

  // See http://members.gamedev.net/EvilSteve/ThreadStuff/PMemory.cpp
  // and http://www.codeproject.com/KB/threads/StackWalker.aspx

  // Initialize current thread context.
  CONTEXT context;
  ::memset ( &context, 0, sizeof ( CONTEXT ) );

  // Must have frame-pointers or this will cause a crash.
  // Haven't figured out how to safely check. See the following:
  // http://www.gamedev.net/community/forums/topic.asp?topic_id=455278
  // http://msdn.microsoft.com/en-us/library/chh3fb0k(VS.71).aspx
  // http://msdn.microsoft.com/en-us/library/2kxx5t2c(VS.71).aspx
  ::RtlCaptureContext ( &context );

  // Initialize stack frame.
  STACKFRAME64 stackFrame;
  ::memset ( &stackFrame, 0, sizeof ( STACKFRAME64 ) );

  // Used to quietly ignore platforms that are not supported.
  bool platformSupported ( true );

  // Initialization of stack frame object varies per platform.
  DWORD machineType;
  #ifdef _M_IX86
  machineType = IMAGE_FILE_MACHINE_I386;
  stackFrame.AddrPC.Offset = context.Eip;
  stackFrame.AddrPC.Mode = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.Ebp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.Esp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
  #elif _M_X64
  machineType = IMAGE_FILE_MACHINE_AMD64;
  stackFrame.AddrPC.Offset = context.Rip;
  stackFrame.AddrPC.Mode = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.Rsp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.Rsp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
  #elif _M_IA64
  machineType = IMAGE_FILE_MACHINE_IA64;
  stackFrame.AddrPC.Offset = context.StIIP;
  stackFrame.AddrPC.Mode = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.IntSp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;
  stackFrame.AddrBStore.Offset = context.RsBSP;
  stackFrame.AddrBStore.Mode = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.IntSp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
  #else
  platformSupported = false;
  #endif

  // The structure to hold the "symbol" for the stack frame.
  const unsigned int maxSymbolNameSize ( 2048 );
  const unsigned int bufferSize ( sizeof ( IMAGEHLP_SYMBOL64 ) + maxSymbolNameSize );
  std::vector<char> buffer ( bufferSize, '\0' );
  IMAGEHLP_SYMBOL64 *symbol ( reinterpret_cast < IMAGEHLP_SYMBOL64 * > ( &buffer[0] ) );
  symbol->SizeOfStruct = sizeof ( IMAGEHLP_SYMBOL64 );
  symbol->MaxNameLength = maxSymbolNameSize - 1;

  // This might look like a hack, but it's an effort to prevent an infinite 
  // loop in case StackWalk64() never returns FALSE.
  const unsigned int maxStackDepth ( 1000000 );

  // Used to hold the undecorated symbol name.
  std::vector<char> undecorated ( maxSymbolNameSize, '\0' );

  // Loop through the stack.
  for ( unsigned int i = 0; i < maxStackDepth; ++i )
  {
    // Get the stack data.
    if ( FALSE == ::StackWalk64 ( machineType, 
                                  ::GetCurrentProcess(), 
                                  ::GetCurrentThread(), 
                                  &stackFrame,
                                  &context,
                                  0x0,
                                  SymFunctionTableAccess64,
                                  SymGetModuleBase64,
                                  0x0 ) )
    {
      // End of the function stack.
      break;
    }

    // This will hold the answer.
    std::ostringstream answer;

    // Get the file name and line number.
    {
      DWORD displacement ( 0 );
      IMAGEHLP_LINE64 line;
      ::memset ( &line, 0, sizeof ( IMAGEHLP_LINE64 ) );
      line.SizeOfStruct =  sizeof ( IMAGEHLP_LINE64 );
      if ( ::SymGetLineFromAddr64 ( ::GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, &line ) )
      {
        answer << ( ( 0x0 != line.FileName ) ? line.FileName : "" );
        answer << " (" << line.LineNumber << ") ";
      }
    }

    // Get the name of the function.
    {
      DWORD64 displacement ( 0 );
      if ( ::SymGetSymFromAddr64 ( ::GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, symbol ) )
      {
        // Get the undecorated name.
        if ( 0 < ::UnDecorateSymbolName ( symbol->Name, &undecorated[0], undecorated.size() - 1, UNDNAME_COMPLETE ) )
        {
          answer << &undecorated[0];
        }
      }
    }

    // Copy the answer. Skip the first one because it's just this class's constructor.
    if ( i > 0 )
    {
      const std::string info ( answer.str() );
      const std::string text ( ( false == info.empty() ) ? info : std::string ( "No stack frame available" ) );
      c.insert ( c.end(), text );
    }
  }

#endif // _MSC_VER
}


/////////////////////////////////////////////////////////////////////////////
//
//  Print the function signatures to stdout and stderr.
//
/////////////////////////////////////////////////////////////////////////////

void StackTrace::print()
{
  // Need to guard this function for all objects.
  Guard guard ( Usul::Diagnostics::Helper::_manager._mutex );

#ifdef __GNUC__

  // Note: instead of using StackTrace::get() or StackTrace::toString() 
  // we use backtrace_symbols_fd which dumps to standard streams. Even 
  // though it makes the code slightly more complex, this is much better 
  // when the proces is so corrupt that allocating memory is not possible.
  // See http://www.ibm.com/developerworks/linux/library/l-cppexcep.html

  const int maxStackLevels ( 1024 );
  void * array[maxStackLevels];
  const int actualStackSize ( ::backtrace ( array, maxStackLevels - 1 ) );

  if ( actualStackSize >= 1 )
  {
    ::backtrace_symbols_fd ( array, actualStackSize, STDERR_FILENO );
    ::fprintf ( stderr, "\n" );
    ::fflush ( stderr );

    ::backtrace_symbols_fd ( array, actualStackSize, STDOUT_FILENO );
    ::fprintf ( stdout, "\n" );
    ::fflush ( stdout );
  }

#else

  StackTrace st;
  const std::string s ( st.toString() );
  std::cerr << s << std::flush;
  std::cout << s << std::flush;

#endif
}
