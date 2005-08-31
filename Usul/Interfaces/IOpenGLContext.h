
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for working with the current OpenGL rendering context.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_OPEN_GL_RENDERING_CONTEXT_H_
#define _USUL_INTERFACE_OPEN_GL_RENDERING_CONTEXT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IOpenGLContext : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOpenGLContext );

  /// Id for this interface.
  enum { IID = 4012637471u };

  /// Make this context current.
  virtual void makeContextCurrent() = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_OPEN_GL_RENDERING_CONTEXT_H_
