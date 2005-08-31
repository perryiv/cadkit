
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for rendering.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_RENDER_H_
#define _USUL_INTERFACE_RENDER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IRender : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRender );

  /// Id for this interface.
  enum { IID = 3121729195u };

  /// Render the scene.
  virtual void render() = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_RENDER_H_
