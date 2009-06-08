
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
//  Test program for thread pool.
//
///////////////////////////////////////////////////////////////////////////////

#include "Threads/OpenThreads/Thread.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Pool.h"
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

  std::ostringstream out;
  out << "  Started thread " << std::setw ( 4 ) << thread->id() 
      << " using system thread " << std::setw ( 4 ) << thread->systemId() 
      << ", sleeping " << sleep << '\n';
  std::cout << out.str() << std::flush;

  Usul::System::Sleep::milliseconds ( sleep );

  // Every 4th thread we cancel.
  const unsigned long id ( thread->id() );
  if ( ( 0 != id ) && ( 0 == ( id % 4 ) ) )
  {
    thread->cancel();
  }

  // Every 5th thread we generate an error.
  if ( ( 0 != id ) && ( 0 == ( id % 5 ) ) )
  {
    throw std::runtime_error ( "Error 1954614090: Generating error for testing" );
  }
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
//  Add a task.
//
///////////////////////////////////////////////////////////////////////////////

void _addTask ( Usul::Threads::Pool *pool )
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 != pool )
  {
    Usul::Threads::Callback::RefPtr started   ( Usul::Threads::newFunctionCallback ( _threadStarted   ) );
    Usul::Threads::Callback::RefPtr finished  ( Usul::Threads::newFunctionCallback ( _threadFinished  ) );
    Usul::Threads::Callback::RefPtr cancelled ( Usul::Threads::newFunctionCallback ( _threadCancelled ) );
    Usul::Threads::Callback::RefPtr error     ( Usul::Threads::newFunctionCallback ( _threadError     ) );
    pool->addTask ( 0, started.get(), finished.get(), cancelled.get(), error.get() );
  }
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

  // The number of tasks to start.
  const unsigned int num ( ( args.size() > 1 && false == args[1].empty() ) ? 
                           ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[1].c_str() ) ) ) ) : 
                           ( 10 ) );
  std::cout << Usul::Strings::format ( "Number of tasks: ", num, '\n' );

  // The pool size.
  const unsigned int size ( ( args.size() > 2 && false == args[2].empty() ) ? 
                            ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[2].c_str() ) ) ) ) : 
                            ( 10 ) );
  std::cout << Usul::Strings::format ( "Pool size: ", size, '\n' );

  // Fill random number vector now. Calling rand() in the child threads is unreliable.
  Detail::randomNumbers.resize ( size * 2 );
  std::generate ( Detail::randomNumbers.begin(), Detail::randomNumbers.end(), ::rand );

  // Make the thread pool.
  Usul::Threads::Pool::ValidRefPtr pool ( new Usul::Threads::Pool ( size ) );

  for ( unsigned int i = 0; i < num; ++i )
  {
    _addTask ( pool.get() );
  }

  // Wait for all tasks to finish.
  pool->waitForTasks();
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

  Usul::Functions::safeCall ( _test,  "6124470160" );
  Usul::Functions::safeCall ( _clean, "3242794296" );

  //std::cout << "Finished test!" << std::endl;
  //std::cin.get();

  return 0;
}
