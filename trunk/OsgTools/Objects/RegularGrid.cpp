
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
//  Geometry represented by a regular grid.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Objects/RegularGrid.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

using namespace OsgTools::Objects;

OSG_TOOLS_OBJECTS_IMPLEMENT_CLASS ( RegularGrid );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RegularGrid::RegularGrid() : BaseClass(),
  _columns ( 0 ),
  _rows    ( 0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

RegularGrid::RegularGrid ( const RegularGrid &copyMe ) : BaseClass ( copyMe ),
  _columns ( 10 ),
  _rows    ( 1 )
{
  USUL_TRACE_SCOPE;
  Guard guard ( copyMe.mutex() );
  _columns = copyMe._columns;
  _rows    = copyMe._rows;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RegularGrid::~RegularGrid()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &RegularGrid::_destroy ), "3011328635" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void RegularGrid::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the object.
//
///////////////////////////////////////////////////////////////////////////////

void RegularGrid::update ( Usul::Interfaces::IUnknown *unknown )
{
  USUL_TRACE_SCOPE;

  // Ensure we're no longer dirty when scope ends.
  OSG_TOOLS_SCOPE_END_SET_DIRTY_STATE ( Dirty::ALL, false );

  // Get the dirty state.
  const bool dirtyVertices  ( this->isDirty ( Dirty::VERTICES   ) );
  const bool dirtyNormals   ( this->isDirty ( Dirty::NORMALS    ) );
  const bool dirtyColors    ( this->isDirty ( Dirty::COLORS     ) );
  const bool dirtyTexCoords ( this->isDirty ( Dirty::TEX_COORDS ) );

  // Update base class.
  BaseClass::update ( unknown );

  const unsigned int numRows ( this->numRows() );
  const unsigned int numColumns ( this->numColumns() );
  const unsigned int numVertices ( numRows * numColumns );

  // Are the vertices or normals dirty?
  if ( ( true == dirtyVertices ) || ( true == dirtyNormals ) )
  {
    // Get the vertices and normals.
    VerticesPtr v ( this->_getVertices ( true ) );
    NormalsPtr  n ( this->_getNormals  ( true ) );

    // Set number of vertices.
    v->resize ( numVertices );
    n->resize ( numVertices );

    // Set the vertices and normals.
    this->_setPointValues();
  }

  // Are the colors dirty?
  if ( true == dirtyColors )
  {
    // Get the colors.
    ColorsPtr c ( this->_getColors ( true ) );

    // Set number of colors.
    c->resize ( numVertices );

    // Set the colors.
    this->_setColorValues();
  }

  // Are the texture coordinates dirty?
  if ( true == dirtyTexCoords )
  {
    // Get the texture coordinates.
    TexCoordsPtr t ( this->_getTexCoords ( true ) );

    // Set number of texture coordinates.
    t->resize ( numVertices );

    // Set the texture coordinates.
    this->_setTexCoordValues();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

void RegularGrid::numColumns ( unsigned int c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _columns = c;
  this->setDirty ( Dirty::VERTICES, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int RegularGrid::numColumns() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _columns;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

void RegularGrid::numRows ( unsigned int r )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _rows = r;
  this->setDirty ( Dirty::VERTICES, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int RegularGrid::numRows() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _rows;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the colors. Default behavior is no colors.
//
///////////////////////////////////////////////////////////////////////////////

void RegularGrid::_setColorValues()
{
  USUL_TRACE_SCOPE;
  this->_setColors ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture coordinates. Default behavior is no texture coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void RegularGrid::_setTexCoordValues()
{
  USUL_TRACE_SCOPE;
  this->_setTexCoords ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vertices and normals. Default behavior does nothing.
//
///////////////////////////////////////////////////////////////////////////////

void RegularGrid::_setPointValues()
{
  USUL_TRACE_SCOPE;
}
