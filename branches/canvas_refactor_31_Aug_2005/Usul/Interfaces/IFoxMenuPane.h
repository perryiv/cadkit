
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Interface for building a fox menu
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_FOX_MENU_PANE_H__
#define __USUL_INTERFACES_FOX_MENU_PANE_H__

namespace FoxTools { namespace Menu { class Bar; } }


namespace Usul {
namespace Interfaces {
    

struct IFoxMenuPane : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxMenuPane );

  enum { IID = 1101919453u };

  // Build the menu
  virtual void buildMenu ( Usul::Interfaces::IUnknown *caller, FoxTools::Menu::Bar *menuBar ) = 0;
};


} // namespace Usul
} // namespace Interfaces


#endif // __USUL_INTERFACES_FOX_MENU_PANE_H__
