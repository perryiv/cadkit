
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting a button callback.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_SET_BUTTON_CALLBACK_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_SET_BUTTON_CALLBACK_H_

#include "Usul/Interfaces/IUnknown.h"

namespace MenuKit { class Button; };


namespace VRV {
namespace Interfaces {


struct IButtonCallback : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef MenuKit::Button Button;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonCallback );

  // Id for this interface.
  enum { IID = 2495219354u };

  // Set the button callback specified by the string.
  virtual void            setCallback ( const std::string &id, Button *button )= 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_SET_BUTTON_CALLBACK_H_
