
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

#include "Usul/File/Log.h"
#include "Usul/Threads/Task.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <map>
#include <set>
#include <vector>

namespace boost { class thread; }

namespace Usul {
namespace Threads {


class USUL_EXPORT Pool
{
public:

  // Useful typedefs.
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef boost::thread Thread;
  typedef std::vector < Thread* > ThreadPool;
  typedef std::pair < int, unsigned long > TaskHandle;
  typedef std::map < TaskHandle, Task::RefPtr > TaskMap;
  typedef std::set < Task::RefPtr > TaskSet;
  typedef std::vector < std::string > Strings;
  typedef Usul::File::Log::RefPtr LogPtr;

  // Constructor
  Pool ( const std::string &name, unsigned int numThreads );
  ~Pool();

  // Add a task.
  TaskHandle              addTask ( int priority, Task *task );

  // Cancel all running threads and remove all queued tasks.
  void                    cancel();

  // Clear the queued tasks. Has no effect on tasks currently being executed.
  void                    clearQueuedTasks();

  // Get the names of the executing tasks.
  void                    executingNames ( Strings & ) const;

  // Does the pool have the task?
  bool                    hasQueuedTask ( TaskHandle ) const;

  // See if a higher-priority job is waiting.
  bool                    isHigherPriorityTaskWaiting ( int priority ) const;

  // Set/get the log.
  void                    logSet ( LogPtr );
  LogPtr                  logGet();

  // Get the mutex.
  Mutex &                 mutex() const;

  // Get the name.
  std::string             name() const;

  // Get the next task id. This will also increment the internal counter.
  unsigned long           nextTaskId();

  // Get the number of threads in the pool.
  unsigned int            numThreads() const;

  // Get the number of tasks.
  unsigned int            numTasks() const;

  // Get the number of tasks that are executing.
  unsigned int            numTasksExecuting() const;

  // Get the number of tasks that are waiting to be executed.
  unsigned int            numTasksQueued() const;

  // Remove the task from the queue. Has no effect on running tasks.
  void                    removeQueuedTask ( TaskHandle );

  // Set/get the sleep duration. This is the amount of time (in milliseconds) 
  // that the internal worker threads will sleep during a loop.
  void                    sleepDuration ( unsigned long );
  unsigned long           sleepDuration() const;

  // Wait for all tasks to complete.
  void                    waitForTasks();

private:

  // No copying or assigning.
  Pool ( const Pool & );
  Pool &operator = ( const Pool & );

  void                    _destroy();

  void                    _logEvent ( const std::string &s, std::ostream *optional = 0x0 );

  Task::RefPtr            _nextTask();

  void                    _startThreads();

  void                    _threadProcessTask ( Usul::Threads::Task *task );
  void                    _threadStarted();

  void                    _waitForThreads();

  // Data members.
  mutable Mutex _mutex;
  ThreadPool _pool;
  TaskMap _queue;
  TaskSet _executing;
  unsigned long _nextTaskId;
  unsigned long _sleep;
  unsigned int _numThreads;
  bool _runThreads;
  bool _started;
  LogPtr _log;
  const std::string _name;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_POOL_CLASS_H_
