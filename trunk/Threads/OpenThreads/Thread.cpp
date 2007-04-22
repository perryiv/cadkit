
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Threads/OpenThreads/Thread.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "OpenThreads/Thread"

using namespace Threads::OT;


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete thread class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class OpenThread : public OpenThreads::Thread
  {
  public:

    typedef OpenThreads::Thread BaseClass;

    OpenThread ( Threads::OT::Thread *thread ) : BaseClass(),
      _thread ( thread )
    {
      USUL_TRACE_SCOPE;
    }

    ~OpenThread()
    {
      USUL_TRACE_SCOPE;
      // Nothing to delete, we do not own the thread object.
    }

    virtual void run()
    {
      USUL_TRACE_SCOPE;
      if ( 0x0 != _thread )
      {
        Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( _thread, &Threads::OT::Thread::_execute ) );
      }
    }

  private:

    Threads::OT::Thread *_thread;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Thread::Thread() : BaseClass(),
  _thread ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Thread::~Thread()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Thread::_destroy ), "3716194294" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_destroy()
{
  USUL_TRACE_SCOPE;
  OpenThreads::Thread *t ( _thread );
  _thread = 0x0;

  if ( 0x0 != t )
  {
    // If the thread is still running then wait for it.
    if ( true == t->isRunning() )
    {
      for ( unsigned int i = 0; i < 10; ++i )
      {
        if ( true == t->isRunning() )
        {
          OpenThreads::Thread::microSleep ( 100000 ); // One tenth of a second.
        }
      }
    }
    delete t;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Kill the thread.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::kill()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Thread::_destroy ), "1854115959" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start this thread.
//
///////////////////////////////////////////////////////////////////////////////

void Thread::_start()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 == _thread )
  {
    _thread = new Detail::OpenThread ( this );
  }

  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( _thread, &Detail::OpenThread::start ), "2262084867" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Creation function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Threads
{
  namespace OT
  {
    Usul::Threads::Thread *newOpenThreadsThread()
    {
      USUL_TRACE_SCOPE;
      return new Thread();
    }
  };
};
