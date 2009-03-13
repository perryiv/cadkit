
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Unix (Linux, OS X) implementation of the Process class.
//
//  Useful websites: 
//    http://www.ibm.com/developerworks/eserver/library/es-MigratingWin32toLinux.html
//    http://www.developertutorials.com/tutorials/linux/processes-threads-050616/page2.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/ProcessUnix.h"
#include "Usul/System/Clock.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Strings/Split.h"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

using namespace Usul::System::Detail;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProcessImpl::ProcessImpl() :
  _id ( -1 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProcessImpl::~ProcessImpl()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a detached process.  This behaves better when trying to kill a process.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace System {
namespace Detail {
  
  pid_t createDetachedProcess()
  {
    // Helpful constants.
    const int READ_PIPE = 0;
    const int WRITE_PIPE = 1;
    
    // Make a pipe for the child proces to write it's process it to.
    int pipeIds[2];
    if ( ::pipe ( pipeIds ) < 0 )
      return -1;
    
    // Create an intermediate process.
    pid_t middlePid ( ::fork() );
    if ( middlePid < 0 )
      return -1;
    else if ( middlePid == 0 )
    {
      // This is the intermediate process.  Create the real child.
      pid_t childPid ( ::fork() );
      if ( childPid == 0 )
      {
        // This is the real child process.
        return 0;
      }
      else
      {
        // Write the child's id to the pipe.
        while ( ::write ( pipeIds[WRITE_PIPE], &childPid, sizeof ( childPid ) ) < 0 && errno == EINTR );
        ::exit ( 0 );
      }
    }
    else
    {
      // We are in the orginal parent.
      
      // Read the process id from the pipe.
      pid_t childId ( -1 );
      while ( ::read ( pipeIds[READ_PIPE], &childId, sizeof ( childId ) ) < 0 && errno == EINTR );
      
      // Wait for the intermediate process to exit and clean it up.
      int status ( 0 );
      while ( ( ::waitpid ( middlePid, &status, 0 ) < 0 ) && ( errno == EINTR) );
      
      // Close the pipe.
      ::close ( pipeIds[READ_PIPE] );
      ::close ( pipeIds[WRITE_PIPE] );
      
      return childId;
    }
    
    return -1;
  }

}
}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for char** argv.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace System {
namespace Detail {

  struct Argv
  {
    Argv ( const std::string& executable, const std::string& arguments ) :
      argv ( 0x0 ),
      argc ( 0 )
    {
      typedef std::vector<std::string> Args;
      Args args;
      Usul::Strings::split ( arguments, " ", false, args );
        
      // The number of elements to malloc for argv (One for the exe and one for the null character).
      const int numArgs ( args.size() + 2 );
        
      argv = (char**) ::malloc ( sizeof ( char** ) * numArgs );
      char ** tArgv ( argv );
        
      *tArgv = ::strdup ( executable.c_str() );
      ++tArgv;
        
      for ( unsigned int i = 0; i < args.size(); ++i )
      {
        *tArgv = ::strdup ( args[i].c_str() );
        ++tArgv;
      }
      
      *tArgv = 0x0;
      
      argc = numArgs - 1;
    }
    
    ~Argv()
    {
      for ( int i = 0; i < argc; ++i )
      {
        ::free ( argv[i] );
      }
      
      ::free ( argv );
    }
    
    char **argv;
    int argc;
  };
  
}
}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the process.
//
///////////////////////////////////////////////////////////////////////////////

bool ProcessImpl::start ( const std::string &executable, 
                          const std::string &arguments )
{
  pid_t childId ( Detail::createDetachedProcess() );
  if ( -1 == childId )
  {
    return false;
  }
  else if ( 0 == childId )
  {
    // We are now in the child process.
    
    Argv args ( executable, arguments );
    if ( ::execv ( args.argv[0], args.argv ) < 0 )
    {
      std::cout << "Error 3962811128: Could not start process.  Reason: " << ::strerror ( errno ) << "." << std::endl;
      ::exit ( -1 );
    }
    
    // We never get to this part of the code.
    ::exit ( 0 );
  }
  else
  {
    // We are in the parent process.
    _id = childId;
  }
  
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current process's id.
//
///////////////////////////////////////////////////////////////////////////////

ProcessImpl::ID ProcessImpl::currentProcessId()
{
  USUL_TRACE_SCOPE_STATIC;
  return ::getpid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the system's ID for this object.
//
///////////////////////////////////////////////////////////////////////////////

ProcessImpl::ID ProcessImpl::id() const
{
  USUL_TRACE_SCOPE;
  return _id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the process running?
//
///////////////////////////////////////////////////////////////////////////////

bool ProcessImpl::isRunning() const
{
  USUL_TRACE_SCOPE;
  
  if ( -1 == _id )
    return false;
  
  // Call the kill function to see if the process exists.  Since 0 is passed for the signal,
  // no signal is actually sent to the process.
  // See: http://www.ibm.com/developerworks/eserver/library/es-MigratingWin32toLinux.html
  
  if ( ::kill ( _id, 0 ) == -1 && errno == ESRCH ) // No process can found corresponding to the process id.
    return false;
  else
    return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the process.
//
///////////////////////////////////////////////////////////////////////////////

bool ProcessImpl::stop()
{
  USUL_TRACE_SCOPE;
  int result ( ::kill ( _id, SIGKILL ) );
  if ( result < 0 )
  {
    std::cout << "Error 2363232931: Could not kill process " << _id << ".  Reason: " << ::strerror ( errno ) << "." << std::endl;
    return false;
  }
  
  // Kill successful.  Reset our internal id.
  _id = -1;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for the process to finish.
//
///////////////////////////////////////////////////////////////////////////////

void ProcessImpl::wait ( unsigned long milliseconds )
{
  USUL_TRACE_SCOPE;
  const Usul::Types::Uint64 startTime ( Usul::System::Clock::milliseconds() );
  do
  {
    int status ( -1 );
    ::waitpid ( _id, &status, WUNTRACED | WNOHANG );
    
    // Exit the loop if the child isn't running.
    if ( WIFEXITED ( status ) || WIFSIGNALED ( status ) || WIFSTOPPED ( status ) )
    {
      break;
    }
    
  } while ( ( Usul::System::Clock::milliseconds() - startTime ) < milliseconds );
}
