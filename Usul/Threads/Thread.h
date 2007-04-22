
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_THREAD_CLASS_H_
#define _USUL_THREADS_THREAD_CLASS_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Callback.h"


namespace Usul {
namespace Threads {


///////////////////////////////////////////////////////////////////////////////
//
//  Base thread class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Thread : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Thread *FactoryFunction();
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Threads::Mutex StaticMutex;
  typedef Usul::Threads::Guard<StaticMutex> StaticGuard;
  typedef Usul::Threads::Callback Callback;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Thread );

  // Run states.
  enum State
  {
    NOT_RUNNING, // Thread is not running.
    SCHEDULED,   // Thread has been asked to run but has not started yet.
    RUNNING,     // Thread is running.
  };

  // Result states.
  enum Result
  {
    NORMAL,       // Thread finished normaly.
    CANCELLED,    // Thread was cancelled.
    ERROR_RESULT, // Thread encountered an error.
  };

  // Set the state to cancelled. This flag is a request that the 
  // implementation has to act upon accordingly.
  void                    cancel();

  // Create a thread. Uses the registered factory-function.
  static Thread *         create();

  // Set/get the factory-function. Return the previous one.
  static FactoryFunction *createFunction ( FactoryFunction *fun );
  static FactoryFunction *createFunction();

  // Set the GUI thread ID.
  static void             guiThread ( unsigned long );

  // Get the thread's ID. This is not the system's ID for the thread, 
  // but a unique integer that increments for each new thread object.
  unsigned long           id() const;

  // See if the thread has the given resut or state.
  bool                    hasResult ( Result ) const;
  bool                    hasState ( State ) const;

  // Is this the GUI thread?
  static bool             isGuiThread();

  // Kill the thread. Behavior is implementation specific.
  virtual void            kill() = 0;

  // Return the mutex. Use with caution.
  Mutex &                 mutex() const;

  // Get the thread's result-state.
  Result                  result() const;

  // Schedules the thread and immediately returns.
  void                    start();

  // Get the thread's run-state.
  State                   state() const;

  // Get the id for the system thread that this object encapsulates. 
  // This number can change for the same object because it can be 
  // used to run many threads (sequentially) before it is destroyed 
  // (e.g., in a thread-pool).
  unsigned long           systemId() const;

  // Set the callbacks.
  void                    cancelled ( Callback * );
  void                    error     ( Callback * );
  void                    finished  ( Callback * );
  void                    started   ( Callback * );

protected:

  // Use creation function.
  Thread();

  // Use reference counting.
  virtual ~Thread();

  // Call this from the system thread's "start" function.
  void                    _execute();

  // Overload this function to start the thread.
  virtual void            _start() = 0;

private:

  // No copying or assigning.
  Thread ( const Thread & );
  Thread &operator = ( const Thread & );

  unsigned long           _getCreationThread() const;
  static StaticMutex &    _getStaticMutex();

  void                    _notifyCancelled();
  void                    _notifyError();
  void                    _notifyFinished();
  void                    _notifyStarted();

  void                    _reportErrorNoThrow ( const std::string &s );

  void                    _set ( State, Result );
  void                    _setError ( const std::string & );
  void                    _setResult ( Result );
  void                    _setState ( State );
  void                    _setSystemThreadId ( unsigned long id );

  // Data members.
  State _state;
  Result _result;
  mutable Mutex _mutex;
  Callback::RefPtr _cancelledCB;
  Callback::RefPtr _errorCB;
  Callback::RefPtr _finishedCB;
  Callback::RefPtr _startedCB;
  std::string _errorMessage;
  unsigned long _id;
  unsigned long _systemId;
  unsigned long _creationThread;

  // Static data members.
  static StaticMutex *_staticMutex;
  static FactoryFunction *_fun;
  static unsigned long _guiThread;
  static unsigned long _nextThreadId;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct. Make a global instance of this if setting in main() is 
//  too late in the program execution.
//
///////////////////////////////////////////////////////////////////////////////

struct SetThreadFactory
{
  template < class FactoryFunction > SetThreadFactory ( FactoryFunction f, bool replace = false )
  {
    Usul::Threads::Thread::FactoryFunction *current ( Usul::Threads::Thread::createFunction() );
    if ( 0x0 == current || true == replace )
      Usul::Threads::Thread::createFunction ( f );
  }
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_THREAD_CLASS_H_
