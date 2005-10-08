
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for fox toolbar
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FOX_TOOLBAR_H__
#define _USUL_INTERFACE_FOX_TOOLBAR_H__

#include "Usul/Interfaces/IUnknown.h"

namespace FoxTools { namespace ToolBar { class Bar; } }

namespace Usul {
namespace Interfaces {


struct IFoxToolbar : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxToolbar );

  /// Id for this interface.
  enum { IID = 1104349797u };

  virtual void addButtons( Usul::Interfaces::IUnknown *caller, FoxTools::ToolBar::Bar* ) = 0;
  
}; // class IFoxToolbar


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_TOOLBAR_H__
