
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
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimerService );

  // Id for this interface.
  enum { IID = 2221836210u };
}; 


} // namespace Usul
} // namespace Interfaces


#endif // _USUL_INTERFACES_TIMER_SERVICE_H_

