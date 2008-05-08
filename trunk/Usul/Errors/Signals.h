
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERRORS_SIGNAL_FUNCTIONS_H_
#define _USUL_ERRORS_SIGNAL_FUNCTIONS_H_

#ifdef __linux
# include <execinfo.h>
# include <signal.h>
#endif


namespace Usul {
namespace Errors {

///////////////////////////////////////////////////////////////////////////////
//
//  File-scope variables.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  std::string _programName;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Signal handler.
//
///////////////////////////////////////////////////////////////////////////////

void handleSIGSEGV ( int num )
{
  assert ( SIGSEGV == num );
  assert ( false == Detail::_programName.empty() );

  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "-------        SIGSEGV caught!       -------" << std::endl;
  std::cout << "------- Printing the function stack. -------" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;

#ifdef __linux

  // See: http://www.ibm.com/developerworks/linux/library/l-cppexcep.html

  void * array[25];
  const int size ( ::backtrace ( array, 25 ) );
  char **symbols ( ::backtrace_symbols ( array, size ) );

  for ( unsigned int i = 0; i < size; ++i )
    std::cout << symbols[i] << std::endl;

  ::free ( symbols );

#endif

  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "-------   Done printing the stack.   -------" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "\nCalling abort()..." << std::endl;

  // Exit the program.
  // TODO: Throw an exception instead of aborting.
  ::abort();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register signal handlers.
//
///////////////////////////////////////////////////////////////////////////////

inline void registerSignalHandlers ( const std::string &programName )
{
  // TODO: Figure out why this won't compile on apple.
#ifndef __APPLE__
  Detail::_programName = programName;
  struct sigaction sa;
  ::sigemptyset ( &sa.sa_mask );
  sa.sa_flags = 0;
  sa.sa_handler = handleSIGSEGV;
  ::sigaction ( SIGSEGV, &sa, 0 );
#endif
}


} // namespace Errors
} // namespace Usul


#endif // _CV_SIGNAL_FUNCTIONS_H_
