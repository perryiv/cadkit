
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for changing entries in the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_CHANGE_MENU_BAR_H__
#define __USUL_INTERFACES_CHANGE_MENU_BAR_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IChangeMenuBar : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IChangeMenuBar );

  /// Id for this interface.
  enum { IID = 1136586688u };

  /// Change to menu bar.
  virtual void              changeManuBar ( IUnknown *caller ) = 0;
};


} // end namespace Interfaces
} // end namespace Usul


#endif // __USUL_INTERFACES_CHANGE_MENU_BAR_H__
