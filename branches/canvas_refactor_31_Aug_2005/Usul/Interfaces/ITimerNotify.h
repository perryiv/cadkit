
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for receiving timer events.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_TIMER_NOTIFY_H_
#define _USUL_INTERFACE_TIMER_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ITimerNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimerNotify );

  /// Id for this interface.
  enum { IID = 2203616723u };

  /// Notification of a timer event. Return false to cancel the timer.
  virtual bool                    timerEventNotify ( unsigned int id, IUnknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_TIMER_NOTIFY_H_
