
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_FOX_MENU_PANE_H__
#define __USUL_INTERFACES_FOX_MENU_PANE_H__

#include <string>

namespace FX { class FXMenuPane; }

namespace Usul 
{
  namespace Interfaces
  {
    class IFoxMenuPane : public Usul::Interfaces::IUnknown
    {
    public:
      // Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IFoxMenuPane );

      enum { IID = 1101919453u };

      // Get the menu text.
      virtual std::string   menuText() const = 0;

      //build the menu
      virtual void buildMenu( Usul::Interfaces::IUnknown *caller, FX::FXMenuPane*  ) = 0;

    }; // class IFoxMenuPane
  }; //namespace Usul
}; // namespace Interfaces

#endif // __USUL_INTERFACES_FOX_MENU_PANE_H__
