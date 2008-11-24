
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for managing processes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Threads/OpenThreads/Thread.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Process.h"
#include "Usul/System/Sleep.h"

#include <algorithm>
#include <fstream>
#include <list>


///////////////////////////////////////////////////////////////////////////////
//
//  Run the child process.
//
///////////////////////////////////////////////////////////////////////////////

void _runChild()
{
  const Usul::System::Process::ID id ( Usul::System::Process::currentProcessId() );

  // Use this make it write to files or not.
#if 0
  const std::string file ( Usul::Strings::format ( "testing_process_", id, ".txt" ) );
  std::ofstream stream ( file.c_str() );
  std::ostream &out ( stream );
#else
  std::ostream &out ( std::cout );
#endif

  Usul::CommandLine::Arguments::Args args ( Usul::CommandLine::Arguments::instance().args() );

  out << Usul::Strings::format ( "Running child ", id, " with command-line: " ) << std::flush;
  std::copy ( args.begin(), args.end(), std::ostream_iterator<std::string> ( out ) );
  out << std::endl;

  for ( unsigned int i = 0; i < 10; ++i )
  {
    out << Usul::Strings::format ( "Child process ", id, " going to sleep" ) << std::endl;
    Usul::System::Sleep::milliseconds 
      ( Usul::Convert::Type<std::string,unsigned int>::convert ( args.at ( 1 ) ) );
  }

  out << Usul::Strings::format ( "Exiting child ", id ) << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the parent.
//
///////////////////////////////////////////////////////////////////////////////

void _runParent()
{
  std::cout << "Running parent" << std::endl;

  const std::string program ( Usul::CommandLine::Arguments::instance().program() );

  typedef Usul::System::Process::ValidRefPtr ProcessPtr;
  typedef std::list<ProcessPtr> Processes;
  Processes processes;

  // Start some processes.
  {
    for ( unsigned int i = 0; i < 10; ++i )
    {
      ProcessPtr process ( new Usul::System::Process ( program, " 500" ) );
      std::cout << Usul::Strings::format ( "Parent just started child ", process->id() ) << std::endl;
      processes.push_back ( process );
    }
  }

  // Loop and wait for them to finish.
  while ( false == processes.empty() )
  {
    Processes::iterator i = processes.begin();
    while ( i != processes.end() )
    {
      ProcessPtr process ( *i );
      if ( false == process->isRunning() )
      {
        Processes::iterator eraseMe ( i );
        std::cout << Usul::Strings::format ( "Child process ", i->get()->id(), " exited" ) << std::endl;
        ++i;
        processes.erase ( eraseMe );
      }
      if ( i != processes.end() )
      {
        ++i;
      }
    }
    Usul::System::Sleep::milliseconds ( 100 );
  }

  // Start a process and wait for it.
  {
    for ( unsigned int i = 0; i < 5; ++i )
    {
      ProcessPtr process ( new Usul::System::Process ( program, " 100" ) );
      std::cout << Usul::Strings::format ( "Parent just started child ", process->id() ) << std::endl;
      process->wait();
      std::cout << Usul::Strings::format ( "Child process ", process->id(), " exited" ) << std::endl;
    }
  }

  // Start some processes and then stop them.
  {
    processes.clear();
    {
      for ( unsigned int i = 0; i < 10; ++i )
      {
        ProcessPtr process ( new Usul::System::Process ( program, " 10000" ) );
        std::cout << Usul::Strings::format ( "Parent just started child ", process->id() ) << std::endl;
        processes.push_back ( process );
      }
    }
    {
      for ( Processes::iterator i = processes.begin(); i != processes.end(); ++i )
      {
        ProcessPtr process ( *i );
        process->stop();
        std::cout << Usul::Strings::format ( "Parent just stopped child ", process->id() ) << std::endl;
      }
    }
  }

  std::cout << "Exiting parent" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _runTest()
{
  Usul::CommandLine::Arguments::Args args ( Usul::CommandLine::Arguments::instance().args() );
  if ( args.size() > 1 )
  {
    ::_runChild();
  }
  else
  {
    ::_runParent();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  Usul::CommandLine::Arguments::instance().set ( argc, argv );
  Usul::Functions::safeCall ( &::_runTest );
  return 0;
}
