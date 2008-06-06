
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the canceled state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CANCELED_STATE_GET_H_
#define _USUL_INTERFACE_CANCELED_STATE_GET_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ICanceledStateGet : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICanceledStateGet );

  /// Id for this interface.
  enum { IID = 1776962488 };

  // Get the canceled state.
  virtual bool canceled() const = 0;

};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CANCELED_STATE_GET_H_
