
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TIMEOUT_SPIN_H__
#define __USUL_INTERFACES_TIMEOUT_SPIN_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct ITimeoutSpin : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimeoutSpin );

  /// Id for this interface.
  enum { IID = 2373359158u };

  virtual void stopSpin ( ) = 0;
  virtual void startSpin ( double timeout ) = 0;

}; 

}

}


#endif // __USUL_INTERFACES_TIMEOUT_SPIN_H__

