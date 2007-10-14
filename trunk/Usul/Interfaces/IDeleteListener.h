
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for listeners of deleted objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DELETE_LISTENER_H_
#define _USUL_INTERFACE_DELETE_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IDeleteListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDeleteListener );

  /// Id for this interface.
  enum { IID = 3770363671u };

  // Notify the listener that the object has been deleted.
  virtual void            objectDeleted ( Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DELETE_LISTENER_H_
