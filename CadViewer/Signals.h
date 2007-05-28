
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_SIGNAL_FUNCTIONS_H_
#define _CV_SIGNAL_FUNCTIONS_H_

#ifdef _WIN32
  TODO
#elif __GNUC__
#ifdef HAVE_GLIB
# include <glib.h>
#endif
#elif __sgi
  TODO
#endif


namespace CV {


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

#ifdef _WIN32
  TODO
#elif __GNUC__
#ifdef HAVE_GLIB
  ::g_on_error_stack_trace ( Detail::_programName.c_str() );
#endif
#elif __sgi
  TODO
#endif

  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "-------   Done printing the stack.   -------" << std::endl;
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "\nCalling abort()..." << std::endl;

  // Exit the program.
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


}; // namespace CV


#endif // _CV_SIGNAL_FUNCTIONS_H_
