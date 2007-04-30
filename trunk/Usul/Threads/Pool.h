
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread pool class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_POOL_CLASS_H_
#define _USUL_THREADS_POOL_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Task.h"
#include "Usul/Threads/Thread.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>
#include <list>
#include <map>


namespace Usul {
namespace Threads {


class USUL_EXPORT Pool : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector<Thread::RefPtr> ThreadPool;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Threads::Callback Callback;
  typedef std::list<Task::RefPtr> QueuedTasks;
  typedef unsigned int TaskHandle;
  typedef std::map<TaskHandle,Task::RefPtr> AllTasks;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Pool );

  // Constructor
  Pool ( unsigned int numThreads = 10 );

  // Add a task.
  TaskHandle              add ( Callback *started, 
                                Callback *finished = 0x0,
                                Callback *cancelled = 0x0,
                                Callback *error = 0x0,
                                Callback *destroyed = 0x0 );

  // Cancel all tasks.
  void                    cancel();

  // Does the pool have the task?
  bool                    hasTask ( TaskHandle ) const;

  // Return the mutex. Use with caution.
  Mutex &                 mutex() const;

  // Get the number of threads in the pool.
  unsigned int            numThreads() const;

  // Get the number of idle threads.
  unsigned int            numThreadsIdle() const;

  // Get the number of tasks that are waiting for idle threads.
  unsigned int            numTasksQueued() const;

  // Remove the task from the pool.
  void                    remove ( TaskHandle );

  // Set/get the sleep duration. This is the amount of time (in milliseconds) 
  // that the internal worker thread will sleep during a loop.
  void                    sleepDuration ( unsigned long );
  unsigned long           sleepDuration() const;

  // Wait for all tasks to complete.
  void                    wait();
  void                    wait ( unsigned long timeout );

protected:

  // Use reference counting.
  virtual ~Pool();

private:

  // No copying or assigning.
  Pool ( const Pool & );
  Pool &operator = ( const Pool & );

  void                    _destroy();

  Thread *                _firstAvailableThread();

  bool                    _isRunning() const;

  void                    _threadProcessTasks();

  Task *                  _nextTask();

  void                    _threadStarted   ( Usul::Threads::Thread * );
  void                    _threadCancelled ( Usul::Threads::Thread * );
  void                    _threadError     ( Usul::Threads::Thread * );
  void                    _threadFinished  ( Usul::Threads::Thread * );

  void                    _waitForThreads ( unsigned long timeout );

  // Data members.
  mutable Mutex _mutex;
  ThreadPool _pool;
  QueuedTasks _queued;
  AllTasks _tasks;
  unsigned long _nextTaskId;
  Thread::RefPtr _thread;
  unsigned long _sleep;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_POOL_CLASS_H_
