
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

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Pool.h"
#include "Usul/Trace/Trace.h"

#include <fstream>


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
                           ( 10 ) );
  std::cout << Usul::Strings::format ( "Number of threads to start: ", num, '\n' );

  // Make the thread pool.
  Usul::Threads::Pool::ValidRefPtr pool ( new Usul::Threads::Pool ( num ) );
#if 0
  // Remove the ones that are done.
  while ( false == Usul::Threads::Manager::instance().empty() )
  {
    Usul::Threads::Manager::instance().purge();
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

void _clean()
{
  Usul::Threads::Mutex::createFunction  ( 0x0 );
  Usul::Threads::Manager::destroy();
  Usul::Trace::Print::execute ( "</functions>\n" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
  Usul::Trace::Print::execute ( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" );
  Usul::Trace::Print::execute ( "<functions>\n" );

  Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex );
  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

  Usul::Functions::safeCall ( _test, "3057710340" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  std::ofstream trace ( "trace.xml" );
  Usul::Trace::Print::stream ( &trace );
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  Usul::Functions::safeCall ( _run,   "6124470160" );
  Usul::Functions::safeCall ( _clean, "3242794296" );

  return 0;
}
