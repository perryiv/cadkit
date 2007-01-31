
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An OpenGL drawing class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_OPENGL_DRAW_H_
#define _GENERIC_SCENE_GRAPH_OPENGL_DRAW_H_

#include "GSG/OpenGL/OglDll.h"

#include "GSG/Core/Draw.h"


namespace GSG {


class Shape;
class Primitive;
class GlContext;
class PrimitiveElement;
class TransformElement;
class MaterialElement;
class ColorElement;


class GSG_OPENGL_EXPORT GlDraw : public Draw
{
public:

  GSG_DECLARE_CLONE ( GlDraw );
  GSG_DECLARE_LOCAL_TYPEDEFS ( GlDraw, Draw );

  // Constructors.
  GlDraw ( RenderBin *bin = 0x0, 
           const Viewport &vp = Viewport(), 
           GlContext *c = 0x0 );
  GlDraw ( const GlDraw &d );

  // Initialize.
  void                  init ( const Viewport &vp, GlContext *context );

  // Set the background color.
  virtual void          backgroundColor ( const Color &c );

  //  Set/get the OpenGL context.
  const GlContext *     context() const;
  void                  context ( GlContext *ct );

  // Set the viewport.
  virtual void          viewport ( const Viewport &vp );

protected:

  virtual ~GlDraw();

  typedef unsigned int gl_enum;

  enum
  {
    COLOR_MATERIAL = 0x00000001,
    LIGHTING       = 0x00000002,
  };

  void                  _enable  ( UnsignedInteger f, gl_enum g );
  void                  _disable ( UnsignedInteger f, gl_enum g );

  virtual void          _makeContextCurrent() const;

  virtual void          _swapBuffers() const;

  virtual void          _preTraverse  ( Node & );
  virtual void          _traverse     ( Node & );
  virtual void          _postTraverse ( Node & );

  GSG_DECLARE_VISIT_ELEMENT_FUNCTIONS;

private:

  mutable Pointer < GlContext > _context;
  UnsignedInteger _stateBits;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_OPENGL_DRAW_H_
