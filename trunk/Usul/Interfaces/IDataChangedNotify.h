
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for data changed notification.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DATA_CHANGED_NOTIFY_H_
#define _USUL_INTERFACE_DATA_CHANGED_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {
    
    
struct IDataChangedNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDataChangedNotify );
  
  /// Id for this interface.
  enum { IID = 4032960362u };
  
  // Add the listener.
  virtual void                addDataChangedListener ( Usul::Interfaces::IUnknown *caller ) = 0;
  
  // Remove the listener.
  virtual void                removeDataChangedListener ( Usul::Interfaces::IUnknown *caller ) = 0;
}; // IDataChangedNotify


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DATA_CHANGED_NOTIFY_H_
