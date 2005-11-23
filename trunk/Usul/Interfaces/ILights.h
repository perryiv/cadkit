
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for toggling lights.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_LIGHTS_H_
#define _USUL_INTERFACE_LIGHTS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ILights : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILights );

  /// Id for this interface.
  enum { IID = 3798770287u };

  virtual void               setLights ( bool ) = 0;
  virtual bool               hasLights ( ) const = 0;

}; // class ILights


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_LIGHTS_H_
