
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ITOOLBAR_ADD_H__
#define __USUL_INTERFACES_ITOOLBAR_ADD_H__

#include "Usul/Interfaces/IUnknown.h"

namespace MenuKit { class Menu; }

namespace Usul {
namespace Interfaces {


struct IToolBarAdd : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IToolBarAdd );
  
  /// Id for this interface.
  enum { IID = 1888414812u };
  
  /// Add to the toolbar.
  virtual void               toolBarAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;
  
}; // struct IToolBarAdd


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_ITOOLBAR_ADD_H__
