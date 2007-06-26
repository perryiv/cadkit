
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

#include "Usul/Base/Object.h"
#include "Usul/Threads/Callback.h"
#include "Usul/Threads/Thread.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"

namespace Usul { namespace Jobs { class Manager; } }
namespace Usul { namespace Jobs { namespace Helper { class ScopedThread; class ScopedDone; } } }


namespace Usul {
namespace Jobs {


class USUL_EXPORT Job : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Threads::Callback Callback;
  typedef Usul::Threads::Thread Thread;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IProgressBar ProgressBar;
  typedef Usul::Interfaces::IStatusBar StatusBar;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Job );

  // Cancel the job.
  void                      cancel();

  // Return this job's id.
  unsigned long             id() const;

  // Is the job done?
  bool                      isDone() const;

  // Return this job's thread.
  const Thread *            thread() const;

  /// Get/Set the progress bar.
  void                      progress ( IUnknown* progress );
  IUnknown*                 progress ();

  /// Set the label.
  void                      label ( IUnknown* label );
protected:

  // Constructor
  Job( Usul::Interfaces::IUnknown* caller = 0x0 );

  // Use reference counting.
  virtual ~Job();

  // Called when the job is cancelled.
  virtual void              _cancelled();

  // Called when the job encounters an error.
  virtual void              _error();

  // Called when the job finishes normally.
  virtual void              _finished();

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
  ProgressBar::QueryPtr _progress;
  StatusBar::QueryPtr   _label;
};


} // namespace Jobs
} // namespace Usul


#endif // _USUL_JOBS_JOB_CLASS_H_
