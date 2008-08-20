
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IBUSY_STATE_H__
#define __USUL_INTERFACES_IBUSY_STATE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IBusyState : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBusyState );
  
  /// Id for this interface.
  enum { IID = 1503980048u };
  
  /// Get the busy state.
  virtual bool             busyStateGet() const = 0;
  
}; // struct IBusyState


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IBUSY_STATE_H__
