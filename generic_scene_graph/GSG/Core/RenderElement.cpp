
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
#include "GSG/Core/PrimitiveSet.h"

using namespace GSG;

GSG_IMPLEMENT_ACCEPT_ELEMENT ( PrimitiveSetElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( MatrixLoadElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( MatrixModeElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( MaterialElement );
GSG_IMPLEMENT_ACCEPT_ELEMENT ( ColorElement );

GSG_IMPLEMENT_CLONE ( PrimitiveSetElement );
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
PrimitiveSetElement::PrimitiveSetElement ( PrimitiveSet *p ) : RenderElement(), _set ( p )
{
  ErrorChecker ( 0x0 != _set );
  _set->ref();
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
PrimitiveSetElement::PrimitiveSetElement ( const PrimitiveSetElement &e ) : RenderElement ( e ), 
  _set ( PrimitiveSet::safeClonePrimitiveSet ( e._set ) )
{
  ErrorChecker ( 0x0 != _set );
  _set->ref();
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
PrimitiveSetElement::~PrimitiveSetElement()
{
  if ( _set )
    _set->unref();
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
