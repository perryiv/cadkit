
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal timer callback.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_TIMER_CALLBACK_H__
#define __VRV_CORE_TIMER_CALLBACK_H__

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Types/Types.h"

namespace VRV {
namespace Core {


class TimerCallback : public Usul::Base::Object
{
public:
  
  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown::RefPtr UnknownPtr;
  typedef Usul::Interfaces::ITimerService::TimerID TimerID;

  USUL_DECLARE_REF_POINTERS ( TimerCallback );

  // Constructor.
  TimerCallback ( TimerID id, unsigned int timeoutInMilliseconds, UnknownPtr callback );

  // Can this timer callback be purged?
  bool                      canPurge() const;

  // Start the timer.
  void                      start();
  
  // Stop the timer.
  void                      stop();

  // Update.  Will fire callback if time elapsed is greater than the timeout.
  void                      update();

protected:

  virtual ~TimerCallback();

private:

  // Call the callback.
  void                      _onTimeout();

  // No default construction.
  TimerCallback();

  // No copying or assignment.
  TimerCallback ( const TimerCallback & );
  TimerCallback &operator = ( const TimerCallback & );

  TimerID _id;
  Usul::Types::Uint64 _lastTimeCalled;
  unsigned int _timeoutInMilliseconds;
  UnknownPtr _callback;
  bool _active;
};


}
}

#endif // __VRV_CORE_TIMER_CALLBACK_H__
