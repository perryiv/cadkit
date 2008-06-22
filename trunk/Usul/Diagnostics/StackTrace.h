
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
#endif
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <unistd.h>


namespace Usul {
namespace Diagnostics {


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

    #endif
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Print the function signatures to stdout and stderr.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void print()
  {
    #ifdef __GNUC__

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

    #endif
  }

private:

  Container _c;
};


} // namespace Diagnostics
} // namespace Usul


#endif // _USUL_DIAGNOSTICS_STACK_TRACE_H_
