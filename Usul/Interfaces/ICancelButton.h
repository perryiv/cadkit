
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for showing and hiding a cancel button
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CANCEL_BUTTON_H_
#define _USUL_INTERFACE_CANCEL_BUTTON_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct ICancelButton : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICancelButton );

  /// Id for this interface.
  enum { IID = 1103221577u };

  virtual void showCancelButton() = 0;
  virtual void hideCancelButton() = 0;

}; //  ICancelButton


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_CANCEL_BUTTON_H_

