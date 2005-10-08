
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FOX_CLIENT_AREA_H_
#define _USUL_INTERFACE_FOX_CLIENT_AREA_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXMDIClient; }

namespace Usul {
namespace Interfaces {

struct IFoxClientArea : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxClientArea );

  /// Id for this interface.
  enum { IID = 2156862857u };

  virtual FX::FXMDIClient *       clientArea  (  ) = 0;
  virtual const FX::FXMDIClient * clientArea  (  ) const = 0;


}; // class IFoxClientArea


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_CLIENT_AREA_H_
