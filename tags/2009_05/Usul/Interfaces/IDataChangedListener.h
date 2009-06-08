
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for render listeners.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DATA_CHANGED_LISTENER_H_
#define _USUL_INTERFACE_DATA_CHANGED_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IDataChangedListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDataChangedListener );
  
  /// Id for this interface.
  enum { IID = 2562153300u };
  
  // Called when data has changed.
  virtual void                dataChangedNotify ( Usul::Interfaces::IUnknown *caller ) = 0;

}; // IDataChangedListener


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DATA_CHANGED_LISTENER_H_
