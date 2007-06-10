
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

#include <list>

namespace Usul {
namespace Jobs {


class USUL_EXPORT Manager
{
public:

  // Useful typedefs.
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Threads::Pool ThreadPool;
  typedef std::list<Job::RefPtr> JobList;

  // Add a job to the list.
  void                    add ( Job * );

  // This will delete the previous instance, if any.
  static void             destroy();

  // See if the list of jobs is empty.
  bool                    empty() const;

  // It's a singleton.
  static Manager &        instance();

  // Get the next job id. This will also increment the internal counter.
  unsigned long           nextJobId();

  // Return the mutex. Use with caution.
  Mutex &                 mutex() const;

  // Get the size and resize the thread pool.
  void                    poolResize ( unsigned int numThreads );
  unsigned int            poolSize() const;

  // Wait for all jobs to complete.
  void                    wait();
  void                    wait ( unsigned long timeout );

private:

  // Constructor and destructor.
  Manager();
  ~Manager();

  // No copying or assigning.
  Manager ( const Manager & );
  Manager &operator = ( const Manager & );

  void                    _destroy();

  // Data members.
  static Manager *_instance;
  mutable Mutex _mutex;
  unsigned long _nextJobId;
  JobList _jobs;
  ThreadPool::RefPtr _pool;
};


}; // namespace Jobs
}; // namespace Usul


#endif // _USUL_JOBS_JOB_MANAGER_CLASS_H_
