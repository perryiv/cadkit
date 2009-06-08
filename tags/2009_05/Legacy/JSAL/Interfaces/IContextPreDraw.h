
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Context-specific pre-draw-notify interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _JSAL_INTERFACE_NOTIFY_CONTEXT_PRE_DRAW_H_
#define _JSAL_INTERFACE_NOTIFY_CONTEXT_PRE_DRAW_H_

#include "Usul/Interfaces/IUnknown.h"


namespace JSAL {
namespace Interfaces {


struct IContextPreDraw : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer defPreDrawions.
  USUL_DECLARE_QUERY_POINTERS ( IContextPreDraw );

  /// Id for this interface.
  enum { IID = 2261579016u };

  // Called once for each display every frame.
  virtual void              contextPreDraw() = 0;
};


}; // namespace Interfaces
}; // namespace JSAL


#endif // _JSAL_INTERFACE_NOTIFY_CONTEXT_PRE_DRAW_H_
