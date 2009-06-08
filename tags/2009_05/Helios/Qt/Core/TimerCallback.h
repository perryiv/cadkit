
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal timer callback.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_QT_CORE_TIMER_CALLBACK_H_
#define _HELIOS_QT_CORE_TIMER_CALLBACK_H_

#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"

#include "QtCore/QTimer"

#include "boost/shared_ptr.hpp"

namespace CadKit { namespace Helios { namespace Core { class TimerServer; } } }


namespace CadKit {
namespace Helios {
namespace Core {


class TimerCallback : public QObject
{
  Q_OBJECT;

public:

  // Typedefs.
  typedef QObject BaseClass;
  typedef boost::shared_ptr < TimerCallback > Ptr;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Interfaces::IUnknown::RefPtr UnknownPtr;
  typedef boost::shared_ptr < QTimer > QTimerPtr;
  typedef Usul::Interfaces::ITimerService::TimerID TimerID;

  // Destructor.
  virtual ~TimerCallback();
  
  // Can this timer callback be purged?
  bool                      canPurge() const;

  // Start the timer.
  void                      start();
  
  // Stop the timer.
  void                      stop();

protected:

  // Construct a timer callback.
  TimerCallback ( TimerID id, unsigned int milliseconds, UnknownPtr callback, bool singleShot );

  friend class TimerServer;

private slots:
  
  void                      _onTimeout();
  void                      _start();
  void                      _stop();
  
private:

  // No copying or assignment.
  TimerCallback ( const TimerCallback & );
  TimerCallback &operator = ( const TimerCallback & );

  void                      _destroy();

  mutable Mutex _mutex;
  TimerID _id;
  unsigned int _milliseconds;
  QTimerPtr _timer;
  UnknownPtr _callback;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit


#endif // _HELIOS_QT_CORE_TIMER_CALLBACK_H_
