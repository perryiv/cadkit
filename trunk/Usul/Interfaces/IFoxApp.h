
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Interface for returning fox application
//
/////////////////////////////////////////////////////////////////////////////
  
#ifndef _USUL_INTERFACE_FOX_APP_H_
#define _USUL_INTERFACE_FOX_APP_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXApp; }

namespace Usul {
namespace Interfaces {

struct IFoxApp : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxApp );

  /// Id for this interface.
  enum { IID = 1102089620u };

  virtual FX::FXApp* getFoxApp() = 0;

}; // class IFoxApp


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_APP_H_
