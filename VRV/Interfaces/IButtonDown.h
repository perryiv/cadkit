
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a button down.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_BUTTON_DOWN_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_BUTTON_DOWN_H_

#include "Usul/Interfaces/IUnknown.h"


namespace VRV {
namespace Interfaces {


struct IButtonDown : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonDown );

  // Id for this interface.
  enum { IID = 3898604149u };

  // Get the id representing the button combination.
  virtual unsigned long   getButtonId() const = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_BUTTON_DOWN_H_
