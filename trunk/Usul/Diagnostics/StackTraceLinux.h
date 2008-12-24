
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility functions for getting and printing the call stack.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DIAGNOSTICS_STACK_TRACE_LINUX_H_
#define _USUL_DIAGNOSTICS_STACK_TRACE_LINUX_H_

#include <execinfo.h>
#include <unistd.h>
#include <cstdlib>

namespace Usul {
namespace Diagnostics {
namespace Helper {

#define MAX_STACK_LEVELS 1024

/////////////////////////////////////////////////////////////////////////////
//
//  Fill the container with the function signatures.
//
/////////////////////////////////////////////////////////////////////////////

template < class Container > void getStackTrace ( Container &c )
{
  // See http://www.ibm.com/developerworks/linux/library/l-cppexcep.html

  void * array[MAX_STACK_LEVELS];
  const int actualStackSize ( ::backtrace ( array, MAX_STACK_LEVELS - 1 ) );

  if ( actualStackSize >= 1 )
  {
    char **symbols ( ::backtrace_symbols ( array, actualStackSize ) );
    for ( int i = 0; i < actualStackSize; ++i )
    {
      c.insert ( c.end(), symbols[i] );
    }
    ::free ( symbols );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Print the function signatures to stdout and stderr.
//
/////////////////////////////////////////////////////////////////////////////

bool printStackTrace()
{
  // Note: instead of using StackTrace::get() or StackTrace::toString() 
  // we use backtrace_symbols_fd which dumps to standard streams. Even 
  // though it makes the code slightly more complex, this is much better 
  // when the proces is so corrupt that allocating memory is not possible.
  // See http://www.ibm.com/developerworks/linux/library/l-cppexcep.html

  void * array[MAX_STACK_LEVELS];
  const int actualStackSize ( ::backtrace ( array, MAX_STACK_LEVELS - 1 ) );

  if ( actualStackSize >= 1 )
  {
    ::backtrace_symbols_fd ( array, actualStackSize, STDERR_FILENO );
    ::fprintf ( stderr, "\n" );
    ::fflush ( stderr );

    ::backtrace_symbols_fd ( array, actualStackSize, STDOUT_FILENO );
    ::fprintf ( stdout, "\n" );
    ::fflush ( stdout );
  }

  // Return true because we handled printing.
  return true;
}

#undef MAX_STACK_LEVELS

} // namespace Helper
} // namespace Diagnostics
} // namespace Usul


#endif // _USUL_DIAGNOSTICS_STACK_TRACE_LINUX_H_
