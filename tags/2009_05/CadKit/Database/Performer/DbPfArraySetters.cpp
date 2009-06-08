
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfArraySetters: Implementations of the abstract array-setting class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DbPfPrecompiled.h"
#include "DbPfArraySetters.h"
#include "DbPfPerformer.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Get the lengths of the primitives.
//
///////////////////////////////////////////////////////////////////////////////

int *DbPfVertexSetter::getPrimitiveLengths() const
{
  // Handle trivial case.
  if ( _ranges.empty() )
    return NULL;

  // Allocate an array to hold the lengths.
  unsigned int numPrimitives ( _ranges.size() );
  int *lengths = new int[numPrimitives];
  if ( NULL == lengths )
    return false;

  // Should be true.
  SL_ASSERT ( 0 == _ranges[0][0] );
  SL_ASSERT ( 0  < _ranges[0][1] );

  // Set the first one.
  lengths[0] = _ranges[0][1];

  // Loop through all the rest of the ranges.
  int start ( lengths[0] );
  for ( unsigned int i = 1; i < numPrimitives; ++i )
  {
    // Make sure the range starts where it should. Note: Performer only 
    // supports primitives that reside side-by-side (so to speak) in the 
    // vertex list. Where as OpenSceneGraph supports overlapping of these 
    // primitive definitions.
    if ( _ranges[i][0] != start )
    {
      SL_ASSERT ( 0 ); // Heads up.
      return false;
    }

    // Set the length of this primitive.
    lengths[i] = _ranges[i][1];

    // Update the position where the next primitive should start.
    start += lengths[i];
  }

  // Return the new array of lengths.
  return lengths;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfVertexSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then return.
  if ( index >= _numVertices )
    return false;

  // Set the vertex.
  _vertices[index].set ( vec[0], vec[1], vec[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfNormalSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then resize.
  if ( index >= _numNormals )
    return false;

  // Set the vertex.
  _normals[index].set ( vec[0], vec[1], vec[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfColorSetter::setData ( const unsigned int &index, const SlVec4f &vec )
{
  // If we're out of range then resize.
  if ( index >= _numColors )
    return false;

  // Set the vertex.
  _colors[index].set ( vec[0], vec[1], vec[2], vec[3] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfTextureCoordSetter::setData ( const unsigned int &index, const SlVec2f &vec )
{
  // If we're out of range then resize.
  if ( index >= _numTexCoords )
    return false;

  // Set the vertex.
  _texCoords[index].set ( vec[0], vec[1] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive length.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfVertexSetter::setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length )
{
  // If we're out of range then resize.
  if ( index >= _ranges.size() )
    return false;

  // Set the primitive.
  _ranges[index].setValue ( start, length );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of primitives.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfVertexSetter::setNumPrimitives ( const unsigned int &num )
{
  SL_ASSERT ( num > 0 );
  _ranges.resize ( num );
  return num == _ranges.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfVertexSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( 0 == _numVertices );
  SL_ASSERT ( size > 0 );
  _vertices = new pfVec3[size];
  _numVertices = ( NULL != _vertices ) ? size : 0;
  return _numVertices == size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfNormalSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( 0 == _numNormals );
  SL_ASSERT ( size > 0 );
  _normals = new pfVec3[size];
  _numNormals = ( NULL != _normals ) ? size : 0;
  return _numNormals == size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfColorSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( 0 == _numColors );
  SL_ASSERT ( size > 0 );
  _colors = new pfVec4[size];
  _numColors = ( NULL != _colors ) ? size : 0;
  return _numColors == size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbPfTextureCoordSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( 0 == _numTexCoords );
  SL_ASSERT ( size > 0 );
  _texCoords = new pfVec2;
  _numTexCoords = ( NULL != _texCoords ) ? size : 0;
  return _numTexCoords == size;
}
