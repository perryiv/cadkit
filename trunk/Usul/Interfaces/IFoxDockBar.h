
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for adding fox dockable toolbar
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FOX_DOCKBAR_H__
#define _USUL_INTERFACE_FOX_DOCKBAR_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IFoxDockBar : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxDockBar );

  /// Id for this interface.
  enum { IID = 1851586789u };

  virtual void addDockBar( Usul::Interfaces::IUnknown *caller ) = 0;
  
}; // class IFoxDockBar


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_DOCKBAR_H__
