
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Threads/OpenThreads/Thread.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"
#include "Usul/Trace/Trace.h"

#include "OpenThreads/Thread"

#include <iostream>

using namespace Threads::OT;


///////////////////////////////////////////////////////////////////////////////
//
//  Beginning of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

namespace Threads {
namespace OT {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Concrete thread class.
//
///////////////////////////////////////////////////////////////////////////////

class OpenThread : public OpenThreads::Thread
{
public:

  typedef OpenThreads::Thread BaseClass;

  OpenThread ( Threads::OT::Thread *thread ) : BaseClass(),
    _thread ( thread )
  {
    USUL_TRACE_SCOPE;
  }

  virtual ~OpenThread()
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


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to wait for a thread to stop running.
//
///////////////////////////////////////////////////////////////////////////////

void wait ( unsigned int ierations, unsigned int seconds, Usul::Threads::Thread *usulThread, OpenThreads::Thread *openThread )
{
  if ( 0x0 == usulThread || 0x0 == openThread )
    return;

  // If the thread is still running then wait for it.
  if ( true == openThread->isRunning() )
  {
    for ( unsigned int i = 0; i < ierations; ++i )
    {
      if ( true == openThread->isRunning() )
      {
        // Many threads may call cout, so formatting the string first will 
        // ensure this line stays together.
        std::cout << Usul::Strings::format 
          ( "Warning 4624401900: waiting for thread ", usulThread, " to stop running, id: ", 
            usulThread->id(), ", OpenThreads id: ", openThread->getThreadId(), '\n' );
        std::cout << std::flush;

        // Wait here...
        Usul::System::Sleep::seconds ( seconds );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to delete a thread.
//
///////////////////////////////////////////////////////////////////////////////

void deleteThread ( OpenThreads::Thread *thread )
{
  delete thread;
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace Detail.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace Detail
} // namespace OT
} // namespace Threads


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Thread::Thread ( const std::string &name ) : BaseClass ( name ),
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
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Threads::OT::Thread::_destroy ), "3716194294" );
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

  // Handle no thread (or a killed thread).
  if ( 0x0 == t )
    return;

  // If the thread is still running then wait for it.
  Detail::wait ( 10, 1, this, t );

  // If it's not running then delete it.
  if ( false == t->isRunning() )
  {
    // By detaching we make calls to join() do nothing.
    t->detach();

    // Delete the OpenThreads object.
    Usul::Functions::safeCallV1 ( &Threads::OT::Detail::deleteThread, t, "3646441490" );
  }

  // Otherwise, it's a leak, which is better than bringing down the house.
  else
  {
    std::cout << Usul::Strings::format 
      ( "Warning 3096769450: memory leak for thread ", this, " because it failed to stop running, id: ", 
        this->id(), ", OpenThreads id: ", t->getThreadId(), '\n' );
  }
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
//  See if it's ok to delete this instance.
//
///////////////////////////////////////////////////////////////////////////////

bool Thread::isIdle() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // See if there is a running thread.
  if ( 0x0 != _thread && true == _thread->isRunning() )
  {
    //std::cout << Usul::Strings::format ( "Thread is still running: ", this->name(), ", ID: ", this->id(), ", Address: ", this ) << std::endl;
    return false;
  }

  // See what the base class has to say.
  return BaseClass::isIdle();
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
    Usul::Threads::Thread::RefPtr newOpenThreadsThread ( const std::string &name )
    {
      USUL_TRACE_SCOPE_STATIC;
      Usul::Threads::Thread::RefPtr thread ( new Thread ( name ) );
      return thread;
    }
  };
};
