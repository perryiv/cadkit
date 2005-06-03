
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the openGL state of a view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_OPEN_GL_STATE_H_
#define _USUL_INTERFACE_OPEN_GL_STATE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IOpenGLState : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOpenGLState );

  /// Id for this interface.
  enum { IID = 2489692457u };


}; // class IOpenGLState


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_OPEN_GL_STATE_H_
