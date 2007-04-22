
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

#include "Usul/Threads/Thread.h"
#include "Usul/Threads/Task.h"

#include <vector>
#include <list>
#include <map>


namespace Usul {
namespace Threads {


class USUL_EXPORT Pool
{
public:

  // Useful typedefs.
  typedef std::vector<Thread::RefPtr> ThreadPool;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Threads::Callback Callback;
  typedef std::list<Task::RefPtr> QueuedTasks;
  typedef unsigned int TaskHandle;
  typedef std::map<TaskHandle,Task::RefPtr> AllTasks;

  // Add a task.
  TaskHandle              add ( Callback *started, 
                                Callback *finished = 0x0,
                                Callback *cancelled = 0x0,
                                Callback *error = 0x0 );

  // It's a singleton.
  Pool &                  instance();

  // Does the pool have the task?
  bool                    hasTask ( TaskHandle ) const;

  // Return the mutex. Use with caution.
  Mutex &                 mutex() const;

  // Set the number of threads the pool has. If the number of non-idle 
  // threads is greater than the new size then this function will throw
  // unless false is passed as the last argument.
  void                    numThreads ( unsigned int num, bool allowThrow );

  // Get the number of threads in the pool.
  unsigned int            numThreads() const;

  // Get the number of threads with the given state or result.
  unsigned int            numThreads ( const Usul::Threads::Thread::Result &result ) const;
  unsigned int            numThreads ( const Usul::Threads::Thread::State &state ) const;

  // Get the number of tasks that are waiting for idle threads.
  unsigned int            numTasksQueued() const;

  // Remove the task from the pool.
  void                    remove ( TaskHandle );

protected:

  // Constructor/destructor.
  Pool();
  ~Pool();

  // Callbacks for internal thread.
  void                    _started   ( Usul::Threads::Thread * );
  void                    _cancelled ( Usul::Threads::Thread * );
  void                    _error     ( Usul::Threads::Thread * );
  void                    _finished  ( Usul::Threads::Thread * );

private:

  // No copying or assigning.
  Pool ( const Pool & );
  Pool &operator = ( const Pool & );

  // Data members.
  mutable Mutex _mutex;
  ThreadPool _pool;
  QueuedTasks _queued;
  AllTasks _tasks;
  unsigned long _nextTaskId;
  Thread::RefPtr _thread;
  static Pool *_instance;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_POOL_CLASS_H_
