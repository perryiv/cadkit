
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a button press.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_BUTTON_PRESS_LISTENER_H_
#define _USUL_INTERFACES_BUTTON_PRESS_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IButtonPressListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonPressListener );

  // Id for this interface.
  enum { IID = 1681623590u };

  // Called when button is pressed.
  virtual bool            buttonPressNotify ( Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_BUTTON_PRESS_LISTENER_H_
