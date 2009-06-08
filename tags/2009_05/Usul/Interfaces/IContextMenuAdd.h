
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICONTEXT_MENU_ADD_H__
#define __USUL_INTERFACES_ICONTEXT_MENU_ADD_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector2.h"

namespace MenuKit { class Menu; }

namespace Usul {
namespace Interfaces {


struct IContextMenuAdd : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IContextMenuAdd );
  
  /// Id for this interface.
  enum { IID = 1511735010u };
  
  virtual void               contextMenuAdd ( MenuKit::Menu& menu, const Usul::Math::Vec2ui& position, Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;
  
}; // struct IContextMenuAdd


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_ICONTEXT_MENU_ADD_H__
