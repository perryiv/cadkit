
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_TIMER_SERVICE_H_
#define _USUL_INTERFACES_TIMER_SERVICE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct ITimerService : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef unsigned long TimerID;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimerService );

  // Id for this interface.
  enum { IID = 2221836210u };

  // Add a timer.
  virtual TimerID                     timerAdd ( unsigned int milliseconds, Usul::Interfaces::IUnknown::RefPtr, bool singleShot = false ) = 0;

  // Remove a timer.
  virtual void                        timerRemove ( TimerID ) = 0;
};


} // namespace Usul
} // namespace Interfaces


#endif // _USUL_INTERFACES_TIMER_SERVICE_H_

