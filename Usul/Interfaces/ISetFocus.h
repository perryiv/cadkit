
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for setting the window focus.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SET_FOCUS_H_
#define _USUL_INTERFACE_SET_FOCUS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ISetFocus : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISetFocus );

  /// Id for this interface.
  enum { IID = 4041454608u };

  /// Set the input focus to this instance.
  virtual void setFocus() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SET_FOCUS_H_
