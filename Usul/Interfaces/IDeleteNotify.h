
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for objects that notify listeners when they are deleted.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DELETE_NOTIFY_H_
#define _USUL_INTERFACE_DELETE_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IDeleteNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDeleteNotify );

  /// Id for this interface.
  enum { IID = 3432253643u };

  // Add the listener (IDeleteListener).
  virtual void            addDeleteListener ( Usul::Interfaces::IUnknown * ) = 0;

  // Remove the listener (IDeleteListener).
  virtual void            removeDeleteListener ( Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DELETE_NOTIFY_H_
