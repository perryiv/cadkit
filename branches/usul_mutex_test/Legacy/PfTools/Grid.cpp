
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a grid.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Grid.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Pointers/Pointers.h"

#include "Performer/pf/pfGeode.h"
#include "Performer/pr/pfGeoArray.h"
#include "Performer/pf/pfDCS.h"

#include <stdexcept>

using namespace PfTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Grid::Grid() :
  _numBlocks ( 10, 10 ),
  _color     ( 1, 0, 0, 1 ),
  _size      ( 1, 1 ),
  _center    ( 0, 0, 0 ),
  _lineWidth ( 1 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build coordinate axes.
//
///////////////////////////////////////////////////////////////////////////////

::pfNode *Grid::operator()() const
{
  // Check size.
  if ( 0 == _size[0] || 0 == _size[1] )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 2351180092, invalid grid size: ", _size[0], ", ", _size[1] );
  }

  // Check number of blocks.
  if ( 0 == _numBlocks[0] || 0 == _numBlocks[1] )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 2856992905, invalid number of grid blocks: ", _numBlocks[0], ", ", _numBlocks[1] );
  }

  // For readability.
  const float cx ( _center[0] );
  const float cy ( _center[1] );
  const float cz ( _center[2] );

  // Set the origin.
  const float halhSizeX ( _size[0] / 2 );
  const float halhSizeZ ( _size[1] / 2 );
  const float originX ( cx - halhSizeX );
  const float originY ( cy );
  const float originZ ( cz - halhSizeZ );

  // Allocate the vertices and colors.
  const unsigned int numRows ( _numBlocks[0] + 1 );
  const unsigned int numColumns ( _numBlocks[1] + 1 );
  const unsigned int numVertices ( 2 * ( numRows + numColumns ) );
  typedef USUL_VALID_ARRAY_POINTER(::pfVec3) ArrayVec3;
  typedef USUL_VALID_ARRAY_POINTER(::pfVec4) ArrayVec4;
  ArrayVec3 vertices ( (pfVec3 *) ::pfMalloc ( sizeof ( pfVec3 ) * numVertices, ::pfGetSharedArena() ) );
  ArrayVec4 colors   ( (pfVec4 *) ::pfMalloc ( sizeof ( pfVec4 ) * numVertices, ::pfGetSharedArena() ) );

  // Shortcuts.
  ::pfVec3 *vertex = vertices.get();
  ::pfVec4 *color  = colors.get();

  // There is one line segment for each row and column.
  typedef USUL_VALID_ARRAY_POINTER(int) PrimitiveLengths;
  const unsigned int numPrims ( numRows + numColumns );
  PrimitiveLengths primLengths ( (int *) ::pfMalloc ( sizeof ( int ) * numPrims, ::pfGetSharedArena() ) );
  int *primLength = primLengths;

  // Used in the loop.
  const float sizeX ( _size[0] );
  const float sizeZ ( _size[1] );
  const Vec4f &c = _color;
  const float xMin ( originX ), xMax ( originX + sizeX );
  const float zMin ( originZ ), zMax ( originZ + sizeZ );

  // Loop through all the rows.
  for ( unsigned int i = 0; i < numRows; ++i )
  {
    float percent ( float ( i + 1 ) / ( numRows - 1 ) );
    float z ( originZ + percent * sizeZ );

    vertex->set ( xMin, originY, z ); vertex += 1;
    vertex->set ( xMax, originY, z ); vertex += 1;
    color->set ( c[0], c[1], c[2], c[3] ); color += 1;
    color->set ( c[0], c[1], c[2], c[3] ); color += 1;

    *primLength = 2;
    primLength += 1;
  }

  // Loop through all the columns.
  for ( unsigned int j = 0; j < numColumns; ++j )
  {
    float percent ( float ( j + 1 ) / ( numColumns - 1 ) );
    float x ( originX + percent * sizeX );

    vertex->set ( x, originY, zMin ); vertex += 1;
    vertex->set ( x, originY, zMax ); vertex += 1;
    color->set ( c[0], c[1], c[2], c[3] ); color += 1;
    color->set ( c[0], c[1], c[2], c[3] ); color += 1;

    *primLength = 2;
    primLength += 1;
  }

  // Should be true.
  USUL_ASSERT ( vertices + numVertices == vertex );
  USUL_ASSERT ( colors + numVertices == color );
  USUL_ASSERT ( primLengths + numPrims == primLength );

  // Allocate geometry.
  typedef USUL_VALID_REF_POINTER(::pfGeoArray) ValidGeoArray;
  ValidGeoArray geometry ( new ::pfGeoArray );

  // Set the type and number of primitives.
  geometry->setPrimType ( PFGS_LINES );
  geometry->setNumPrims ( numPrims );
  geometry->setPrimLengths ( primLengths );

  // Set the vertices.
  ::pfVertexAttr *vertexAttribute = pfGeoArray::queryAttrType ( "vertex" );
  USUL_ASSERT ( vertexAttribute );
  geometry->addAttr ( vertexAttribute, 3, GL_FLOAT, 0, vertices.get() );

  // Set the line-width.
  geometry->setLineWidth ( _lineWidth );

  // Set the colors.
  ::pfVertexAttr *colorAttribute = pfGeoArray::queryAttrType ( "color" );
  USUL_ASSERT ( colorAttribute );
  geometry->addAttr ( colorAttribute, 4, GL_FLOAT, 0, colors.get() );

  // Make the geode and add the geometry.
  typedef USUL_VALID_REF_POINTER(::pfGeode) ValidGeode;
  ValidGeode geode ( new ::pfGeode );
  geode->addGSet ( geometry.get() );

  // Have the array-pointers release their internal pointers.
  vertices.release();
  colors.release();
  primLengths.release();

  // Return the geode.
  return geode.release();
}
