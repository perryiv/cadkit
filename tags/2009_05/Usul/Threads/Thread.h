
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

#include "Usul/Base/Object.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Pointers/Pointers.h"


namespace Usul {
namespace Threads {


class USUL_EXPORT Thread : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Thread *FactoryFunction();
  typedef BaseClass::Mutex Mutex;
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

  // If this function is called from the system thread that this instance 
  // encapsulates then it will throw the "cancelled" exception. Otherwise,
  // it simply sets the state to cancelled. Setting this flags is a request 
  // that the implementation has to act upon accordingly.
  void                    cancel();

  // Get the thread's ID. This is not the system's ID for the thread, 
  // but a unique integer that increments for each new thread object.
  unsigned long           id() const;

  // See if the thread has the given resut or state.
  bool                    hasResult ( Result ) const;
  bool                    hasState ( State ) const;

  // See if this thread is idle. When it's idle it is ok to delete.
  virtual bool            isIdle() const;

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
  void                    destroyed ( Callback * );
  void                    error     ( Callback * );
  void                    finished  ( Callback * );
  void                    started   ( Callback * );

protected:

  // Use creation function.
  Thread ( const std::string &name );

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

  void                    _destroy();

  void                    _executeThread() throw();

  unsigned long           _getCreationThread() const;

  void                    _notifyCancelled();
  void                    _notifyDestroyed();
  void                    _notifyError();
  void                    _notifyFinished();
  void                    _notifyStarted();

  void                    _reportError ( const std::string &s ) throw();

  void                    _setError ( const std::string & );
  void                    _setResult ( Result );
  void                    _setState ( State );
  void                    _setSystemThreadId ( unsigned long id );
  void                    _setSystemThreadName();

  // Data members.
  State _state;
  Result _result;
  Callback::RefPtr _cancelledCB;
  Callback::RefPtr _destroyedCB;
  Callback::RefPtr _errorCB;
  Callback::RefPtr _finishedCB;
  Callback::RefPtr _startedCB;
  std::string _errorMessage;
  unsigned long _id;
  unsigned long _systemId;
  unsigned long _creationThread;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_THREAD_CLASS_H_
