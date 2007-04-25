
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

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/IO/Redirect.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Types/Types.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <list>
#include <algorithm>
#include <windows.h>

typedef std::list<Usul::Threads::Thread::RefPtr> ThreadList;

namespace Detail
{
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  Mutex *_mutex ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a thread is started.
//
///////////////////////////////////////////////////////////////////////////////

void _threadStarted ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE_STATIC;
  Detail::Guard guard ( *(Detail::_mutex) );

  // Use thread's address to make pseudo random time to sleep.
  const Usul::Types::Uint64 sleep ( ( reinterpret_cast<Usul::Types::Uint64> ( thread ) ) / 10000 );

  std::ostringstream out;
  out << "  Started thread " << std::setw ( 4 ) << thread->id() 
      << " using system thread " << std::setw ( 4 ) << thread->systemId() 
      << ", sleeping " << sleep << '\n';
  std::cout << out.str() << std::flush;

  ::Sleep ( static_cast<DWORD> ( sleep ) );

  // Every 4th thread we cancel.
  const unsigned long id ( thread->id() );
  if ( ( 0 != id ) && ( 0 == ( id % 4 ) ) )
  {
    thread->cancel();
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
  Detail::Guard guard ( *(Detail::_mutex) );

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
  Detail::Guard guard ( *(Detail::_mutex) );

  std::ostringstream out;
  out << "Cancelled thread " << std::setw ( 4 ) << thread->id() 
      << " using system thread " << std::setw ( 4 ) << thread->systemId() << '\n';
  std::cout << out.str() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a thread
//
///////////////////////////////////////////////////////////////////////////////

void _startThread ( ThreadList &threads )
{
  USUL_TRACE_SCOPE_STATIC;
  Usul::Threads::Thread::RefPtr thread ( Usul::Threads::Manager::instance().create() );
  threads.push_back ( thread );
  thread->started   ( Usul::Threads::newFunctionCallback ( _threadStarted   ) );
  thread->finished  ( Usul::Threads::newFunctionCallback ( _threadFinished  ) );
  thread->cancelled ( Usul::Threads::newFunctionCallback ( _threadCancelled ) );
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
  const unsigned int num ( ( args.size() > 1 && false == args[1].empty() ) ? 
                           ( static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( args[1].c_str() ) ) ) ) : 
                           ( 20 ) );

  std::cout << Usul::Strings::format ( "Number of threads to start: ", num, '\n' );
  ThreadList threads;

  // Start some threads.
  {
    for ( unsigned int i = 0; i < num; ++i )
    {
      _startThread ( threads );
      ::Sleep ( 100 );
    }
  }

  // Remove the ones that are done.
  {
    while ( false == threads.empty() )
    {
      ThreadList::iterator i ( std::find_if ( threads.begin(), threads.end(), std::bind2nd ( std::mem_fun ( &Usul::Threads::Thread::hasState ), Usul::Threads::Thread::NOT_RUNNING ) ) );
      if ( threads.end() != i )
      {
        Usul::Threads::Thread::RefPtr thread ( *i );
        if ( true == thread.valid() )
        {
          std::ostringstream out;
          out << "  Removing thread " << std::setw ( 4 ) << thread->id() 
              << " using system thread " << std::setw ( 4 ) << thread->systemId() 
              << ", address " << thread << '\n';
          std::cout << out.str() << std::flush;
        }
        threads.erase ( i );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  Detail::_mutex = 0x0;
  Usul::Threads::Mutex::createFunction  ( 0x0 );
  Usul::Threads::Manager::instance().factory ( 0x0 );
  Usul::Trace::Print::execute ( "</functions>\n" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
  ::srand ( static_cast < unsigned int > ( ::time ( 0x0 ) ) );

  Usul::Trace::Print::execute ( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" );
  Usul::Trace::Print::execute ( "<functions>\n" );

  Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex );
  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

  Detail::_mutex = Usul::Threads::Mutex::create();

  Usul::Functions::safeCall ( _test, "1824980904" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  //Usul::IO::Redirect redirect ( "output.txt", true, true );
  std::ofstream trace ( "trace.xml" );
  Usul::Trace::Print::stream ( &trace );
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  Usul::Functions::safeCall ( _run,   "7106715220" );
  Usul::Functions::safeCall ( _clean, "1461687444" );

  //std::cin.get();
  return 0;
}
