
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Jobs/Job.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>

using namespace Usul::Jobs;

USUL_IMPLEMENT_TYPE_ID ( Job );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Job, Job::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Job::Job ( Usul::Interfaces::IUnknown *caller, bool showProgressBar ) : BaseClass(),
  _id          ( 0 ),
  _cancelledCB ( 0x0 ),
  _errorCB     ( 0x0 ),
  _finishedCB  ( 0x0 ),
  _startedCB   ( 0x0 ),
  _thread      ( 0x0 ),
  _done        ( false ),
  _canceled    ( false ),
  _progress    ( static_cast < ProgressBar * > ( 0x0 ) ),
  _label       ( static_cast < StatusBar  * > ( 0x0 ) ),
  _priority    ( 0 )
{
  USUL_TRACE_SCOPE;

  _cancelledCB = Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Job::_threadCancelled ) );
  _errorCB     = Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Job::_threadError     ) );
  _finishedCB  = Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Job::_threadFinished  ) );
  _startedCB   = Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Job::_threadStarted   ) );

  // Check to see if our caller can create our progress bar.
  Usul::Interfaces::IProgressBarFactory::QueryPtr factory ( caller );
  if( factory.valid() )
  {
    Usul::Interfaces::IUnknown::QueryPtr unknown ( factory->createProgressBar().get() );
    _progress = unknown.get();
    _label = unknown.get();

    if ( _progress.valid() && showProgressBar )
      _progress->showProgressBar();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Job::~Job()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Job::_destroy ), "3574964895" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Job::_destroy()
{
  USUL_TRACE_SCOPE;

  _cancelledCB = 0x0;
  _errorCB     = 0x0;
  _finishedCB  = 0x0;
  _startedCB   = 0x0;
  _thread      = 0x0;

  if ( _progress.valid() )
    _progress->hideProgressBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Job::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ICancel::IID:
    return static_cast < Usul::Interfaces::ICancel * > ( this );
  case Usul::Interfaces::ICanceledStateGet::IID:
    return static_cast < Usul::Interfaces::ICanceledStateGet * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to set the thread.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Jobs
  {
    namespace Helper
    {
      class ScopedThread
      {
      public:
        ScopedThread ( Job &job, Usul::Threads::Thread *during, Usul::Threads::Thread *after ) : _job ( job ), _after ( after )
        {
          _job._setThread ( during );
        }
        ~ScopedThread()
        {
          _job._setThread ( _after.get() );
        }
      private:
        Job &_job;
        Usul::Threads::Thread::RefPtr _after;
      };
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to set the done flag.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Jobs
  {
    namespace Helper
    {
      class ScopedDone
      {
      public:
        ScopedDone ( Job &job, bool done ) : _job ( job ), _done ( done )
        {
        }
        ~ScopedDone()
        {
          _job._setDone ( _done );
        }
      private:
        Job &_job;
        bool _done;
      };
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the thread is cancelled.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_threadCancelled ( Thread *thread )
{
  USUL_TRACE_SCOPE;
  ScopedThread scoped ( *this, thread, 0x0 );
  ScopedDone done ( *this, true );
  this->_cancelled();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job is cancelled.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_cancelled()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the thread encounters an error.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_threadError ( Thread *thread )
{
  USUL_TRACE_SCOPE;
  ScopedThread scoped ( *this, thread, 0x0 );
  ScopedDone done ( *this, true );
  this->_error();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job encounters an error.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_error()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the thread finishes normally.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_threadFinished ( Thread *thread )
{
  USUL_TRACE_SCOPE;

  // Declare up here.
  ScopedThread scoped ( *this, thread, 0x0 );
  ScopedDone done ( *this, true );

  // See if we've been cancelled.
  if ( true == this->canceled() )
    this->cancel();

  // Call finish callback.
  this->_finished();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job finishes normally.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_finished()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the thread starts.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_threadStarted ( Thread *thread )
{
  USUL_TRACE_SCOPE;

  this->_setThread ( thread );

  if ( true == this->canceled() )
    this->cancel();

  this->_started();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job starts.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_started()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the thread.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_setThread ( Thread *thread )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _thread = thread;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the done flag.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_setDone ( bool done )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _done = done;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel this job.
//
///////////////////////////////////////////////////////////////////////////////

void Job::cancel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _canceled = true;

  if ( true == _thread.valid() )
  {
    _thread->cancel();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Was the job canceled?
//
///////////////////////////////////////////////////////////////////////////////

bool Job::canceled() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _canceled;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return this job's id.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Job::id() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set this job's id.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_setId ( unsigned int value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _id = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return this job's thread, which will be null unless the job is running.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Threads::Thread::RefPtr Job::thread() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _thread;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this job done?
//
///////////////////////////////////////////////////////////////////////////////

bool Job::isDone() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _done;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void Job::progress ( IUnknown* progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _progress = progress;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

Job::IUnknown* Job::progress()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _progress.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label.
//
///////////////////////////////////////////////////////////////////////////////

void Job::label ( IUnknown* label )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _label = label;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_updateProgress ( unsigned int numerator, unsigned int denominator, bool state )
{
  // If we should...
  if ( state )
  {
    // Report progress.
    if ( _progress.valid() )
    {
      const double n ( static_cast < double > ( numerator ) );
      const double d ( static_cast < double > ( denominator ) );
      const double fraction ( n / d );
      _progress->updateProgressBar ( static_cast < unsigned int > ( fraction * 100 ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label text.
//
///////////////////////////////////////////////////////////////////////////////

void Job::_setLabel ( const std::string& text )
{
  if ( _label.valid() )
    _label->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the priority.
//
///////////////////////////////////////////////////////////////////////////////

void Job::priority ( int value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _priority = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the priority.
//
///////////////////////////////////////////////////////////////////////////////

int Job::priority() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _priority;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for this job to finish. Calling from the job's thread will throw.
//
///////////////////////////////////////////////////////////////////////////////

void Job::wait ( std::ostream *out, unsigned int numLoops, unsigned int sleep )
{
  USUL_TRACE_SCOPE;

  // See if we're already done.
  if ( true == this->isDone() )
  {
    return;
  }

  // Have we been queued?
  if ( 0 != this->id() )
  {
    return;
  }

  // When we get to here we know that:
  // 1. We've been added to a job manager.
  // 2. We're not done executing.

  // Note: Moved the checking of thread ids into the loop because the job may 
  // not have started yet, which means the thread is null. Nevertheless, we 
  // need to wait for it, and we need to keep track of the time (loops) while 
  // we wait.

  // Used to make sure we check the thread id once.
  bool needToCheckThreadId ( true );

  // Loop until it's done or we reach the maximum number of loops allowed.
  for ( unsigned int i = 0; i < numLoops; ++i )
  {
    if ( true == needToCheckThreadId )
    {
      // Do not call from the thread the job is running in.
      Usul::Threads::Thread::RefPtr thread ( this->thread() );
      if ( true == thread.valid() )
      {
        if ( thread->systemId() == Usul::Threads::currentThreadId() )
        {
          throw std::runtime_error ( "Error 1534701658: cannot wait for job in its execution thread" );
        }
        needToCheckThreadId = false;
      }
    }

    if ( true == this->isDone() )
    {
      break;
    }

    if ( 0x0 != out )
    {
      (*out) << Usul::Strings::format ( "Waiting for job ", this->id(), ", loop ", i, " of ", numLoops, ", sleeping for ", sleep, " ms" ) << std::endl;
    }

    Usul::System::Sleep::milliseconds ( sleep );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Overload to return an accurate indication of success.
//
///////////////////////////////////////////////////////////////////////////////

bool Job::success() const
{
  USUL_TRACE_SCOPE;
  return true;
}
