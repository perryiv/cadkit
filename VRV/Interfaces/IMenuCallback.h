
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting a menu callback.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_SET_MENU_CALLBACK_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_SET_MENU_CALLBACK_H_

#include "Usul/Interfaces/IUnknown.h"

namespace MenuKit { class Menu; };


namespace VRV {
namespace Interfaces {


struct IMenuCallback : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef MenuKit::Menu Menu;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMenuCallback );

  // Id for this interface.
  enum { IID = 1286669827u };

  // Set the menu callback specified by the string.
  virtual void            setCallback ( const std::string &id, Menu *Menu )= 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_SET_MENU_CALLBACK_H_
