
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting a menu.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_GET_MENU_FILE_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_GET_MENU_FILE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace MenuKit { class Menu; };


namespace VRV {
namespace Interfaces {


struct IMenuGet : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef MenuKit::Menu Menu;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMenuGet );

  // Id for this interface.
  enum { IID = 2393210527u };

  // Return a pointer to the menu.
  virtual const Menu *    getMenu() const = 0;
  virtual Menu *          getMenu() = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_GET_MENU_FILE_H_
