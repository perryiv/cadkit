
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interfaces for getting FOX window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FOX_WINDOW_H_
#define _USUL_INTERFACE_FOX_WINDOW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXWindow; }

namespace Usul {
namespace Interfaces {

struct IFoxWindow : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxWindow );

  /// Id for this interface.
  enum { IID = 2463843388u };

  virtual FX::FXWindow* getFoxWindow() = 0;

}; // class IFoxWindow


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_WINDOW_H_
