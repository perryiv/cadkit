
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _USUL_INTERFACE_FOX_MDI_MENU_H_
#define _USUL_INTERFACE_FOX_MDI_MENU_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXMDIMenu; }

namespace Usul {
namespace Interfaces {

struct IFoxMDIMenu : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxMDIMenu );

  /// Id for this interface.
  enum { IID = 3196383247u };

  virtual FX::FXMDIMenu *       mdiMenu  (  ) = 0;
  virtual const FX::FXMDIMenu * mdiMenu  (  ) const = 0;


}; // class IFoxMDIMenu


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_MDI_MENU_H_
