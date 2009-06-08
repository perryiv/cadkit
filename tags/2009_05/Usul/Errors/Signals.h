
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ERRORS_SIGNAL_FUNCTIONS_H_
#define _USUL_ERRORS_SIGNAL_FUNCTIONS_H_

#include "Usul/Threads/Named.h"

#ifdef __GNUC__
# include <execinfo.h>
# include <pthread.h>
# include <signal.h>
#endif

#include "Usul/File/Path.h"
#include "Usul/Strings/Format.h"

#include <fstream>
#include <iostream>
#include <sstream>


namespace Usul {
namespace Errors {
namespace Signals {


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
  {
    std::cout << "Error 8694474270: wrong signal caught in handler" << std::endl;
    return;
  }

  if ( true == Detail::_programName.empty() )
  {
    std::cout << "Error 1295396200: empty program name in handler" << std::endl;
    return;
  }

  std::ostringstream out;

  out << "--------------------------------------------" << std::endl;
  out << "-------        SIGSEGV caught!       -------" << std::endl;
  out << "------- Printing the function stack. -------" << std::endl;
  out << "--------------------------------------------" << std::endl;

#ifdef  __GNUC__

  // See: http://www.ibm.com/developerworks/linux/library/l-cppexcep.html

  const int maxStackLevels ( 100 );
  void * array[maxStackLevels];
  const int actualStackSize ( ::backtrace ( array, maxStackLevels - 1 ) );
  
  // If there is a stack...
  if ( actualStackSize >= 1 )
  {
    char **symbols ( ::backtrace_symbols ( array, actualStackSize ) );
  
    for ( int i = 0; i < actualStackSize; ++i )
    {
      out << symbols[i] << std::endl;
    }

    ::free ( symbols );
  }
#endif

  out << "--------------------------------------------" << std::endl;
  out << "-------   Done printing the stack.   -------" << std::endl;
  out << "--------------------------------------------" << std::endl;

  // Print to file.
  const std::string file ( Usul::File::fullPath ( Usul::File::base ( Detail::_programName ) + ".stack" ) );
  {
    std::ofstream dump ( file.c_str() );
    dump << out.str();
  }

  // Also notify with stdout.
  std::cout << "Contents of stack trace file: " << file << std::endl;
  std::cout << out.str() << std::endl;

  // Pop up a dialog.
  const std::string command ( "xmessage -file " + file );

  // TODO: Throw an exception instead of aborting.

  // Abort if this is the main thread, otherwise just exit the thread.
  if ( Usul::Threads::Named::instance().is ( Usul::Threads::Names::MAIN ) )
  {
    out << "\nCalling abort()..." << std::endl;
    ::abort();
  }
  else
  {
    // This will likely leave a few mutexes locked...
    out << "\nCalling pthread_exit()..." << std::endl;
    ::pthread_exit ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register signal handlers.
//
///////////////////////////////////////////////////////////////////////////////

inline void registerHandlers ( const std::string &programName )
{
#ifndef __APPLE__ // TODO: Figure out why this won't compile on apple.

  Detail::_programName = Usul::File::fullPath ( programName );
  struct sigaction sa;
  ::sigemptyset ( &sa.sa_mask );
  sa.sa_flags = 0;
  sa.sa_handler = handleSIGSEGV;
  ::sigaction ( SIGSEGV, &sa, 0 );

#endif
}


} // namespace Signals
} // namespace Errors
} // namespace Usul


#endif // _CV_SIGNAL_FUNCTIONS_H_
