
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An element of the render-bin.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_ELEMENT_H_
#define _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_ELEMENT_H_

#include "GSG/Core/Referenced.h"
#include "GSG/Core/Color.h"


namespace GSG {


class Material;
class PrimitiveSet;
class Visitor;


///////////////////////////////////////////////////////////////////////////////
//
//  Base render-element class.
//
///////////////////////////////////////////////////////////////////////////////

class GSG_CORE_EXPORT RenderElement : public Referenced
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( RenderElement, Referenced );

  virtual void          _accept ( Visitor &v ) = 0;

protected:

  explicit RenderElement(); // Not implemented by design.
  RenderElement ( const RenderElement & );
  virtual ~RenderElement();
};


///////////////////////////////////////////////////////////////////////////////
//
//  PrimitiveSet element class.
//
///////////////////////////////////////////////////////////////////////////////

class GSG_CORE_EXPORT PrimitiveSetElement : public RenderElement
{
public:

  GSG_DECLARE_REFERENCED ( PrimitiveSetElement );
  GSG_DECLARE_LOCAL_TYPEDEFS ( PrimitiveSetElement, RenderElement );

  PrimitiveSetElement ( PrimitiveSet * );
  PrimitiveSetElement ( const PrimitiveSetElement & );

  const PrimitiveSet *set() const { return _set; }

  GSG_DECLARE_ACCEPT ( PrimitiveSetElement );

protected:

  explicit PrimitiveSetElement(); // Not implemented by design.
  virtual ~PrimitiveSetElement();

private:

  PrimitiveSet *_set;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix-loading element class.
//
///////////////////////////////////////////////////////////////////////////////

class GSG_CORE_EXPORT MatrixLoadElement : public RenderElement
{
public:

  GSG_DECLARE_REFERENCED ( MatrixLoadElement );
  GSG_DECLARE_LOCAL_TYPEDEFS ( MatrixLoadElement, RenderElement );

  MatrixLoadElement ( const Matrix & );
  MatrixLoadElement ( const MatrixLoadElement & );

  const Matrix &matrix() const { return _matrix; }

  GSG_DECLARE_ACCEPT ( MatrixLoadElement );

protected:

  explicit MatrixLoadElement(); // Not implemented by design.
  virtual ~MatrixLoadElement();

private:

  Matrix _matrix;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix-mode element class.
//
///////////////////////////////////////////////////////////////////////////////

class GSG_CORE_EXPORT MatrixModeElement : public RenderElement
{
public:

  GSG_DECLARE_REFERENCED ( MatrixModeElement );
  GSG_DECLARE_LOCAL_TYPEDEFS ( MatrixModeElement, RenderElement );

  enum Mode { MODELVIEW, PROJECTION, TEXTURE };

  MatrixModeElement ( Mode );
  MatrixModeElement ( const MatrixModeElement & );

  Mode mode() const { return _mode; }

  GSG_DECLARE_ACCEPT ( MatrixModeElement );

protected:

  explicit MatrixModeElement(); // Not implemented by design.
  virtual ~MatrixModeElement();

private:

  Mode _mode;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Material element class.
//
///////////////////////////////////////////////////////////////////////////////

class GSG_CORE_EXPORT MaterialElement : public RenderElement
{
public:

  GSG_DECLARE_REFERENCED ( MaterialElement );
  GSG_DECLARE_LOCAL_TYPEDEFS ( MaterialElement, RenderElement );

  MaterialElement ( Material * );
  MaterialElement ( const MaterialElement & );

  const Material *material() const { return _material; }

  GSG_DECLARE_ACCEPT ( MaterialElement );

protected:

  explicit MaterialElement(); // Not implemented by design.
  virtual ~MaterialElement();

private:

  Material *_material;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Color element class.
//
///////////////////////////////////////////////////////////////////////////////

class GSG_CORE_EXPORT ColorElement : public RenderElement
{
public:

  GSG_DECLARE_REFERENCED ( ColorElement );
  GSG_DECLARE_LOCAL_TYPEDEFS ( ColorElement, RenderElement );

  ColorElement ( const Color & );
  ColorElement ( const ColorElement & );

  const Color &color() const { return _color; }

  GSG_DECLARE_ACCEPT ( ColorElement );

protected:

  explicit ColorElement(); // Not implemented by design.
  virtual ~ColorElement();

private:

  Color _color;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_RENDER_BIN_ELEMENT_H_
