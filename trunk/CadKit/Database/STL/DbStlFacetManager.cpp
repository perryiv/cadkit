
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

//???????????
// I need this up here or else I get this VC++ error (and many others):
// SlTemplateSupport.h(79) : error C2039: 'sqrtl' : is not a member of '`global namespace''
#include <math.h>
//????????????

#include "DbStlPrecompiled.h"
#include "DbStlAsciiOutputFile.h"
#include "DbStlBinaryOutputFile.h"
#include "DbStlFacetManager.h"

/*DEBUG*/ #include <iostream>
#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlStringFunctions.h"
#include "Standard/SlMessageIds.h"

using namespace CadKit;

/*DEBUG*/extern std::ofstream stl_out;
///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::DbStlFacetManager() constructor
//
///////////////////////////////////////////////////////////////////////////////
DbStlFacetManager::DbStlFacetManager( ) : /*DEBUG*/_tnv(0), _tns(0)
/*
 :
  _facets( new Facets ),
	_transforms( new TransformStack ), // Eric, these are objects in the header, not pointers.
  _vSetter( new DbStlVertexSetter ),
  _nSetter( new DbStlNormalSetter ),
  _vbuf( new Vertices ),
  _nbuf( new Normals )
*/
{
  _vSetter.setBuffer( &_vbuf );
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::~DbStlFacetManager() destructor
//
///////////////////////////////////////////////////////////////////////////////
DbStlFacetManager::~DbStlFacetManager( )
{
/*
  delete( _facets );
	delete( _transforms );
  delete( _vSetter );
  delete( _nSetter );// Eric, these are objects in the header, not pointers.
  delete( _vbuf );
  delete( _nbuf );
*/
  //delete( _partLodCheck ); Eric, not a member.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all data structures
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::clearAll( )
{
  _facets.clear();
	_transforms.clear();
  _vbuf.clear();
  _nbuf.clear();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Clear all data structures
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::init( )
{
  clearAll();
	_transforms.init();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Adds a facet
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::addFacet( SlVec3f vertices[3], const SlVec3f &normal )
{
  DbStlFacetManager::facet f( vertices, normal );
  _facets.push_back( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::addFacet()
// 
//  Adds a facet and automatically calculate normal.  Vertex order is important 
//  as normal is automatically calculated as V21XV31. 
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::addFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3  )
{
  SlVec3f v1( vertex2 - vertex1 );
  SlVec3f v2( vertex3 - vertex1 );
  SlVec3f normal( v1.cross(v2) );
  normal.normalize();

  DbStlFacetManager::facet f( vertex1, vertex2, vertex3, normal );
  _facets.push_back( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Adds a facet
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::addFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal )
{
  DbStlFacetManager::facet f( vertex1, vertex2, vertex3, normal );
  _facets.push_back( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the facet information to an STL file.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::storeData ( const std::string &filename, const StlFileMode &mode )
{
  Facets::iterator i;
  SlVec3f vertices[3], normal;

  if ( mode == STL_ASCII_FILE_MODE )
  {
    DbStlAsciiOutputFile asciiOut ( filename );

    // check if output file was indeed opened
    if ( asciiOut.is_open() )
    {
      for( i = _facets.begin(); i != _facets.end(); i++ )
      {
        i->getVertices( vertices );
        i->getNormal( normal );
        asciiOut.writeFacet( vertices[0], vertices[1], vertices[2], normal );
      }
      /*DEBUG*/std::cout << "_tnv = " << _tnv << ", _tns = " << _tns << ", num facets = " << _facets.size() << std::endl;
      asciiOut.close();
      return( true );
    }
    else
      return( false );
  }
  else if ( mode == STL_BINARY_FILE_MODE )
  {
    DbStlBinaryOutputFile binOut ( filename );

        // check if output file was indeed opened
    if ( binOut.is_open() )
    {
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
  else
    return( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fetch vertices for all sets in "shape" at once (optimization for speed) and
//  place that data into _vbuf using DbStlVertexSetter as our functional
//  IQueryShapeVerticesVec3f interface to the data source.  Once data points
//  are fetched into _vbuf, they are converted and placed into _facets.
//
//  Only supports vertex set type of CadKit::TRI_STRIP_SET at this time...
//  all other vertex set types are ignored.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::fetchVerticesPerShape( IUnknown *caller, ShapeHandle shape )
{
//  SL_PRINT5 ( "In DbStlFacetManager::fetchVerticesPerShape{}, this = %X, caller = %X, shape = %d\n", this, caller, shape );
  SL_ASSERT ( caller );
  // Get the interface we need from the caller.
  SlQueryPtr<IQueryShapeVerticesVec3f> query ( caller );
  if ( query.isNull() )
    return ERROR ( "Failed to obtain needed interface from caller.", NO_INTERFACE );

  // Get the primitive type.
  VertexSetType type;
  if ( false == query->getVertexSetType ( shape, type ) )
    return ERROR ( "Failed to obtain primitive type.", FAILED );

  // Should be true.
  SL_ASSERT ( CadKit::UNKNOWN != type );

  // Get the vertices if they are of type CadKit::TRI_STRIP_SET
  // TODO add support for other set types
  switch ( type )
  {
  case CadKit::TRI_STRIP_SET:
    {
    /*DEBUG*/stl_out << "CadKit::TRI_STRIP_SET\n" << std::endl;
      _vbuf.clear(); // clear out previous set if it exists
      if ( query->getVertices ( shape, this->_vSetter ) )
      {
        int numVertices( _vbuf.getData().size() ); // number of points in all sets
        int numSets( _vbuf.getIndices().size() ); // number of sets
/*DEBUG*/_tnv+=numVertices;
/*DEBUG*/_tns+=numSets;
        if ( !_transforms.empty() ) // if there are transforms on the stack
        {
          SlVec3f vIn, vOut; // need nonmutable copy of vector for transform op

          // Transform all vertices w/ current transformations
          for (int i=0; i<numVertices; i++)
          {
            vIn.setValue( _vbuf.getData()[i] );

            _transforms.applyTransforms( vIn, vOut );
            _vbuf.getData()[i].setValue( vOut );
          }
        }

        for (int s=0; s<numSets; s++) // for each set
        {
          int offset = _vbuf.getIndices()[s]; // starting index for this set
          
          // number of vertices in this set is the set start index less the next start index
          // UNLESS this is the last set, in which case we subtract from total number
          // of vertices
          int numVerticesInSet = ( (s < (numSets-1)) ? _vbuf.getIndices()[s+1] : numVertices ) - offset;

          SL_ASSERT( numVerticesInSet >= 3); //must be at least 3 vertices in tristrip
          if ( numVerticesInSet >= 3 )
          {
            // Add facets based on transformed tristrip set coords
            // Need to be careful of order because of the way normals
            // are automatically calculated.
            // TODO... check first vertex normal to determine if we have the right direction for this strip
            for (int j=offset+2; j<offset+numVerticesInSet; j++)
            {
              if ( j % 2 == 0 ) //if index is even
                addFacet( _vbuf.getData()[j-2], _vbuf.getData()[j-1], _vbuf.getData()[j] );
              else // index is odd
                addFacet( _vbuf.getData()[j-2], _vbuf.getData()[j], _vbuf.getData()[j-1] );
            }
          }
        } // for (int s=0....
        // It worked.
        return true;
      } // if ( query->...
    } // case CadKit::TRI_STRIP_SET
    
  case CadKit::LINE_STRIP_SET:
    /*DEBUG*/stl_out << "CadKit::LINE_STRIP_SET\n" << std::endl;
    break;

  case CadKit::POINT_SET:
    /*DEBUG*/stl_out << "CadKit::POINT_SET\n" << std::endl;
    break;

  case CadKit::POLYGON_SET:
    /*DEBUG*/stl_out << "CadKit::POLYGON_SET\n" << std::endl;
    break;

  default:
    /*DEBUG*/stl_out << "Unknown set type\n" << std::endl;
    break;
  }

  // It didn't work.
  return false; // TODO - get this working... ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), FAILED );
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TransformStack::TransformStack()
{
  // Empty.
}


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

void TransformStack::push()
{
  SL_ASSERT ( !_stack.empty() ); 
  if ( !_stack.empty() )
  {
    SlMatrix44f tm( _stack.top() );
    _stack.push( tm );
    _stack.top().multLeft( tm );
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

void TransformStack::push( const SlMatrix44f &M )
{
  if ( !_stack.empty() )
  {
    SlMatrix44f tm( _stack.top() );
    _stack.push( M );
    _stack.top().multLeft( tm );
  }
  else
    _stack.push( M );
}


bool TransformStack::applyTransforms( const SlVec3f &input, SlVec3f &output )
{
  SlMatrix44f m;
  
  if ( !_stack.empty() )
  {
    m = _stack.top();

    output[0]  = m[0]  * input[0]  + m[4]  * input[1]  + m[8]  * input[2]  + m[12];
    output[1]  = m[1]  * input[0]  + m[5]  * input[1]  + m[9]  * input[2]  + m[13];
    output[2]  = m[2]  * input[0]  + m[6]  * input[1]  + m[10] * input[2]  + m[14];

    return true;
  }
  else
    return false;
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
DbStlFacetManager::facet::facet( )
//????: _vertices[0]( SL_VEC3_ZERO ), _vertices[1]( SL_VEC3_ZERO ), _vertices[2]( SL_VEC3_ZERO ), _normal( SL_VEC3_ZERO )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::facet
//
//  constructor
//
///////////////////////////////////////////////////////////////////////////////
DbStlFacetManager::facet::facet( const SlVec3f vertices[3], const SlVec3f &normal )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
  _normal.setValue( normal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::facet
//
//  constructor
//
///////////////////////////////////////////////////////////////////////////////
DbStlFacetManager::facet::facet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal )
{
  _vertices[0].setValue( vertex1 );
  _vertices[1].setValue( vertex2 );
  _vertices[2].setValue( vertex3 );
  _normal.setValue( normal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setValue
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////
void inline DbStlFacetManager::facet::setValue( const SlVec3f vertices[3], const SlVec3f &normal )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
  _normal.setValue( normal );
}

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setVertices
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////
void inline DbStlFacetManager::facet::setVertices( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 )
{
  _vertices[0].setValue( vertex1 );
  _vertices[1].setValue( vertex2 );
  _vertices[2].setValue( vertex3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setValue
//
//  Set the value of the normal
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::facet::setNormal( const SlVec3f &normal )
{
  _normal.setValue( normal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setValue
//
//  Set the value of the vertices
//
///////////////////////////////////////////////////////////////////////////////
void inline DbStlFacetManager::facet::setValue( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal )
{
  _vertices[0].setValue( vertex1 );
  _vertices[1].setValue( vertex2 );
  _vertices[2].setValue( vertex3 );
  _normal.setValue( normal );
}
    


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::setVertices
//
//  Set the value of the vertices
//
///////////////////////////////////////////////////////////////////////////////

void inline DbStlFacetManager::facet::setVertices( const SlVec3f vertices[3] )
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

void inline DbStlFacetManager::facet::getNormal( SlVec3f &normal )
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

void inline DbStlFacetManager::facet::getVertices( SlVec3f vertices[3] )
{
  vertices[0].setValue( _vertices[0] );
  vertices[1].setValue( _vertices[1] );
  vertices[2].setValue( _vertices[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::facet::getVertices
//
//  Gets the vertices
//
///////////////////////////////////////////////////////////////////////////////
void inline DbStlFacetManager::facet::getVertices( SlVec3f &vertex1, SlVec3f &vertex2, SlVec3f &vertex3 )
{
  vertex1.setValue( _vertices[0] );
  vertex2.setValue( _vertices[1] );
  vertex3.setValue( _vertices[2] );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.  Note that this operates on the vertex buffer _vbuf owned
//  by parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlVertexSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  if ( _vertices ) // make sure we have a vertex buffer
  {
    // If we're out of range then resize.
    if ( index >= _vertices->getData().size() )
      _vertices->getData().resize( index + 1 );
//        return false;
/*      if ( false == (_vertices->getData().resize( index + 1 )) )
        return false;*/ // this evidently isn't returning bool???

    // Set the vertex.
    (_vertices->getData())[index].setValue( vec[0], vec[1], vec[2] );

    // It worked.
    return true;
  }
  else
    return false;

}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.  Note that this operates on the vertex buffer _vbuf owned
//  by parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlVertexSetter::setSize ( const unsigned int &size )
{
  if ( _vertices ) // make sure we have a vertex buffer
  {
    SL_ASSERT ( size > 0 );
    _vertices->getData().resize( size );
    return size == _vertices->getData().size();
  }
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the index to the primitive set.  Note that this operates on the vertex 
//  buffer _vbuf owned by parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlVertexSetter::setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length )
{
  if ( _vertices ) // make sure we have a vertex buffer
  {
    // If we're out of range then resize.
    if ( index >= _vertices->getIndices().size() )
      _vertices->getIndices().resize( index + 1 );
/*      if ( false == _vertices->getIndices().resize( index + 1 ) )
        return false;*/ // this evidently isn't returning bool??

    // Set the index to start of primitive set.
    (_vertices->getIndices())[index] = start;

    // It worked.
    return true;
  }
  else
    return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of primitive sets.  Note that this operates on the vertex 
//  buffer _vbuf owned by parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlVertexSetter::setNumPrimitives ( const unsigned int &num )
{
  if ( _vertices ) // make sure we have a vertex buffer
  {
    SL_ASSERT ( num > 0 );
    _vertices->getIndices().resize( num );
    return num == _vertices->getIndices().size();
  }
  else
    return false;
}
