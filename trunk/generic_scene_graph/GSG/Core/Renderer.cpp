
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A renderer (a sequence of visitors).
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Renderer.h"
#include "GSG/Core/Transform.h"
#include "GSG/Core/Lod.h"
#include "GSG/Core/Camera.h"
#include "GSG/Core/Shape.h"
#include "GSG/Core/Functors.h"
#include "GSG/Core/Adaptors.h"

using namespace GSG;

GSG_IMPLEMENT_CLONE ( Renderer );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() : Visitor(), 
  _visitors()
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Renderer ( const Renderer &s ) : Visitor ( s ), 
  _visitors ( s._visitors )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::~Renderer()
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Append the child visitor.
//
/////////////////////////////////////////////////////////////////////////////

void Renderer::append ( Visitor *v )
{
  Lock lock ( this );
  _visitors.insert ( _visitors.end(), Visitor::ValidPtr ( v ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Prepend the child visitor.
//
/////////////////////////////////////////////////////////////////////////////

void Renderer::prepend ( Visitor *v )
{
  Lock lock ( this );
  _visitors.insert ( _visitors.begin(), Visitor::ValidPtr ( v ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Insert the child.
//
/////////////////////////////////////////////////////////////////////////////

void Renderer::insert ( Visitors::iterator beforeMe, Visitor *v )
{
  Lock lock ( this );
  _visitors.insert ( beforeMe, Visitor::ValidPtr ( v ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Iterators to the visitors.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Visitors::const_iterator Renderer::begin() const
{
  return _visitors.begin();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Iterators to the visitors.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Visitors::iterator Renderer::begin()
{
  return _visitors.begin();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Iterators to the visitors.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Visitors::const_iterator Renderer::end() const
{
  return _visitors.end();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Iterators to the visitors.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Visitors::iterator Renderer::end()
{
  return _visitors.end();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Traverse the scene.
//
/////////////////////////////////////////////////////////////////////////////

void Renderer::_traverse ( Node &scene )
{
  typedef GSG::Detail::Traverse < Node > Traverse;
  std::for_each ( _visitors.begin(), _visitors.end(), Traverse ( &scene ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the internal state.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::reset()
{
  std::for_each ( _visitors.begin(), _visitors.end(), Detail::mem_fun_void ( &Visitor::reset ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewport ( const Viewport &vp )
{
  Lock lock ( this );
  BaseClass::viewport ( vp );
  for ( iterator i = _visitors.begin(); i != _visitors.end(); ++i )
    (*i)->viewport ( vp );
}
