
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for notifying.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_NOTIFY_H_
#define _USUL_INTERFACE_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


template
<
  class MessageType, 
  unsigned long iid
>
struct INotify : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( INotify );

  // Id for this interface.
  enum { IID = iid };

  // Notify the component.
  virtual void          notify ( Usul::Interfaces::IUnknown *caller, const MessageType &message ) = 0;
};


typedef INotify < std::string,   1099894744u > INotifyString;
typedef INotify < unsigned long, 1099894745u > INotifyULong;


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_NOTIFY_H_
