
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface a component that has a menu button.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MENU_ENTRY_H_
#define _USUL_INTERFACE_MENU_ENTRY_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IMenuEntry : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMenuEntry );

  // Id for this interface.
  enum { IID = 2280889375u };

  // Get the menu text.
  virtual std::string   menuText() const = 0;

  // Get the hot-key text.
  virtual std::string   hotKeyText() const = 0;

  // Get the status-bar text.
  virtual std::string   statusBarText() const = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_MENU_ENTRY_H_

