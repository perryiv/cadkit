
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IMENU_ADD_H__
#define __USUL_INTERFACES_IMENU_ADD_H__

#include "Usul/Interfaces/IUnknown.h"

namespace MenuKit { class Menu; }

namespace Usul {
namespace Interfaces {


struct IMenuAdd : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMenuAdd );

  /// Id for this interface.
  enum { IID = 2732755211u };

  virtual void               menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;

}; // struct IMenuAdd


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_IMENU_ADD_H__
