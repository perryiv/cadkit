
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_POLYGON_MODE_H_
#define _USUL_INTERFACE_POLYGON_MODE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IPolygonMode : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPolygonMode );

  /// Id for this interface.
  enum { IID = 2837403757u };

  /// Possible modes.
  enum Mode { NONE, POINTS, WIRE_FRAME, HIDDEN_LINES, FILLED };

  /// Set/get the polygon mode state.
  virtual void                    polygonModeSet ( Mode mode ) = 0;
  virtual Mode                    polygonModeGet() const = 0;

}; // class IPolygonMode


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_POLYGON_MODE_H_
