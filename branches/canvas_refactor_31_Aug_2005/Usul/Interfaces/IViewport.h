
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with viewports.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_VIEWPORT_H_
#define _USUL_INTERFACE_VIEWPORT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IViewport : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IViewport );

  /// Id for this interface.
  enum { IID = 3627635665u };

  /// Set/get the viewport.
  virtual void viewport ( int  x, int  y, unsigned int  width, unsigned int  height ) = 0;
  virtual void viewport ( int &x, int &y, unsigned int &width, unsigned int &height ) const = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_VIEWPORT_H_
