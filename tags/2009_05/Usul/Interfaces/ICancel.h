
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for canceling state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CANCEL_H_
#define _USUL_INTERFACE_CANCEL_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ICancel : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICancel );

  /// Id for this interface.
  enum { IID = 1072013552 };

  // Set the canceled state. This may immediatly throw depending on the thread.
  virtual void cancel() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CANCEL_H_
