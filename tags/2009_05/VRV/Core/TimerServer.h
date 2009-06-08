
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Timer server.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HELIOS_VR_CORE_TIMER_SERVER_H_
#define _HELIOS_VR_CORE_TIMER_SERVER_H_

#include "VRV/Core/TimerCallback.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/ITimerService.h"

#include "boost/tuple/tuple.hpp"

#include <list>
#include <map>


namespace VRV {
namespace Core {


class TimerServer : public Usul::Base::Object,
                    public Usul::Interfaces::ITimerService
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef Usul::Interfaces::ITimerService ITimerService;
  typedef ITimerService::TimerID TimerID;
  typedef std::map < TimerID, TimerCallback::RefPtr > Timers;
  typedef Usul::Interfaces::IUnknown::RefPtr UnknownPtr;
  typedef std::list < TimerCallback::RefPtr > RemovedTimers;

  // Constructor.
  TimerServer();

  // Type information.
  USUL_DECLARE_TYPE_ID ( TimerServer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TimerServer );

  // Usul::Interfaces::IUnknown members.
	USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Add the timers that were added from non-gui threads.
  // Note: call this from the gui thread.
  void                      addPendingTimers();

  // Clear the timers.
  void                      clear();
  
  // Purge all null timers, or timers with one reference count.
  void                      purge();

  // Start all the timers.
  void                      start();

  // Stop all the timers.
  void                      stop();

  // Usul::Interfaces::ITimerService.
  virtual TimerID           timerAdd ( unsigned int milliseconds, UnknownPtr, bool singleShot = false );
  virtual void              timerRemove ( TimerID );

  // Update all timers.
  void                      update();
  
protected:

  // Use reference counting.
	virtual ~TimerServer();

  void                      _timerAdd ( TimerID id, unsigned int milliseconds, UnknownPtr callback, bool singleShot );

private:

  // No copying or assignment.
  TimerServer ( const TimerServer & );
  TimerServer &operator = ( const TimerServer & );

  void                      _destroy();

  Timers _timers;
  TimerID _nextId;
  RemovedTimers _removed;
};


} // namespace Core
} // namespace VRV


#endif // _HELIOS_VR_CORE_TIMER_SERVER_H_
