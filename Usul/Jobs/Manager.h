
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

#include "Usul/Jobs/Job.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Pool.h"


namespace Usul {
namespace Jobs {


class USUL_EXPORT Manager
{
public:

  // Useful typedefs.
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Threads::Pool ThreadPool;

  // Constructor and destructor. Use as a singleton or as individual objects.
  Manager ( unsigned int poolSize = Usul::Threads::Pool::DEFAULT_NUM_THREADS );
  ~Manager();

  // Add a job to the list.
  void                    addJob ( Job * );

  // Cancel the job(s).
  void                    cancel();
  void                    cancel ( Job * );

  // Clear any jobs that are queued, but not running.
  void                    clearQueuedJobs();

  // This will delete the singleton instance, if any.
  static void             destroy();

  // Initialize singleton with given thread-pool size.
  static void             init ( unsigned int poolSize = Usul::Threads::Pool::DEFAULT_NUM_THREADS );

  // Get the singleton.
  static Manager &        instance();

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

  // Remove the queued job. Has no effect on running jobs.
  void                    removeQueuedJob ( Job * );

  // Wait for all jobs to complete.
  void                    wait();

private:

  // No copying or assigning.
  Manager ( const Manager & );
  Manager &operator = ( const Manager & );

  void                    _destroy();

  // Data members.
  static Manager *_instance;
  mutable Mutex _mutex;
  ThreadPool::RefPtr _pool;
};


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_JOB_MANAGER_CLASS_H_
