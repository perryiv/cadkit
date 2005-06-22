
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for building a sub menu
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_FOX_SUB_MENU_H__
#define __USUL_INTERFACES_FOX_SUB_MENU__H__

#include <string>

namespace FX { class FXWindow; class FXMenuPane; }

namespace FoxTools { namespace Menu { class Group; } }

namespace Usul  {
namespace Interfaces   {
    
struct IFoxSubMenu : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxSubMenu );

  enum { IID = 1102691747u };

  //build the menu
  virtual void buildSubMenu( Usul::Interfaces::IUnknown *caller, FoxTools::Menu::Group *menuPane  ) = 0;

}; // class IFoxSubMenu

}; //namespace Usul
}; // namespace Interfaces

#endif // __USUL_INTERFACES_FOX_SUB_MENU_H__

