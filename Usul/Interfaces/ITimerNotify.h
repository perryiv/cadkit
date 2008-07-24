
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_TIMER_NOTIFY_H_
#define _USUL_INTERFACES_TIMER_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct ITimerNotify : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef unsigned long TimerID;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimerNotify );

  // Id for this interface.
  enum { IID = 2615516348u };

  // Called when the timer fires.
  virtual void                        timerNotify ( TimerID ) = 0;
};


} // namespace Usul
} // namespace Interfaces


#endif // _USUL_INTERFACES_TIMER_NOTIFY_H_

