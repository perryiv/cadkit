
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_SET_CURSOR_TYPE_H__
#define __USUL_INTERFACES_SET_CURSOR_TYPE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct ISetCursorType : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISetCursorType );

  /// Id for this interface.
  enum { IID = 1453657148u };

  virtual void updateCursor () = 0;

  virtual void setCursorNav() = 0;
  virtual void setCursorTrans() = 0;
  virtual void setCursorRot() = 0;
  virtual void setCursorScale() = 0;
}; 

}
}

#endif //__USUL_INTERFACES_SET_CURSOR_TYPE_H__
