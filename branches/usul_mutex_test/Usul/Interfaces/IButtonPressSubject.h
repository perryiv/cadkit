
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

#ifndef _USUL_INTERFACES_BUTTON_PRESS_SUBJECT_H_
#define _USUL_INTERFACES_BUTTON_PRESS_SUBJECT_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IButtonPressSubject : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonPressSubject );

  // Id for this interface.
  enum { IID = 4195388897u };

  // Add the listener.
  virtual void            addButtonPressListener ( Usul::Interfaces::IUnknown * ) = 0;

  // Remove all listeners.
  virtual void            clearButtonPressListeners() = 0;

  // Remove the listener.
  virtual void            removeButtonPressListener ( Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_BUTTON_PRESS_SUBJECT_H_
