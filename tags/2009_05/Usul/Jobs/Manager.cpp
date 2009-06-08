
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
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Trace/Trace.h"

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
          BaseClass ( job->id(), job->_startedCB, 0x0, job->_cancelledCB, job->_errorCB ), 
          _job ( job ),
          _manager ( manager )
        {
          BaseClass::name ( ( true == _job.valid() ) ? _job->name() : std::string() );
					_finishedCB = Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Task::_taskFinished ) );
        }

        virtual std::string name() const
        {
          return ( ( true == _job.valid() ) ? _job->name() : BaseClass::name() );
        }

      protected:

        virtual ~Task()
        {
        }
        
        void _taskFinished ( Usul::Threads::Thread* thread )
        {
          // Call the job's callback first.
          if ( _job.valid() && _job->_finishedCB.valid() )
            (*_job->_finishedCB) ( thread );
          
          // Let the manager know.
          if ( 0x0 != _manager )
            _manager->_jobFinished ( _job.get() );
          
          _finishedCB = 0x0;
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

Manager::Manager ( const std::string &name, unsigned int poolSize, bool lazyStart ) :
  _mutex     (),
  _pool      ( new Usul::Threads::Pool ( name, poolSize, lazyStart ) ),
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
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Manager::_destroy ), "3016860991" );
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
    Manager::init ( "Usul::Jobs::Manager::instance()", Usul::Threads::Pool::DEFAULT_NUM_THREADS, true );
  }
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton construction with given size.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::init ( const std::string &name, unsigned int poolSize, bool lazyStart )
{
  USUL_TRACE_SCOPE_STATIC;
  Manager::destroy();
  _instance = new Manager ( name, poolSize, lazyStart );
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

  // Make a copy of the pool pointer.
  Usul::Threads::Pool::RefPtr pool ( Usul::Threads::Safe::get ( this->mutex(), _pool ) );
  USUL_ASSERT ( 2 == pool->refCount() );

  // Set member _pool to null.
  { Guard guard ( this ); _pool = 0x0; }

  // The pool will clear its queued tasks and cancel running threads.
  // It then waits for running threads (jobs) to finish.
  USUL_ASSERT ( 1 == pool->refCount() );
  pool = 0x0;

  // Set member _log to null.
  { Guard guard ( this ); _log = 0x0; }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a job to the container.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addJob ( Job::RefPtr job )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( true == job.valid() ) && ( true == _pool.valid() ) )
  {
    job->_setId ( this->nextJobId() );
    Usul::Jobs::Detail::Task::RefPtr task ( new Usul::Jobs::Detail::Task ( job.get(), this ) );

    this->_logEvent ( "Adding job", job );
    _pool->addTask ( job->priority(), task.get() );
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
  Guard guard ( this );

  if ( ( true == job.valid() ) && ( true == _pool.valid() ) )
  {
    Usul::Threads::Pool::TaskHandle task ( job->priority(), job->id() );
    this->_logEvent ( "Removing queued job", job );
    _pool->removeQueuedTask ( task );
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
  return ( ( true == _pool.valid() ) ? _pool->nextTaskId() : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Wait for all jobs to complete.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::wait()
{
  USUL_TRACE_SCOPE;
  if ( true == _pool.valid() )
  {
    this->_logEvent ( "Waiting for tasks... " );
    _pool->waitForTasks();
    this->_logEvent ( "Done waiting for tasks" );
  }
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
  return ( ( true == _pool.valid() ) ? _pool->numThreads() : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel all threads and pending tasks in the pool, which cancels the jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::cancel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( true == _pool.valid() )
  {
    this->_logEvent ( "Canceling thread pool..." );
    _pool->cancel();
    this->_logEvent ( "Done canceling thread pool" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel the given job.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::cancel ( Job::RefPtr job )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( true == job.valid() ) && ( true == _pool.valid() ) )
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
  Guard guard ( this );

  // Clear all queued tasks.
  if ( true == _pool.valid() )
  {
    this->_logEvent ( "Clearing queued tasks" );
    _pool->clearQueuedTasks();
    this->_logEvent ( "Done clearing queued tasks" );
  }
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
  return ( ( true == _pool.valid() ) ? _pool->numTasksQueued() : 0 );
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
  return ( ( true == _pool.valid() ) ? _pool->numTasksExecuting() : 0 );
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
  if ( true == _pool.valid() )
    _pool->executingNames ( names );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace Jobs {
namespace Helper
{
  template < class Listeners, class MutexType >
  inline void removeListener ( Listeners &listeners, Usul::Interfaces::IUnknown::RefPtr caller, MutexType &mutex )
  {
    typedef typename Listeners::value_type::element_type InterfaceType;
    typedef Usul::Threads::Guard<MutexType> Guard;
    typedef typename Listeners::iterator Itr;
    
    USUL_TRACE_SCOPE_STATIC;
    
    typename InterfaceType::QueryPtr listener ( caller );
    if ( true == listener.valid() )
    {
      Guard guard ( mutex );
      typename InterfaceType::RefPtr value ( listener );
      Itr end ( std::remove ( listeners.begin(), listeners.end(), value ) );
      listeners.erase ( end, listeners.end() );
    }
  }
}
}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add the listener.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace Jobs {
namespace Helper
{
  template < class Listeners, class MutexType >
  inline void addListener ( Listeners &listeners, Usul::Interfaces::IUnknown::RefPtr caller, MutexType &mutex )
  {
    typedef typename Listeners::value_type::element_type InterfaceType;
    typedef Usul::Threads::Guard<MutexType> Guard;
    
    USUL_TRACE_SCOPE_STATIC;
    
    // Don't add twice.
    removeListener ( listeners, caller, mutex );
    
    // Check for necessary interface.
    typename InterfaceType::QueryPtr listener ( caller );
    if ( true == listener.valid() )
    {
      // Block while we add the listener.
      Guard guard ( mutex );
      listeners.push_back ( typename InterfaceType::RefPtr ( listener ) );
    }
  }
}
}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a job finished listener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addJobFinishedListener ( Usul::Interfaces::IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Helper::addListener ( _jobFinishedListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a job finished listener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeJobFinishedListener ( Usul::Interfaces::IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Helper::removeListener ( _jobFinishedListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A job has finished.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_jobFinished ( Job::RefPtr j )
{
  USUL_TRACE_SCOPE;

  // Make a copy to reduce risk of deadlocks.
  JobFinishedListeners listeners ( Usul::Threads::Safe::get ( this->mutex(), _jobFinishedListeners ) );
  Usul::Jobs::Job::RefPtr job ( j );

  this->_logEvent ( "Job finished", job );

  // Notify the listeners.
  std::for_each ( listeners.begin(), listeners.end(), std::bind2nd ( std::mem_fun ( &IJobFinishedListener::jobFinished ), job.get() ) );
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

  if ( true == _pool.valid() )
    _pool->logSet ( lp );
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
  return ( ( true == _pool.valid() ) ? _pool->isHigherPriorityTaskWaiting ( priority ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Manager::name() const
{
  USUL_TRACE_SCOPE;
  Usul::Threads::Pool::RefPtr pool ( 0x0 );
  Usul::Threads::Safe::set ( this->mutex(), _pool, pool );
  return ( ( true == pool.valid() ) ? pool->name() : std::string() );
}
