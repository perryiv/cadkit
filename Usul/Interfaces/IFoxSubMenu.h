
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_FOX_SUB_MENU_H__
#define __USUL_INTERFACES_FOX_SUB_MENU__H__

#include <string>

namespace FX { class FXWindow; class FXMenuPane; }

namespace Usul 
{
  namespace Interfaces
  {
    class IFoxSubMenu : public Usul::Interfaces::IUnknown
    {
    public:
      // Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IFoxSubMenu );

      enum { IID = 1102691747u };

      // Get the menu text.
      virtual std::string   menuText() const = 0;

      //build the menu
      virtual void buildSubMenu( Usul::Interfaces::IUnknown *caller, FX::FXWindow *owner, FX::FXMenuPane *menuPane  ) = 0;

    }; // class IFoxSubMenu
  }; //namespace Usul
}; // namespace Interfaces

#endif // __USUL_INTERFACES_FOX_SUB_MENU_H__

