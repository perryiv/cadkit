
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for threads.
//
///////////////////////////////////////////////////////////////////////////////

#include "Threads/OpenThreads/Thread.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/IO/Redirect.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

namespace Detail
{
  typedef std::vector<unsigned long> RandomNumbers;
  RandomNumbers randomNumbers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a thread is started.
//
///////////////////////////////////////////////////////////////////////////////

void _threadStarted ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE_STATIC;
  
  const unsigned long sleep ( Detail::randomNumbers.at ( thread->id() ) );

  {
    std::ostringstream out;
    out << "  Started thread " << std::setw ( 4 ) << thread->id() 
        << " using system thread " << std::setw ( 4 ) << thread->systemId() 
        << ", sleeping " << sleep << '\n';
    std::cout << out.str() << std::flush;
  }

  Usul::System::Sleep::milliseconds ( sleep );

  // Every 4th thread we cancel.
  const unsigned long id ( thread->id() );
  if ( ( 0 != id ) && ( 0 == ( id % 4 ) ) )
  {
    thread->cancel();
    return;
  }

  // Every 5th thread we generate an error.
  if ( ( 0 != id ) && ( 0 == ( id % 5 ) ) )
  {
    throw std::runtime_error ( "Error 1954614090: Generating error for testing" );
  }
#if 0
  // Every 6th thread we kill.
  if ( ( 0 != id ) && ( 0 == ( id % 6 ) ) )
  {
    {
      std::ostringstream out;
      out << "  Killing thread " << std::setw ( 4 ) << thread->id() 
          << " using system thread " << std::setw ( 4 ) << thread->systemId() << '\n';
      std::cout << out.str() << std::flush;
    }
    thread->kill();
    {
      std::ostringstream out;
      out << "   Killed thread " << std::setw ( 4 ) << thread->id() 
          << " using system thread " << std::setw ( 4 ) << thread->systemId() << '\n';
      std::cout << out.str() << std::flush;
    }
    return;
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a thread is done.
//
///////////////////////////////////////////////////////////////////////////////

void _threadFinished ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE_STATIC;

  std::ostringstream out;
  out << " Finished thread " << std::setw ( 4 ) << thread->id() 
      << " using system thread " << std::setw ( 4 ) << thread->systemId() << '\n';
  std::cout << out.str() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a thread is cancelled.
//
///////////////////////////////////////////////////////////////////////////////

void _threadCancelled ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE_STATIC;

  std::ostringstream out;
  out << "Cancelled thread " << std::setw ( 4 ) << thread->id() 
      << " using system thread " << std::setw ( 4 ) << thread->systemId() << '\n';
  std::cout << out.str() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a thread encounters an error.
//
///////////////////////////////////////////////////////////////////////////////

void _threadError ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE_STATIC;

  std::ostringstream out;
  out << " Error in thread " << std::setw ( 4 ) << thread->id() 
      << " using system thread " << std::setw ( 4 ) << thread->systemId() << '\n';
  std::cout << out.str() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a thread is deleted.
//
///////////////////////////////////////////////////////////////////////////////

void _threadDestroyed ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE_STATIC;

  std::ostringstream out;
  out << "Destroyed thread " << std::setw ( 4 ) << thread->id() 
      << " using system thread " << std::setw ( 4 ) << thread->systemId() << '\n';
  std::cout << out.str() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a thread
//
///////////////////////////////////////////////////////////////////////////////

void _startThread()
{
  USUL_TRACE_SCOPE_STATIC;
  Usul::Threads::Thread::RefPtr thread ( Usul::Threads::Manager::instance().create() );
  thread->started   ( Usul::Threads::newFunctionCallback ( _threadStarted   ) );
  thread->finished  ( Usul::Threads::newFunctionCallback ( _threadFinished  ) );
  thread->cancelled ( Usul::Threads::newFunctionCallback ( _threadCancelled ) );
  thread->error     ( Usul::Threads::newFunctionCallback ( _threadError     ) );
  thread->destroyed ( Usul::Threads::newFunctionCallback ( _threadDestroyed ) );
  thread->start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _test()
{
  USUL_TRACE_SCOPE_STATIC;

  Usul::CommandLine::Arguments::Args args ( Usul::CommandLine::Arguments::instance().args() );

  // The number of threads to start.
  const unsigned int num ( ( args.size() > 1 && false == args[1].empty() ) ? 
                           ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[1].c_str() ) ) ) ) : 
                           ( 20 ) );
  std::cout << Usul::Strings::format ( "Number of threads to start: ", num, '\n' );

  // The time to sleep between starting threads.
  const unsigned long sleep ( ( args.size() > 2 && false == args[2].empty() ) ? 
                              ( static_cast < unsigned long > ( Usul::Math::absolute ( ::atoi ( args[2].c_str() ) ) ) ) : 
                              ( 100 ) );

  // Fill random number vector now. Calling rand() in the child threads is unreliable.
  Detail::randomNumbers.resize ( num );
  std::generate ( Detail::randomNumbers.begin(), Detail::randomNumbers.end(), ::rand );

  // Start some threads.
  for ( unsigned int i = 0; i < num; ++i )
  {
    _startThread();
    Usul::System::Sleep::milliseconds ( sleep );
  }

  // Remove the ones that are done.
  while ( false == Usul::Threads::Manager::instance().empty() )
  {
    Usul::Threads::Manager::instance().purge();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  Detail::randomNumbers.clear();
  Usul::Threads::Manager::destroy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  ::srand ( 10 );
  Detail::randomNumbers.clear();

  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

  std::ofstream trace ( "trace.csv" );
  Usul::Trace::Print::stream ( &trace );
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  Usul::Functions::safeCall ( _test,  "7106715220" );
  Usul::Functions::safeCall ( _clean, "1461687444" );

  std::cout << "Finished test!" << std::endl;

  std::cin.get();
  return 0;
}
