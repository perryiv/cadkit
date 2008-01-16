
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
#include "Usul/Scope/Caller.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"

using namespace Usul::Jobs;

USUL_IMPLEMENT_TYPE_ID ( Job );


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
    Usul::Interfaces::IUnknown::QueryPtr unknown ( factory->createProgressBar() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );

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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
  _id = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return this job's thread, which will be null unless the job is running.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Threads::Thread *Job::thread() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _thread.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this job done?
//
///////////////////////////////////////////////////////////////////////////////

bool Job::isDone() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
  return _priority;
}
