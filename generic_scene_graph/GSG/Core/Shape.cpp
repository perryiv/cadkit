
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A shape node.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Shape.h"
#include "GSG/Core/Attributes.h"
#include "GSG/Core/Visitor.h"
#include "GSG/Core/PushPop.h"

using namespace GSG;

GSG_IMPLEMENT_ACCEPT_NODE ( Shape );
GSG_IMPLEMENT_CLONE  ( Shape );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Shape::Shape() : Node(), 
  _sets(),
  _attributes ( 0x0 )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Shape::Shape ( const Shape &s ) : Node ( s ), 
  _sets ( s._sets ),
  _attributes ( s._attributes )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Shape::~Shape()
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Append the primitive-set.
//
/////////////////////////////////////////////////////////////////////////////

void Shape::append ( PrimitiveSet *ps )
{
  Lock lock ( this );
  _sets.insert ( _sets.end(), PrimitiveSet::ValidPtr ( ps ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Prepend the primitive-set.
//
/////////////////////////////////////////////////////////////////////////////

void Shape::prepend ( PrimitiveSet *ps )
{
  Lock lock ( this );
  _sets.insert ( _sets.begin(), PrimitiveSet::ValidPtr ( ps ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Insert the primitive-set.
//
/////////////////////////////////////////////////////////////////////////////

void Shape::insert ( Sets::iterator beforeMe, PrimitiveSet *ps )
{
  Lock lock ( this );
  _sets.insert ( beforeMe, PrimitiveSet::ValidPtr ( ps ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the attributes.
//
/////////////////////////////////////////////////////////////////////////////

void Shape::attributes ( Attributes *a )
{
  Lock lock ( this );
  _attributes = a;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update this shape.
//
/////////////////////////////////////////////////////////////////////////////

void Shape::update()
{
  Lock lock ( this );

  // Call the base class's function first.
  BaseClass::update();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Calculate the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

void Shape::calculateBoundingSphere()
{
  Lock lock ( this );

  // Loop through the primitive sets and grow the bounding sphere.
  BoundingSphere bound;
  for ( Sets::iterator i = _sets.begin(); i != _sets.end(); ++i )
  {
    PrimitiveSet::ValidPtr ps ( *i );
    ps->calculateBoundingSphere();
    bound.grow ( ps->boundingSphere() );
  }

  // Set this instance's sphere.
  this->boundingSphere ( bound );
}
