
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

#ifndef _USUL_JOBS_JOB_MANAGER_CLASS_H_
#define _USUL_JOBS_JOB_MANAGER_CLASS_H_

#include "Usul/File/Log.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Pool.h"

#include "boost/version.hpp"
#if BOOST_VERSION >= 103900
#include "boost/signals2/signal.hpp"
#endif

#include <string>
#include <vector>

namespace Usul {
namespace Jobs {


class USUL_EXPORT Manager
{
public:

  // Useful typedefs.
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Threads::Pool ThreadPool;
  typedef ThreadPool::Strings Strings;
  typedef Usul::File::Log::RefPtr LogPtr;
  
#if BOOST_VERSION >= 103900
  typedef boost::signals2::signal<void ( Job* )> JobFinishedListeners;
#endif

  // Constructor and destructor. Use as a singleton or as individual objects.
  Manager ( const std::string &name, unsigned int poolSize );
  ~Manager();

  // Add a job to the list.
  void                    addJob ( Job::RefPtr );
  
  // Add a job finished listener.
  template<class Slot>
  void                    addJobFinishedListener ( const Slot& subscriber );

  // Cancel the job(s).
  void                    cancel();
  void                    cancel ( Job::RefPtr );

  // Clear any jobs that are queued, but not running.
  void                    clearQueuedJobs();

  // This will delete the singleton instance, if any.
  static void             destroy();

  // Get the names of the executing jobs.
  void                    executingNames ( Strings & ) const;

  // Initialize singleton with given thread-pool size.
  static void             init ( const std::string &name, unsigned int poolSize );

  // Get the singleton.
  static Manager &        instance();

  // See if a higher-priority job is waiting.
  bool                    isHigherPriorityJobWaiting ( int priority ) const;

  // Set/get the log.
  void                    logSet ( LogPtr );
  LogPtr                  logGet();

  // Get the name.
  std::string             name() const;

  // Get the next job id. This will also increment the internal counter.
  unsigned long           nextJobId();

  // Get the number of jobs.
  unsigned int            numJobs() const;
  unsigned int            numJobsExecuting() const;
  unsigned int            numJobsQueued() const;

  // Return the mutex. Use with caution.
  Mutex &                 mutex() const;

  // Get the size and resize the thread pool.
  unsigned int            poolSize() const;
  
  // Remove a job finished listener.
  template<class Slot>
  void                    removeJobFinishedListener ( const Slot& subscriber );

  // Remove the queued job. Has no effect on running jobs.
  void                    removeQueuedJob ( Job::RefPtr );

  // Wait for all jobs to complete.
  void                    wait();

private:

  // No copying or assigning.
  Manager ( const Manager & );
  Manager &operator = ( const Manager & );

  void                    _destroy();

  void                    _jobFinished ( Job::RefPtr );

  void                    _logEvent ( const std::string &s, Job::RefPtr job = Job::RefPtr ( 0x0 ) );

  // Use namespaces here, or gcc 4.0 will give an error.
  friend class Usul::Jobs::Detail::Task;

  // Data members.
  static Manager *_instance;
  mutable Mutex _mutex;
  ThreadPool _pool;
  LogPtr _log;
#if BOOST_VERSION >= 103900
  JobFinishedListeners _jobFinishedListeners;
#endif
};


///////////////////////////////////////////////////////////////////////////////
//
//  Add a job finished listener.
//
///////////////////////////////////////////////////////////////////////////////

template<class Slot>
inline void Manager::addJobFinishedListener ( const Slot& subscriber )
{
#if BOOST_VERSION >= 103900
  _jobFinishedListeners.connect ( subscriber );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a job finished listener.
//
///////////////////////////////////////////////////////////////////////////////

template<class Slot>
inline void Manager::removeJobFinishedListener ( const Slot& subscriber )
{
#if BOOST_VERSION >= 103900
  _jobFinishedListeners.disconnect ( subscriber );
#endif
}


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_JOB_MANAGER_CLASS_H_
