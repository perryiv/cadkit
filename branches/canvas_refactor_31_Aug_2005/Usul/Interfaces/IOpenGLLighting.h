
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting working with lighting.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_OPEN_GL_LIGHTING_H_
#define _USUL_INTERFACE_OPEN_GL_LIGHTING_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IOpenGLLighting : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOpenGLLighting );

  /// Id for this interface.
  enum { IID = 2489692457u };

  virtual bool                lighting() const = 0;
  virtual void                lighting( bool ) = 0;
  virtual bool                lightBothSides() const = 0;
  virtual void                lightBothSides( bool ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_OPEN_GL_LIGHTING_H_
