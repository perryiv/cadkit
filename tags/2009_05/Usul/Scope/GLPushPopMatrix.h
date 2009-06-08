
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Calls glPushMatrix in constructor and glEnd in glPopMatrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPE_GL_PUSH_POP_MATRIX_H_
#define _USUL_SCOPE_GL_PUSH_POP_MATRIX_H_

#include "GL/GL.h"


namespace Usul {
namespace Scope {


struct GLPushPopMatrix
{
  GLPushPopMatrix ( GLenum mode ) : _mode ( mode )
  {
    ::glMatrixMode ( _mode );
    ::glPushMatrix();
  }
  ~GLPushPopMatrix()
  {
    ::glMatrixMode ( _mode );
    ::glPopMatrix();
  }
private:
  GLenum _mode;
};



} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPE_GL_PUSH_POP_MATRIX_H_
