
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_JOBS_JOB_CLASS_H_
#define _USUL_JOBS_JOB_CLASS_H_

#include "Usul/Base/Observed.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/ICancel.h"
#include "Usul/Interfaces/ICanceledStateGet.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Thread.h"

#include <iosfwd>

namespace Usul { namespace Jobs { class Manager; } }
namespace Usul { namespace Jobs { namespace Detail { class Task; } } }
namespace Usul { namespace Jobs { namespace Helper { class ScopedThread; class ScopedDone; } } }


namespace Usul {
namespace Jobs {


class USUL_EXPORT Job : public Usul::Base::Observed,
                        public Usul::Interfaces::ICancel,
                        public Usul::Interfaces::ICanceledStateGet
{
public:

  // Useful typedefs.
  typedef Usul::Base::Observed BaseClass;
  typedef Usul::Threads::Callback Callback;
  typedef Usul::Threads::Thread Thread;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IProgressBar ProgressBar;
  typedef Usul::Interfaces::IStatusBar StatusBar;
  typedef Usul::Pointers::WeakPointer < Job > WeakPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Job );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Job );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Cancel the job.
  virtual void              cancel();

  // Was the job canceled?
  virtual bool              canceled() const;

  // Return this job's id.
  unsigned long             id() const;

  // Is the job done?
  bool                      isDone() const;

  // Set the label.
  void                      label ( IUnknown* label );

  // Get/Set the priority.
  void                      priority( int );
  int                       priority() const;

  // Get/Set the progress bar.
  void                      progress ( IUnknown* progress );
  IUnknown *                progress();

  // Overload to return an accurate indication of success.
  virtual bool              success() const;

  // Return this job's thread, which may be null.
  const Thread::RefPtr      thread() const;

  // Wait for this job to finish. Calling from the job's thread will throw.
  void                      wait ( std::ostream *out = 0x0, unsigned int numLoops = 0xFFFFFFFF, unsigned int sleep = 500 );

protected:

  // Constructors
  Job ( Usul::Interfaces::IUnknown* caller = 0x0, bool showProgressBar = true  );

  // Use reference counting.
  virtual ~Job();

  // Called when the job is cancelled.
  virtual void              _cancelled();

  // Called when the job encounters an error.
  virtual void              _error();

  // Called when the job finishes normally.
  virtual void              _finished();

  // Set the id. Protected so only the Job Manager can set it.
  void                      _setId ( unsigned int value );

  // Called when the job starts.
  virtual void              _started();

  /// Update the progress bar.
  void                      _updateProgress ( unsigned int numerator, unsigned int denominator, bool state = true );

  /// Set the label text.
  void                      _setLabel ( const std::string& text );

private:

  typedef Usul::Jobs::Helper::ScopedThread ScopedThread;
  typedef Usul::Jobs::Helper::ScopedDone ScopedDone;

  // No copying or assignment.
  Job ( const Job & );
  Job &operator = ( const Job & );

  void                      _destroy();

  void                      _setDone ( bool );
  void                      _setThread ( Thread *thread );

  void                      _threadCancelled ( Thread * );
  void                      _threadError     ( Thread * );
  void                      _threadFinished  ( Thread * );
  void                      _threadStarted   ( Thread * );

	// Use namespaces here, or gcc 4.0 will give an error.
  friend class Usul::Jobs::Detail::Task;
  friend class Usul::Jobs::Manager;
  friend class Helper::ScopedThread;
  friend class Helper::ScopedDone;

  unsigned long _id;
  Callback::RefPtr _cancelledCB;
  Callback::RefPtr _errorCB;
  Callback::RefPtr _finishedCB;
  Callback::RefPtr _startedCB;
  Thread::RefPtr _thread;
  bool _done;
  bool _canceled;
  ProgressBar::QueryPtr _progress;
  StatusBar::QueryPtr   _label;
  int _priority;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Template job class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class F > class GenericJob : public Job
  {
  public:

    typedef Job BaseClass;

    GenericJob ( F f, Usul::Interfaces::IUnknown *caller = 0x0, bool showProgress = true ) : 
      BaseClass ( caller, showProgress ),
      _f ( f )
    {
    }
  protected:

    virtual ~GenericJob()
    {
    }

  private:

    virtual void _started()
    {
      _f();
    }

    F _f;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make template job class.
//
///////////////////////////////////////////////////////////////////////////////

template < class F > Job *create ( F f, Usul::Interfaces::IUnknown *caller = 0x0, bool showProgress = true )
{
  return new Detail::GenericJob<F> ( f, caller, showProgress );
}


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_JOB_CLASS_H_
