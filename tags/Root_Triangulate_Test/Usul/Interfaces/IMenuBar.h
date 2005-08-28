
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for getting the menu bar.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MENU_BAR_H_
#define _USUL_INTERFACE_MENU_BAR_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FoxTools { namespace Menu { class Bar; } }

namespace Usul {
namespace Interfaces {


struct IMenuBar : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMenuBar );

  /// Id for this interface.
  enum { IID = 2210002179u };

  virtual FoxTools::Menu::Bar* getMenuBar () = 0;

}; // struct IMenuBar


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MENU_BAR_H_

