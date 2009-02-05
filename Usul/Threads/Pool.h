
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

#include "Usul/Base/Object.h"
#include "Usul/File/Log.h"
#include "Usul/Threads/Task.h"
#include "Usul/Threads/Thread.h"
#include "Usul/Pointers/Pointers.h"

#include <map>
#include <set>
#include <vector>


namespace Usul {
namespace Threads {


class USUL_EXPORT Pool : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef std::vector < Thread::RefPtr > ThreadPool;
  typedef Usul::Threads::Callback Callback;
  typedef std::pair < int, unsigned long > TaskHandle;
  typedef std::map < TaskHandle, Task::RefPtr > TaskMap;
  typedef std::set < Task::RefPtr > TaskSet;
  typedef std::vector < std::string > Strings;
  typedef Usul::File::Log::RefPtr LogPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Pool );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Pool );

  // Enumerations.
  enum { DEFAULT_NUM_THREADS = 10 };

  // Constructor
  Pool ( const std::string &name, unsigned int numThreads = Pool::DEFAULT_NUM_THREADS, bool lazyStart = true );

  // Add a task.
  TaskHandle              addTask ( int priority, Task *task );
  TaskHandle              addTask ( int priority,
                                    Callback *started, 
                                    Callback *finished = 0x0,
                                    Callback *cancelled = 0x0,
                                    Callback *error = 0x0 );

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

protected:

  // Use reference counting.
  virtual ~Pool();

private:

  // No copying or assigning.
  Pool ( const Pool & );
  Pool &operator = ( const Pool & );

  void                    _cancelThreads();

  void                    _destroy();

  void                    _logEvent ( const std::string &s, Thread::RefPtr thread = Thread::RefPtr ( 0x0 ), std::ostream *optional = 0x0 );

  Task::RefPtr            _nextTask();

  void                    _startThreads();

  void                    _threadProcessTasks ( Usul::Threads::Task *task, Usul::Threads::Thread * );
  void                    _threadStarted ( Usul::Threads::Thread * );

  void                    _waitForThreads();

  // Data members.
  ThreadPool _pool;
  TaskMap _queue;
  TaskSet _executing;
  unsigned long _nextTaskId;
  unsigned long _sleep;
  bool _runThreads;
  bool _started;
  LogPtr _log;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_POOL_CLASS_H_
