
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for redrawing
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_REDRAW_H_
#define _USUL_INTERFACE_REDRAW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IRedraw : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRedraw );

  /// Id for this interface.
  enum { IID = 1101930131u };

  virtual void redraw() = 0;
  virtual void setStatsDisplay ( bool ) = 0;

}; // class IRedraw

}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_REDRAW_H_
