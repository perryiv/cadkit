
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for notify to cancel
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CANCEL_NOTIFY_H_
#define _USUL_INTERFACE_CANCEL_NOTIFY_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct ICancelNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICancelNotify );

  /// Id for this interface.
  enum { IID = 1103221670u };

  virtual void cancelNotify() = 0;

}; //  ICancelNotify


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_CANCEL_NOTIFY_H_

