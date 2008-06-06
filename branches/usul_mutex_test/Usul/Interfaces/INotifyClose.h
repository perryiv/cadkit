
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for to notify closing.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_NOTIFY_CLOSE_H_
#define _USUL_INTERFACE_NOTIFY_CLOSE_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct INotifyClose : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( INotifyClose );

  // Id for this interface.
  enum { IID = 1800673408u };

  // Notify the component.
  virtual bool          notifyClose ( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CLOSE_NOTIFY_H_
