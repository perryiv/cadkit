
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting the cursor
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SET_CURSOR_H__
#define _USUL_INTERFACE_SET_CURSOR_H__

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXCursor; }

namespace Usul {
namespace Interfaces {


struct ISetCursor : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISetCursor );

  /// Id for this interface.
  enum { IID = 1104265934u };

  virtual FX::FXCursor* getCursor() = 0;

  
}; // class ISetCursor


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_SET_CURSOR_H__
