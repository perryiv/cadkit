
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
//  Test program for job manager.
//
///////////////////////////////////////////////////////////////////////////////

#include "TestJob.h"

#include "Threads/OpenThreads/Thread.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <fstream>
#include <algorithm>

namespace Detail
{
  typedef std::vector<unsigned long> RandomNumbers;
  RandomNumbers randomNumbers;
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

  // The number of jobs to start.
  const unsigned int num ( ( args.size() > 1 && false == args[1].empty() ) ? 
                           ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[1].c_str() ) ) ) ) : 
                           ( 10 ) );
                           std::cout << "Number of jobs: " << num << std::endl;

  // The pool size.
  const unsigned int size ( ( args.size() > 2 && false == args[2].empty() ) ? 
                            ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[2].c_str() ) ) ) ) : 
                            ( 10 ) );
                            std::cout << "Pool size: " << size << std::endl;

  // Fill random number vector now. Calling rand() in the child threads is unreliable.
  Detail::randomNumbers.resize ( num * 2 );
  std::generate ( Detail::randomNumbers.begin(), Detail::randomNumbers.end(), ::rand );

  // Set the job manager's thread-pool size.
  Usul::Jobs::Manager::init ( size, true );

  // Start the jobs.
  for ( unsigned int i = 0; i < num; ++i )
  {
    Usul::Jobs::Job::RefPtr job ( new TestJob ( Detail::randomNumbers.at(i) ) );
    Usul::Jobs::Manager::instance().addJob ( job.get() );
  }

  // Wait until all jobs finish.
  TRACE_AND_PRINT ( "Waiting for jobs to finish...\n" );
  Usul::Jobs::Manager::instance().wait();
  TRACE_AND_PRINT ( "All jobs have finished.\n" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  USUL_TRACE_SCOPE_STATIC;

  // Clear global list of random numbers.
  Detail::randomNumbers.clear();

  // The job manager has a thread-pool.
  Usul::Jobs::Manager::destroy();

  // There should not be any threads running.
  TRACE_AND_PRINT ( "Waiting for all threads to finish...\n" );
  Usul::Threads::Manager::instance().wait();
  TRACE_AND_PRINT ( "All threads have finished.\n" );

  // Clean up the thread manager.
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

  Usul::Functions::safeCall ( _test,  "3274711482" );
  Usul::Functions::safeCall ( _clean, "1164335400" );

  std::cout << "Finished test!" << std::endl;

  //std::cin.get();
  return 0;
}
