
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SIGNALS_HANDLER_H_
#define _USUL_SIGNALS_HANDLER_H_

#include "Usul/Strings/Format.h"
#include "Usul/Threads/ThreadId.h"

#ifdef __GNUC__
# include <signal.h>
#endif
#include <string>


namespace Usul {
namespace Signals {


///////////////////////////////////////////////////////////////////////////////
//
//  Possible signal ids.
//
///////////////////////////////////////////////////////////////////////////////

namespace ID
{
#ifdef __GNUC__

  const int SegmentationViolation  ( SIGSEGV );
  const int AbortSignal            ( SIGABRT );
  const int FloatingPointException ( SIGFPE  );
  const int BusError               ( SIGBUS  );

#else

  // Meaningless values to satisfy the compiler.
  const int SegmentationViolation  ( 0 );
  const int AbortSignal            ( 1 );
  const int FloatingPointException ( 2 );
  const int BusError               ( 3 );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Signal handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ActionType > struct Handler
{
  Handler ( int id ) : 
    _id ( id )
  {
    #ifdef __GNUC__
    #ifndef __APPLE__
    
    struct sigaction sa;
    ::sigemptyset ( &sa.sa_mask );
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = &Handler::_handle;
    ::sigaction ( _id, &sa, 0x0 );

    #endif
    #endif
  }

  ~Handler()
  {
    #ifdef __GNUC__
    #ifndef __APPLE__
    
    struct sigaction sa;
    ::sigemptyset ( &sa.sa_mask );
    sa.sa_flags = 0;
    sa.sa_handler = SIG_DFL; // Restore default handler.
    ::sigaction ( _id, &sa, 0x0 );

    #endif
    #endif
  }

private:

  // No copying or assignment.
  Handler ( const Handler & );
  Handler &operator = ( const Handler & );


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the signal type as a string.
  //
  /////////////////////////////////////////////////////////////////////////////

  static std::string _getDescription ( int num )
  {
    #ifdef __GNUC__

    switch ( num )
    {
    case SIGSEGV:
      return "segmentation violation";
    case SIGABRT:
      return "abort signal";
    case SIGFPE:
      return "floating-point exception";
    default:
      return Usul::Strings::format ( "signal ", num );
    }

    #else

    return std::string();

    #endif
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Handle the signal. For how to use "info" and "secret" see:
  //  http://www.linuxjournal.com/files/linuxjournal.com/linuxjournal/articles/063/6391/6391l3.html
  //
  /////////////////////////////////////////////////////////////////////////////

  #ifdef __GNUC__

  static void _handle ( int num, siginfo_t *info, void *secret )
  {
    const std::string message ( Usul::Strings::format ( "Error 1598305946: ", _getDescription ( num ), " caught in thread ", Usul::Threads::currentThreadId() ) );
    ActionType action;
    action ( message );
  }

  #endif

  int _id;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient macros to help declare a signal handler.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_SIGNAL_HANDLER(action_type,signal_id) \
  Usul::Signals::Handler<action_type> signal_handler_for_##signal_id ( Usul::Signals::ID::signal_id );

} // namespace Signals
} // namespace Usul


#endif // _USUL_SIGNALS_HANDLER_H_
