
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

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/RenderElement.h"
#include "GSG/Core/Visitor.h"
#include "GSG/Core/PushPop.h"
#include "GSG/Core/Material.h"
#include "GSG/Core/Transform.h"
#include "GSG/Core/Primitive.h"

using namespace GSG;

GSG_IMPLEMENT_ACCEPT_ELEMENT ( PrimitiveElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( MatrixLoadElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( MatrixModeElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( MaterialElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( ColorElement );

GSG_IMPLEMENT_CLONE ( PrimitiveElement );
GSG_IMPLEMENT_CLONE ( MatrixLoadElement );
GSG_IMPLEMENT_CLONE ( MatrixModeElement );
GSG_IMPLEMENT_CLONE ( MaterialElement );
GSG_IMPLEMENT_CLONE ( ColorElement );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructors.
//
///////////////////////////////////////////////////////////////////////////////

RenderElement::RenderElement() : Referenced()
{
}
PrimitiveElement::PrimitiveElement ( Primitive *p ) : RenderElement(), _primitive ( p )
{
  ErrorChecker ( 0x0 != _primitive );
  _primitive->ref();
}
MatrixLoadElement::MatrixLoadElement ( const Matrix &m ) : RenderElement(), _matrix ( m )
{
}
MatrixModeElement::MatrixModeElement ( Mode m ) : RenderElement(), _mode ( m )
{
}
MaterialElement::MaterialElement ( Material *m, Mode mode ) : RenderElement(), _material ( m ), _mode ( mode )
{
  ErrorChecker ( 0x0 != _material );
  _material->ref();
}
ColorElement::ColorElement ( const Color &c ) : RenderElement(), _color ( c )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructors.
//
///////////////////////////////////////////////////////////////////////////////

RenderElement::RenderElement ( const RenderElement &e )
{
}
PrimitiveElement::PrimitiveElement ( const PrimitiveElement &e ) : RenderElement ( e ), 
  _primitive ( Primitive::safeClonePrimitive ( e._primitive ) )
{
  ErrorChecker ( 0x0 != _primitive );
  _primitive->ref();
}
MatrixLoadElement::MatrixLoadElement ( const MatrixLoadElement &e ) : RenderElement ( e ), 
  _matrix ( e._matrix )
{
}
MatrixModeElement::MatrixModeElement ( const MatrixModeElement &e ) : RenderElement ( e ), 
  _mode ( e._mode )
{
}
MaterialElement::MaterialElement ( const MaterialElement &e ) : RenderElement ( e ), 
  _material ( Material::safeCloneMaterial ( e._material ) ),
  _mode ( e._mode )
{
  ErrorChecker ( 0x0 != _material );
  _material->ref();
}
ColorElement::ColorElement ( const ColorElement &e ) : RenderElement ( e ), 
  _color ( e._color )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructors.
//
///////////////////////////////////////////////////////////////////////////////

RenderElement::~RenderElement()
{
}
PrimitiveElement::~PrimitiveElement()
{
  if ( _primitive )
    _primitive->unref();
}
MatrixLoadElement::~MatrixLoadElement()
{
}
MatrixModeElement::~MatrixModeElement()
{
}
MaterialElement::~MaterialElement()
{
  if ( _material )
    _material->unref();
}
ColorElement::~ColorElement()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access.
//
///////////////////////////////////////////////////////////////////////////////

const Primitive *PrimitiveElement::primitive() const
{
  return _primitive;
}
const Material *MaterialElement::material() const
{
  return _material;
}
