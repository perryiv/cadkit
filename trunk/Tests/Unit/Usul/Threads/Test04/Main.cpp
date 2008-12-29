
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for read-write mutex.
//
///////////////////////////////////////////////////////////////////////////////

#include "TestJob.h"
#include "TestObject.h"

#include "Threads/OpenThreads/Thread.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Threads/Manager.h"

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

  // The object to hammer on.
  TestObject::RefPtr object ( new TestObject );

  // Start the jobs.
  for ( unsigned int i = 0; i < num; ++i )
  {
    Usul::Jobs::Job::RefPtr job ( new TestJob ( Detail::randomNumbers.at(i), object.get() ) );
    Usul::Jobs::Manager::instance().addJob ( job.get() );
  }

  // Wait until all jobs finish.
  std::cout << "Waiting for jobs to finish...\n" << std::flush;
  Usul::Jobs::Manager::instance().wait();
  std::cout << "All jobs have finished \n" << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  // Clear global list of random numbers.
  Detail::randomNumbers.clear();

  // The job manager has a thread-pool.
  Usul::Jobs::Manager::destroy();

  // There should not be any threads running.
  std::cout << "Waiting for all threads to finish...\n" << std::flush;
  Usul::Threads::Manager::instance().wait();
  std::cout << "All threads have finished \n" << std::flush;

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
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  Usul::Functions::safeCall ( _test,  "2846936252" );
  Usul::Functions::safeCall ( _clean, "2694560263" );

  std::cout << "Finished test!" << std::endl;

  std::cin.get();
  return 0;
}
