
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job manager class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Jobs/Manager.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

#include "boost/bind.hpp"
#include "boost/thread.hpp"

#include <algorithm>
#include <ctime>

using namespace Usul::Jobs;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

Manager *Manager::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Internal task class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Jobs
  {
    namespace Detail
    {
      class Task : public Usul::Threads::Task
      {
      public:

        typedef Usul::Threads::Task BaseClass;

        Task ( Usul::Jobs::Job *job, Manager* manager ) : 
        BaseClass ( job->id(), boost::bind ( &Job::_threadStarted, job ), Task::Callback(), boost::bind ( &Job::_threadCancelled, job ), boost::bind ( &Job::_threadError, job ) ), 
          _job ( job ),
          _manager ( manager )
        {
          _finishedCB = boost::bind ( &Task::_taskFinished, this );

          BaseClass::name ( ( true == _job.valid() ) ? _job->name() : std::string() );
        }

        virtual std::string name() const
        {
          return ( ( true == _job.valid() ) ? _job->name() : BaseClass::name() );
        }

      protected:

        virtual ~Task()
        {
        }
        
        void _taskFinished()
        {
          // Call the job's callback first.
          if ( _job.valid() )
            _job->_threadFinished();
          
          // Let the manager know.
          if ( 0x0 != _manager )
            _manager->_jobFinished ( _job );
          
          _finishedCB = Task::Callback();
        }

      private:

        Usul::Jobs::Job::RefPtr _job;
        Manager *_manager;
      };
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager ( const std::string &name, unsigned int poolSize ) :
  _mutex     (),
  _pool      ( name, poolSize ),
  _jobFinishedListeners(),
  _log       ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( boost::bind ( &Manager::_destroy, this ), "3016860991" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton construction.
//
///////////////////////////////////////////////////////////////////////////////

Manager &Manager::instance()
{
  USUL_TRACE_SCOPE_STATIC;
  if ( 0x0 == _instance )
  {
    Manager::init ( "Usul::Jobs::Manager::instance()", boost::thread::hardware_concurrency() );
  }
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton construction with given size.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::init ( const std::string &name, unsigned int poolSize )
{
  USUL_TRACE_SCOPE_STATIC;
  Manager::destroy();
  _instance = new Manager ( name, poolSize );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton destruction.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::destroy()
{
  USUL_TRACE_SCOPE_STATIC;
  delete _instance;
  _instance = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the members.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_destroy()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _log = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a job to the container.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addJob ( Job::RefPtr job )
{
  USUL_TRACE_SCOPE;

  if ( true == job.valid() )
  {
    job->_setId ( this->nextJobId() );
    Usul::Jobs::Detail::Task::RefPtr task ( new Usul::Jobs::Detail::Task ( job.get(), this ) );

    this->_logEvent ( "Adding job", job );
    {
      Guard guard ( this );
      _pool.addTask ( job->priority(), task.get() );
    }
    this->_logEvent ( "Done adding job", job );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the queued job. Has no effect on running jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeQueuedJob ( Job::RefPtr job )
{
  USUL_TRACE_SCOPE;

  if ( true == job.valid() )
  {
    Usul::Threads::Pool::TaskHandle task ( job->priority(), job->id() );
    this->_logEvent ( "Removing queued job", job );
    {
      Guard guard ( this );
      _pool.removeQueuedTask ( task );
    }
    this->_logEvent ( "Done removing queued job", job );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mutex. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Mutex &Manager::mutex() const
{
  USUL_TRACE_SCOPE;
  return _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the next job id. This will also increment the internal counter.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Manager::nextJobId()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _pool.nextTaskId();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all jobs to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::wait()
{
  USUL_TRACE_SCOPE;
  this->_logEvent ( "Waiting for tasks... " );
  {
    Guard guard ( this );
    _pool.waitForTasks();
  }
  this->_logEvent ( "Done waiting for tasks" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the thread pool size.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Manager::poolSize() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _pool.numThreads();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel all threads and pending tasks in the pool, which cancels the jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::cancel()
{
  USUL_TRACE_SCOPE;
  this->_logEvent ( "Canceling thread pool..." );
  {
    Guard guard ( this );
    _pool.cancel();
  }
  this->_logEvent ( "Done canceling thread pool" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel the given job.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::cancel ( Job::RefPtr job )
{
  USUL_TRACE_SCOPE;

  if ( true == job.valid() )
  {
    this->removeQueuedJob ( job );
    this->_logEvent ( "Canceling job", job );
    job->cancel();
    this->_logEvent ( "Done canceling job", job );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear any jobs that are queued, but not running.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::clearQueuedJobs()
{
  USUL_TRACE_SCOPE;

  // Clear all queued tasks.
  this->_logEvent ( "Clearing queued tasks" );
  {
    Guard guard ( this );
    _pool.clearQueuedTasks();
  }
  this->_logEvent ( "Done clearing queued tasks" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Number of jobs queued.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Manager::numJobsQueued() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _pool.numTasksQueued();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Number of jobs executing.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Manager::numJobsExecuting() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _pool.numTasksExecuting();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Number of jobs.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Manager::numJobs() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( this->numJobsQueued() + this->numJobsExecuting() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the names of the executing jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::executingNames ( Strings &names ) const
{
  Guard guard ( this );
  _pool.executingNames ( names );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A job has finished.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_jobFinished ( Job::RefPtr job )
{
  USUL_TRACE_SCOPE;

  // Notify the listeners.
  _jobFinishedListeners ( job.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the log.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::logSet ( LogPtr lp )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _log = lp;
  _pool.logSet ( lp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the log.
//
///////////////////////////////////////////////////////////////////////////////

Manager::LogPtr Manager::logGet()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return LogPtr ( _log );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Log the event.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_logEvent ( const std::string &s, Job::RefPtr job )
{
  USUL_TRACE_SCOPE;

  LogPtr file ( this->logGet() );
  if ( ( false == s.empty() ) && ( true == file.valid() ) )
  {
    if ( true == job.valid() )
    {
      file->write ( Usul::Strings::format ( "clock: ", ::clock(), ", system thread: ", Usul::Threads::currentThreadId(), ", id: ", job->id(), ", priority: ", job->priority(), ", address: ", job.get(), ", manager: ", this, ", name: ", job->name(), ", event: ", s ) );
    }
    else
    {
      file->write ( Usul::Strings::format ( "clock: ", ::clock(), ", system thread: ", Usul::Threads::currentThreadId(), ", event: ", s ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if a higher-priority job is waiting.
//
///////////////////////////////////////////////////////////////////////////////

bool Manager::isHigherPriorityJobWaiting ( int priority ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _pool.isHigherPriorityTaskWaiting ( priority );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Manager::name() const
{
  USUL_TRACE_SCOPE;
  return _pool.name();
}
