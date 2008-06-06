
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper functions for the OpenGL API.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_OPENGL_API_H_
#define _GENERIC_SCENE_GRAPH_OPENGL_API_H_

#include "GSG/Core/Assert.h"

#include <GL/glu.h>


namespace GSG {
namespace GL {


///////////////////////////////////////////////////////////////////////////////
//
//  Misc.
//
///////////////////////////////////////////////////////////////////////////////

inline void enable ( GLenum cap )
{
  ::glEnable ( cap );
}
inline void disable ( GLenum cap )
{
  ::glDisable ( cap );
}
inline void enableClientState ( GLenum array )
{
  ::glEnableClientState ( array );
}
inline void disableClientState ( GLenum array )
{
  ::glDisableClientState ( array );
}
inline void clear ( GLbitfield mask )
{
  ::glClear ( mask );
}
inline void flush()
{
  ::glFlush();
}
inline void loadIdentity()
{
  ::glLoadIdentity();
}
inline void matrixMode ( GLenum mode )
{
  ::glMatrixMode ( mode );
}
inline void clearDepth ( GLclampd depth )
{
  ::glClearDepth ( depth );
}
inline GLenum getError()
{
  return ::glGetError();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perspective.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct SetPerspective;
  template <> struct SetPerspective < GLdouble >
  {
    void operator () ( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar ) const
    {
      ::gluPerspective ( fovy, aspect, zNear, zFar );
    }
  };
  template <> struct SetPerspective < GLfloat >
  {
    void operator () ( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar ) const
    {
      ::gluPerspective ( fovy, aspect, zNear, zFar );
    }
  };
};
template < typename T > inline void perspective ( T fovy, T aspect, T zNear, T zFar )
{
  Detail::SetPerspective<T> () ( fovy, aspect, zNear, zFar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Material.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct SetMaterial;
  template <> struct SetMaterial < GLdouble >
  {
    void operator () ( GLenum face, GLenum name, const GLdouble *c ) const
    {
      GLfloat color[4] = 
      { 
        static_cast < GLfloat > ( c[0] ), 
        static_cast < GLfloat > ( c[1] ), 
        static_cast < GLfloat > ( c[2] ), 
        static_cast < GLfloat > ( c[3] )
      };
      ::glMaterialfv ( face, name, color );
    }
  };
  template <> struct SetMaterial < GLfloat >
  {
    void operator () ( GLenum face, GLenum name, const GLfloat *color ) const
    {
      ::glMaterialfv ( face, name, color );
    }
  };
};
inline void material ( GLenum face, GLenum name, GLdouble shininess )
{
  GSG_ASSERT ( GL_SHININESS == name );
  ::glMaterialf ( face, name, static_cast < GLfloat > ( shininess ) );
}
inline void material ( GLenum face, GLenum name, GLfloat shininess )
{
  GSG_ASSERT ( GL_SHININESS == name );
  ::glMaterialf ( face, name, shininess );
}
template < typename Color_ > inline void material ( GLenum face, GLenum name, const Color_ &color )
{
  typedef typename Color_::value_type value_type;
  Detail::SetMaterial < value_type > () ( face, name, color.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Color.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct SetColor;
  template <> struct SetColor < GLdouble >
  {
    void operator () ( const GLdouble *color ) const
    {
      ::glColor4dv ( color );
    }
  };
  template <> struct SetColor < GLfloat >
  {
    void operator () ( const GLfloat *color ) const
    {
      ::glColor4fv ( color );
    }
  };
};
template < typename Color_ > inline void color ( const Color_ &color )
{
  typedef typename Color_::value_type value_type;
  Detail::SetColor < value_type > () ( color.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Normal.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct SetNormal;
  template <> struct SetNormal < GLdouble >
  {
    void operator () ( const GLdouble *n ) const
    {
      ::glNormal3dv ( n );
    }
  };
  template <> struct SetNormal < GLfloat >
  {
    void operator () ( const GLfloat *n ) const
    {
      ::glNormal3fv ( n );
    }
  };
};
template < typename Normal_ > inline void normal ( const Normal_ &n )
{
  typedef typename Normal_::value_type value_type;
  Detail::SetNormal < value_type > () ( n.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Matrices.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct LoadMatrix;
  template <> struct LoadMatrix < GLdouble >
  {
    void operator () ( const GLdouble *m ) const
    {
      ::glLoadMatrixd ( m );
    }
  };
  template <> struct LoadMatrix < GLfloat >
  {
    void operator () ( const GLfloat *m ) const
    {
      ::glLoadMatrixf ( m );
    }
  };
};
template < typename Matrix_ > inline void loadMatrix ( const Matrix_ &m )
{
  typedef typename Matrix_::value_type value_type;
  Detail::LoadMatrix < value_type > () ( m.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Light.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct SetLight;
  template <> struct SetLight < GLdouble >
  {
    void operator () ( GLenum light, GLenum pname, const GLdouble *c ) const
    {
      GLfloat color[4] = 
      { 
        static_cast < GLfloat > ( c[0] ), 
        static_cast < GLfloat > ( c[1] ), 
        static_cast < GLfloat > ( c[2] ), 
        static_cast < GLfloat > ( c[3] )
      };
      ::glLightfv ( light, pname, color );
    }
  };
  template <> struct SetLight < GLfloat >
  {
    void operator () ( GLenum light, GLenum pname, const GLfloat *color ) const
    {
      ::glLightfv ( light, pname, color );
    }
  };
};
template < typename Vec4_ > inline void light ( GLenum light, GLenum pname, const Vec4_ &color )
{
  typedef Vec4_::value_type value_type;
  Detail::SetLight < value_type > () ( light, pname, color.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear color.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct SetClearColor;
  template <> struct SetClearColor < GLdouble >
  {
    void operator () ( GLdouble r, GLdouble g, GLdouble b, GLdouble a ) const
    {
      ::glClearColor ( static_cast < GLfloat > ( r ), 
                       static_cast < GLfloat > ( g ),
                       static_cast < GLfloat > ( b ), 
                       static_cast < GLfloat > ( a ) );
    }
  };
  template <> struct SetClearColor < GLfloat >
  {
    void operator () ( GLfloat r, GLfloat g, GLfloat b, GLfloat a ) const
    {
      ::glClearColor ( r, g, b, a );
    }
  };
};
template < typename Color_ > inline void clearColor ( const Color_ &c )
{
  typedef typename Color_::value_type value_type;
  Detail::SetClearColor < value_type > () ( c.red(), c.green(), c.blue(), c.alpha() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting model.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > struct SetLightModel;
  template <> struct SetLightModel < GLint >
  {
    void operator () ( GLenum pname, GLint param ) const
    {
      ::glLightModeli ( pname, param );
    }
  };
  template <> struct SetLightModel < GLfloat >
  {
    void operator () ( GLenum pname, GLfloat param ) const
    {
      ::glLightModelf ( pname, param );
    }
  };
};
template < typename T > inline void lightModel ( GLenum pname, T param )
{
  Detail::SetLightModel<T> () ( pname, param );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Viewport.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T1, typename T2 > struct SetViewport;
  template <> struct SetViewport < int, int >
  {
    void operator () ( int x, int y, int width, int height ) const
    {
      ::glViewport ( x, y, width, height );
    }
  };
  template <> struct SetViewport < int, unsigned int >
  {
    void operator () ( int x, int y, unsigned int width, unsigned int height ) const
    {
      ::glViewport ( x, y, width, height );
    }
  };
};
template < typename Viewport_ > inline void viewport ( const Viewport_ &vp )
{
  typedef typename Viewport_::LocationType LocationType;
  typedef typename Viewport_::SizeType SizeType;
  Detail::SetViewport < LocationType, SizeType > () ( vp.x(), vp.y(), vp.width(), vp.height() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Vertex arrays.
//
///////////////////////////////////////////////////////////////////////////////

template < typename SizeType_, typename StrideType_ > 
inline void vertexPointer ( SizeType_ size, StrideType_ stride, const double *array )
{
  ::glVertexPointer ( (GLint) size, GL_DOUBLE, (GLsizei) stride, (const GLvoid *) array );
}
template < typename SizeType_, typename StrideType_ > 
inline void vertexPointer ( SizeType_ size, StrideType_ stride, const float *array )
{
  ::glVertexPointer ( (GLint) size, GL_FLOAT, (GLsizei) stride, (const GLvoid *) array );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Interleaved arrays.
//
///////////////////////////////////////////////////////////////////////////////

template < typename StrideType_ > 
inline void interleavedArrays ( GLenum format, StrideType_ stride, const double *array )
{
  ::glInterleavedArrays ( format, (GLsizei) stride, (const GLvoid *) array );
}
template < typename StrideType_ > 
inline void interleavedArrays ( GLenum format, StrideType_ stride, const float *array )
{
  ::glInterleavedArrays ( format, (GLsizei) stride, (const GLvoid *) array );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the arrays that are set.
//
///////////////////////////////////////////////////////////////////////////////

template < typename IndexType_, typename SizeType_ > 
inline void drawArrays ( GLenum mode, IndexType_ first, SizeType_ count )
{
  ::glDrawArrays ( mode, (GLint) first, (GLsizei) count );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Begin and end pair.
//
///////////////////////////////////////////////////////////////////////////////

struct BeginAndEnd
{
  BeginAndEnd ( GLenum mode )
  {
    ::glBegin ( mode );
  }
  ~BeginAndEnd()
  {
    ::glEnd();
  }
};


}; // namespace GL
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_OPENGL_API_H_
