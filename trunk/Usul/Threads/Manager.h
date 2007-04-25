
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread manager class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_THREAD_MANAGER_CLASS_H_
#define _USUL_THREADS_THREAD_MANAGER_CLASS_H_

#include "Usul/Threads/Thread.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <list>


namespace Usul {
namespace Threads {


class USUL_EXPORT Manager
{
public:

  // Useful typedefs.
  typedef Thread *FactoryFunction();
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef std::list<Thread::RefPtr> ThreadList;

  // Create a thread. Uses the registered factory-function.
  Thread *                create();

  // Set/get the factory-function. Return the previous one.
  FactoryFunction *       factory ( FactoryFunction *fun );
  FactoryFunction *       factory();

  // Set/get the GUI thread ID.
  unsigned long           guiThread() const;
  void                    guiThread ( unsigned long );

  // It's a singleton.
  static Manager &        instance();

  // Is this the GUI thread?
  bool                    isGuiThread() const;

  // Get the next thread id. This will also increment the internal counter.
  unsigned long           nextThreadId();

  // Return the mutex. Use with caution.
  Mutex &                 mutex() const;

private:

  // Constructor and destructor.
  Manager();
  ~Manager();

  // No copying or assigning.
  Manager ( const Manager & );
  Manager &operator = ( const Manager & );

  // Data members.
  static Manager *_instance;
  mutable Mutex _mutex;
  FactoryFunction *_factory;
  unsigned long _guiThread;
  unsigned long _nextThreadId;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_THREAD_MANAGER_CLASS_H_
