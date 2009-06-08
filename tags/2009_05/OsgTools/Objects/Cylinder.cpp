
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Represents a cylinder.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Objects/Cylinder.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>

using namespace OsgTools::Objects;

OSG_TOOLS_OBJECTS_IMPLEMENT_CLASS ( Cylinder );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder() : BaseClass(),
  _radii   ( 1, 1 ),
  _height  ( 1 ),
  _bottom  ( 0, 0, 0 ),
  _axis    ( 0, 1, 0 ),
  _indices ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder ( const Vec2d &r, double h ) : BaseClass(),
  _radii   ( r ),
  _height  ( h ),
  _bottom  ( 0, 0, 0 ),
  _axis    ( 0, 1, 0 ),
  _indices ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder ( const Vec2d &r, const Vec3d &b, const Vec3d &t ) : BaseClass(),
  _radii   ( r ),
  _height  ( b.distance ( t ) ),
  _bottom  ( b ),
  _axis    ( t - b ),
  _indices ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder ( const Vec2d &r, double h, const Vec3d &b, const Vec3d &a ) : BaseClass(),
  _radii   ( r ),
  _height  ( h ),
  _bottom  ( b ),
  _axis    ( a ),
  _indices ( 0x0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder ( const Cylinder &copyMe ) : BaseClass ( copyMe ),
  _radii   ( 1, 1 ),
  _height  ( 1 ),
  _bottom  ( 0, 0, 0 ),
  _axis    ( 0, 1, 0 ),
  _indices ( 0x0 )
{
  USUL_TRACE_SCOPE;
  Guard guard ( copyMe.mutex() );
  _radii   = copyMe._radii;
  _height  = copyMe._height;
  _bottom  = copyMe._bottom;
  _axis    = copyMe._axis;
  _indices = ( ( true == copyMe._indices.valid() ) ? new Indices ( *(copyMe._indices), osg::CopyOp::DEEP_COPY_ALL ) : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::~Cylinder()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Cylinder::_destroy ), "1368370743" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::_destroy()
{
  USUL_TRACE_SCOPE;
  _indices = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the object.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::update ( Usul::Interfaces::IUnknown *unknown )
{
  USUL_TRACE_SCOPE;

  // Ensure we're no longer dirty when scope ends.
  OSG_TOOLS_SCOPE_END_SET_DIRTY_STATE ( Dirty::ALL, false );

  // Update base class.
  BaseClass::update ( unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the points.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::_setPointValues()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get the numbers.
  const unsigned int numRows ( this->numRows() );
  const unsigned int numCols ( this->numColumns() );
  const unsigned int numVerts ( numCols * numRows );

  // Get the vertices and normals.
  VerticesPtr vertices ( this->_getVertices ( true ) );
  NormalsPtr  normals  ( this->_getNormals  ( true ) );

  // Should be true.
  if ( ( vertices->size() != numVerts ) || ( normals->size() != numVerts ) )
  {
    throw std::runtime_error ( "Error 2241799180: Inconsistent number of vertices and normals" );
  }

  // Calculate the number of indices needed for triangles (not tri-strips).
  const unsigned int numIndices ( ( numRows - 1 ) * ( numCols - 1 ) * 6 );

  // Make sure we have the right number of indices.
  if ( false == _indices.valid() )
  {
    _indices = new Indices ( osg::PrimitiveSet::TRIANGLES );
  }
  if ( numIndices != _indices->size() )
  {
    _indices->resize ( numIndices );
  }

  // Loop through the rows.
  for ( unsigned int i = 0; i < numRows - 1; ++i )
  {
    // Height for this row.
    const double height ( _height * ( static_cast < double > ( ( i + 1 ) ) / ( numRows - 1 ) ) );

    // Loop through all the columns.
    for ( unsigned int j = 0; j < numCols; ++j )
    {
      const unsigned int index ( i * numCols + j );
      osg::Vec3f &v ( vertices->at ( index ) );
      osg::Vec3f &n ( normals->at  ( index ) );

      //Here...
      //Bumping vertices around because you were marked as dirty.
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the colors.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::_setColorValues()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make sure we either have the same number as vertices, or none at all.
  ColorsPtr c ( this->_getColors ( false ) );
  if ( true == c.valid() )
  {
    if ( c->size() != this->numVertices() )
    {
      // Handle mis-matched size.
      this->_setColors ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture coordinates. Default behavior is no texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::_setTexCoordValues()
{
  USUL_TRACE_SCOPE;

  // Make sure we either have the same number as vertices, or none at all.
  TexCoordsPtr t ( this->_getTexCoords ( false ) );
  if ( true == t.valid() )
  {
    if ( t->size() != this->numVertices() )
    {
      // Handle mis-matched size.
      this->_setTexCoords ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the radii.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::radii ( const Vec2d &r )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _radii = r;
  this->setDirty ( Dirty::VERTICES, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radius.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Vec2d Cylinder::radii() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _radii;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the height.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::height ( double h )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _height = h;
  this->setDirty ( Dirty::VERTICES, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height.
//
///////////////////////////////////////////////////////////////////////////////

double Cylinder::height() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bottom.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::bottom ( const Vec3d &b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _bottom = b;
  this->setDirty ( Dirty::VERTICES, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Vec3d Cylinder::bottom() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _bottom;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the axis.
//
///////////////////////////////////////////////////////////////////////////////

void Cylinder::axis ( const Vec3d &a )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _axis = a;
  this->setDirty ( Dirty::VERTICES, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the height.
//
///////////////////////////////////////////////////////////////////////////////

Cylinder::Vec3d Cylinder::axis() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _axis;
}
