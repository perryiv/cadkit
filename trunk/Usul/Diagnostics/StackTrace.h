
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
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DIAGNOSTICS_STACK_TRACE_H_
#define _USUL_DIAGNOSTICS_STACK_TRACE_H_

#ifdef __GNUC__
# include <execinfo.h>
# include <unistd.h>
#elif _MSC_VER
#include <dbghelp.h>
#endif

#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>


namespace Usul {
namespace Diagnostics {


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize and clean up the symbols. 
//  Make a single instance of this class for the entire process.
//
///////////////////////////////////////////////////////////////////////////////

struct SymbolManager
{
  SymbolManager()
  {
    #ifdef _MSC_VER
    ::SymInitialize ( ::GetCurrentProcess(), NULL, TRUE );
    #endif
  }
  ~SymbolManager()
  {
    #ifdef _MSC_VER
    ::SymCleanup ( ::GetCurrentProcess() );
    #endif
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  StackTrace class.
//
///////////////////////////////////////////////////////////////////////////////

class StackTrace
{
public:

  typedef std::vector < std::string > Container;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default construction grabs the current function stack.
  //
  /////////////////////////////////////////////////////////////////////////////

  StackTrace() : _c()
  {
    StackTrace::get ( _c );
  }

  StackTrace ( const StackTrace &b ) : _c ( b._c )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return internal container.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Container &get() const
  {
    return _c;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return a string.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::string toString() const
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

  static void get ( Container &c )
  {
    #ifdef __GNUC__

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
    DWORD machineType;
    CONTEXT context;
    ::memset ( &context, 0, sizeof ( CONTEXT ) );
    ::RtlCaptureContext ( &context );

    // Initialize stack frame.
    STACKFRAME64 stackFrame;
    ::memset ( &stackFrame, 0, sizeof ( STACKFRAME64 ) );

    // Used to quietly ignore platforms that are not supported.
    bool platformSupported ( true );

    // Initialization of stack frame object varies per platform.
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

      // Needed when getting the frame's "symbol".
      DWORD64 displacement ( 0 );

      // Get the name of the function.
      if ( ::SymGetSymFromAddr64 ( ::GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, symbol ) )
      {
        // Get the undecorated name.
        if ( 0 < ::UnDecorateSymbolName ( symbol->Name, &undecorated[0], undecorated.size() - 1, UNDNAME_COMPLETE ) )
        {
          std::string name ( &undecorated[0] );
          c.insert ( c.end(), name );
        }
      }
    }

    #endif // _MSC_VER
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Print the function signatures to stdout and stderr.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void print()
  {
    #ifdef __GNUC__

    // Note: instead of using StackTrace::get() or StackTrace::toString() 
    // we use backtrace_symbols_fd which dumps to standard streams. Even 
    // though it makes the code slightly more complex, this is much better 
    // when the proces is so corrupt that allocating memory is not possible.

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

private:

  Container _c;
};


} // namespace Diagnostics
} // namespace Usul


#endif // _USUL_DIAGNOSTICS_STACK_TRACE_H_
