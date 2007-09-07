
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Calls glBegin in constructor and glEnd in destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPE_GL_BEGIN_END_H_
#define _USUL_SCOPE_GL_BEGIN_END_H_

#include "Usul/Headers/OpenGL.h"


namespace Usul {
namespace Scope {


struct GLBeginEnd
{
  GLBeginEnd ( GLenum mode )
  {
    ::glBegin ( mode );
  }
  ~GLBeginEnd()
  {
    ::glEnd();
  }
};


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPE_GL_BEGIN_END_H_
