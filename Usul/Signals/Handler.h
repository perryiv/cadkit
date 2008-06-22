
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
//  Signal handler.
//
///////////////////////////////////////////////////////////////////////////////

template < class ActionType > struct Handler
{
  Handler ( int id ) : 
    _id ( id )
  {
    #ifdef __GNUC__

    struct sigaction sa;
    ::sigemptyset ( &sa.sa_mask );
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = &Handler::_handle;
    ::sigaction ( _id, &sa, 0x0 );

    #endif
  }

  ~Handler()
  {
    #ifdef __GNUC__

    struct sigaction sa;
    ::sigemptyset ( &sa.sa_mask );
    sa.sa_flags = 0;
    sa.sa_handler = SIG_DFL; // Restore default handler.
    ::sigaction ( _id, &sa, 0x0 );

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
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Handle the signal. For how to use "info" and "secret" see:
  //  http://www.linuxjournal.com/files/linuxjournal.com/linuxjournal/articles/063/6391/6391l3.html
  //
  /////////////////////////////////////////////////////////////////////////////

  static void _handle ( int num, siginfo_t *info, void *secret )
  {
    const std::string message ( Usul::Strings::format ( "Error 1598305946: ", _getDescription ( num ), " caught in thread ", Usul::Threads::currentThreadId() ) );
    ActionType action;
    action ( message );
  }

  int _id;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient macros to help declare a signal handler.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_SIGNAL_HANDLER(action_type,id)    \
  Usul::Signals::Handler<action_type> signal_handler_for_##id ( id );


} // namespace Signals
} // namespace Usul


#endif // _USUL_SIGNALS_HANDLER_H_
