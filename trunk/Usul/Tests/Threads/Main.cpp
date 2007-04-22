
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
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Types/Types.h"
#include "Usul/IO/Redirect.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <list>
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
  USUL_TRACE_SCOPE;
  Detail::Guard guard ( *(Detail::_mutex) );
  const Usul::Types::Uint64 sleep ( ( reinterpret_cast<Usul::Types::Uint64> ( thread ) ) / 10000 );
  std::cout << "Started thread " << thread->id() << " using system thread " << thread->systemId() << ", sleeping " << sleep << std::endl;
  ::Sleep ( static_cast<DWORD> ( sleep ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a thread is done.
//
///////////////////////////////////////////////////////////////////////////////

void _threadFinished ( Usul::Threads::Thread *thread )
{
  USUL_TRACE_SCOPE;
  Detail::Guard guard ( *(Detail::_mutex) );
  std::cout << "Finished thread " << thread->id() << " using system thread " << thread->systemId() << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a thread
//
///////////////////////////////////////////////////////////////////////////////

void _startThread ( ThreadList &threads )
{
  USUL_TRACE_SCOPE;
  Usul::Threads::Thread::RefPtr thread ( Usul::Threads::Thread::create() );
  threads.push_back ( thread );
  thread->started  ( Usul::Threads::newFunctionCallback ( _threadStarted ) );
  thread->finished ( Usul::Threads::newFunctionCallback ( _threadFinished ) );
  thread->start();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _test()
{
  USUL_TRACE_SCOPE;

  const unsigned int num ( 100 );
  ThreadList threads;

  // Start some threads.
  for ( unsigned int i = 0; i < num; ++i )
  {
    _startThread ( threads );
    ::Sleep ( 100 );
  }

  // Remove the ones that are done.
  while ( false == threads.empty() )
  {
    threads.remove_if ( std::bind2nd ( std::mem_fun ( &Usul::Threads::Thread::hasState ), Usul::Threads::Thread::NOT_RUNNING ) );
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
  Usul::Threads::Thread::createFunction ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
  ::srand ( static_cast < unsigned int > ( ::time ( 0x0 ) ) );

  Usul::Threads::Mutex::createFunction  ( &Threads::OT::newOpenThreadsMutex  );
  Usul::Threads::Thread::createFunction ( &Threads::OT::newOpenThreadsThread );

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

  Usul::Functions::safeCall ( _run,   "7106715220" );
  Usul::Functions::safeCall ( _clean, "1461687444" );

  std::cin.get();
  return 0;
}
