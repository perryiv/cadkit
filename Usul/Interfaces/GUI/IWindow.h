
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for a window
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_WINDOW_H_
#define _USUL_INTERFACE_WINDOW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IWindow : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWindow );

  /// Id for this interface.
  enum { IID = 4041454608u };

  virtual void setFocus() = 0;
  virtual void handleMessage ( unsigned short message ) = 0;

};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_WINDOW_H_

