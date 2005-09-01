
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for registering timer callbacks.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_TIMER_SERVER_H_
#define _USUL_INTERFACE_TIMER_SERVER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ITimerServer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimerServer );

  /// Id for this interface.
  enum { IID = 2106047749u };

  /// Query if timer exists.
  virtual bool                    timerCallbackHas ( unsigned int id ) const = 0;

  /// Remove a timer callback.
  virtual void                    timerCallbackRemove ( unsigned int id ) = 0;

  /// Set a timer callback.
  virtual void                    timerCallbackSet ( double seconds, unsigned int id, IUnknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_TIMER_SERVER_H_
