///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Eric W. Schmidt
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager: STL Facet housekeeping and file ops
//
///////////////////////////////////////////////////////////////////////////////

#include "DbStlFileIO.h"
#include "DbStlFacetManager.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Override the default push function so that the new matrix is multiplied
//  by the matrix on top of the stack. This version pushes a copy of the
//  existing top matrix M back onto the stack,and then multiplies this new top 
//  matrix by itself.  Net effect is new matrix M^2 on top of stack.  This 
//  function is only useful if there are two identicle transformation matrices
//  applied in a row.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::TransMatStack::push()
{
  SL_ASSERT ( !empty() ); 
  if ( !empty() )
  {
    SlMatrix44f tm( top() );
    push( tm );
    top().multLeft( tm );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Override the default push function so that the new matrix is multiplied
//  by the matrix on top of the stack. This version creates a new matrix nM
//  which is equal to M * T (T being the current top matrix on stack), and 
//  pushes nM onto the top of the stack.  This allows for a cumulative
//  transformation matrix.  This version will be the most commonly used in
//  this application.
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::TransMatStack::push( const SlMatrix44f &M )
{
  if ( !empty() )
  {
    SlMatrix44f tm( top() );
    push( M );
    top().multLeft( tm );
  }
  else
    push( M );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Adds a facet
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::addFacet( SlVec3f vertices[3], SlVec3f &normal )
{
  DbStlFacetManager::facet f( vertices, normal );
  _facets.push_back( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the facet information to an STL file.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::storeData ( const char *filename, const StlFileMode &mode )
{
  Facets::iterator i;
  SlVec3<float> vertices[3], normal;

  if ( mode == STL_ASCII_FILE_MODE )
  {
    DbStlAsciiOutputFile asciiOut( filename );
    for( i = _facets.begin(); i != _facets.end(); i++ )
    {
      i->getVertices( vertices );
      i->getNormal( normal );
      asciiOut.writeFacet( vertices[0], vertices[1], vertices[2], normal );
    }
    asciiOut.close();
    return( true );
  }
  else if ( mode == STL_BINARY_FILE_MODE )
  {
    DbStlBinaryOutputFile binOut( filename );
    for( i = _facets.begin(); i != _facets.end(); i++ )
    {
      i->getVertices( vertices );
      i->getNormal( normal );
      binOut.writeFacet( vertices[0], vertices[1], vertices[2], normal );
    }
    binOut.close();
    return( true );
  }
  else
    return( false );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// this stuff used to be in header file... but got linker errors
   
///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::facet
//
//  constructor
//
///////////////////////////////////////////////////////////////////////////////
DbStlFacetManager::facet::facet( const SlVec3<float> vertices[3], const SlVec3<float> &normal )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
  _normal.setValue( normal );
}



///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setValue
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////
void inline DbStlFacetManager::facet::setValue( const SlVec3<float> vertices[3], const SlVec3<float> &normal )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
  _normal.setValue( normal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setValue
//
//  Set the value of the normal
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::facet::setNormal( const SlVec3<float> &normal )
{
  _normal.setValue( normal );
}



///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setVertices
//
//  Set the value of the vertices
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::facet::setVertices( const SlVec3<float> vertices[3] )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::getNormal
//
//  Gets the normal
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::facet::getNormal( SlVec3<float> &normal )
{
  normal.setValue( _normal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::getVertices
//
//  Gets the vertices
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::facet::getVertices( SlVec3<float> vertices[3] )
{
  vertices[0].setValue( _vertices[0] );
  vertices[1].setValue( _vertices[1] );
  vertices[2].setValue( _vertices[2] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////////////