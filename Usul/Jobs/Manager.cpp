
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
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include <algorithm>

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
          this->name ( job->name() );

					_finishedCB = Usul::Threads::newFunctionCallback ( Usul::Adaptors::memberFunction ( this, &Task::_taskFinished ) );
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

Manager::Manager ( unsigned int poolSize, bool lazyStart ) :
  _mutex     (),
  _pool      ( new Usul::Threads::Pool ( poolSize, lazyStart ) ),
  _jobFinishedListeners ()
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
    Manager::init ( Usul::Threads::Pool::DEFAULT_NUM_THREADS, true );
  }
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton construction with given size.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::init ( unsigned int poolSize, bool lazyStart )
{
  USUL_TRACE_SCOPE_STATIC;
  Manager::destroy();
  _instance = new Manager ( poolSize, lazyStart );
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a job to the container.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addJob ( Job *job )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( 0x0 != job ) && ( true == _pool.valid() ) )
  {
    job->_setId ( this->nextJobId() );
    Usul::Jobs::Detail::Task::RefPtr task ( new Usul::Jobs::Detail::Task ( job, this ) );
    _pool->addTask ( job->priority(), task.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the queued job. Has no effect on running jobs.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeQueuedJob ( Job *job )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( 0x0 != job ) && ( true == _pool.valid() ) )
  {
    Usul::Threads::Pool::TaskHandle task ( job->priority(), job->id() );
    _pool->removeQueuedTask ( task );
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
    _pool->waitForTasks();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the thread pool.
//  Depreciated. Use init() instead. Delete this after a while (25-Nov-2007).
//
///////////////////////////////////////////////////////////////////////////////
#if 0
void Manager::poolResize ( unsigned int numThreads )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Should be true.
  USUL_ASSERT ( 1 == _pool->refCount() );

  // To minimize the number of threads, delete existing pool first.
  _pool = 0x0;

  // Now make new pool.
  _pool = new ThreadPool ( numThreads );
}
#endif

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
    _pool->cancel();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel the given job.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::cancel ( Job *job )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( 0x0 != job ) && ( true == _pool.valid() ) )
  {
    this->removeQueuedJob ( job );
    job->cancel();
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
    _pool->clearQueuedTasks();
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
//  Helper function to remove the listener.
//
///////////////////////////////////////////////////////////////////////////////
namespace Usul {
namespace Jobs {
namespace Helper
{
  template < class Listeners, class MutexType > void removeListener ( Listeners &listeners, Usul::Interfaces::IUnknown *caller, MutexType &mutex )
  {
    typedef typename Listeners::value_type::element_type InterfaceType;
    typedef Usul::Threads::Guard<MutexType> Guard;
    typedef typename Listeners::iterator Itr;
    
    USUL_TRACE_SCOPE_STATIC;
    
    typename InterfaceType::QueryPtr listener ( caller );
    if ( true == listener.valid() )
    {
      Guard guard ( mutex );
      typename InterfaceType::RefPtr value ( listener.get() );
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
  template < class Listeners, class MutexType > void addListener ( Listeners &listeners, Usul::Interfaces::IUnknown *caller, MutexType &mutex )
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
      listeners.push_back ( typename InterfaceType::RefPtr ( listener.get() ) );
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

void Manager::addJobFinishedListener ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Helper::addListener ( _jobFinishedListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a job finished listener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeJobFinishedListener ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Helper::removeListener ( _jobFinishedListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A job has finished.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::_jobFinished ( Job* job_ )
{
  USUL_TRACE_SCOPE;

  // Make a copy to reduce risk of deadlocks.
  JobFinishedListeners listeners ( Usul::Threads::Safe::get ( this->mutex(), _jobFinishedListeners ) );
  Usul::Jobs::Job::RefPtr job ( job_ );

  // Notify the listeners.
  std::for_each ( listeners.begin(), listeners.end(), std::bind2nd ( std::mem_fun ( &IJobFinishedListener::jobFinished ), job.get() ) );
}
