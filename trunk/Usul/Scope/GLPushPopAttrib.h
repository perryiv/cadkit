
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Calls glPushAttrib in constructor and glEnd in glPopAttrib.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPE_GL_PUSH_POP_ATTRIB_H_
#define _USUL_SCOPE_GL_PUSH_POP_ATTRIB_H_

#include "GL/GL.h"


namespace Usul {
namespace Scope {


struct GLPushPopAttrib
{
  GLPushPopAttrib ( GLenum mode )
  {
    ::glPushAttrib ( mode );
  }
  ~GLPushPopAttrib()
  {
    ::glPopAttrib();
  }
};


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPE_GL_PUSH_POP_ATTRIB_H_
