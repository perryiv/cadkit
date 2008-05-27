
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERRORS_SIGNAL_FUNCTIONS_H_
#define _USUL_ERRORS_SIGNAL_FUNCTIONS_H_

#ifdef __GNUC__
# include <execinfo.h>
# include <signal.h>
#endif

#include <sstream>
#include <fstream>


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
  if ( SIGSEGV != num )
    return;

  if ( false == Detail::_programName.empty() )
    return;

  std::ostringstream out;

  out << "--------------------------------------------" << std::endl;
  out << "-------        SIGSEGV caught!       -------" << std::endl;
  out << "------- Printing the function stack. -------" << std::endl;
  out << "--------------------------------------------" << std::endl;

#ifdef  __GNUC__

  // See: http://www.ibm.com/developerworks/linux/library/l-cppexcep.html

  const int maxStackLevels ( 1000 );
  void * array[maxStackLevels];
  const int actualStackSize ( ::backtrace ( array, maxStackLevels ) );
  char **symbols ( ::backtrace_symbols ( array, actualStackSize ) );

  for ( unsigned int i = 0; i < actualStackSize; ++i )
    out << symbols[i] << std::endl;

  ::free ( symbols );

#endif

  out << "--------------------------------------------" << std::endl;
  out << "-------   Done printing the stack.   -------" << std::endl;
  out << "--------------------------------------------" << std::endl;

  out << "\nCalling abort()..." << std::endl;


  // Print to file.
  const std::string file ( Usul::Strings::format ( Usul::File::base ( Detail::_programName ), ".stack" ) );
  std::ofstream dump ( file.c_str() );
  dump << out.str();

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
