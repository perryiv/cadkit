
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
#include "DbStlFileIO.h"
#include "DbStlFacetManager.h"

using namespace CadKit;

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager::DbStlFacetManager() constructor
//
///////////////////////////////////////////////////////////////////////////////
DbStlFacetManager::DbStlFacetManager( )
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
  // empty
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

void DbStlFacetManager::addFacet( SlVec3f vertices[3], SlVec3f &normal )
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

  DbStlFacetManager::facet f( vertices, normal );
  _facets.push_back( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Adds a facet
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::addFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal )
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
  SlVec3f vertices[3], normal;

  if ( mode == STL_ASCII_FILE_MODE )
  {
    DbStlAsciiOutputFile asciiOut( filename );

    // check if output file was indeed opened
    if ( asciiOut->is_open() )
    {
      for( i = _facets.begin(); i != _facets.end(); i++ )
      {
        i->getVertices( vertices );
        i->getNormal( normal );
        asciiOut.writeFacet( vertices[0], vertices[1], vertices[2], normal );
      }
      asciiOut.close();
      return( true );
    }
    else
      return( false );
  }
  else if ( mode == STL_BINARY_FILE_MODE )
  {
    DbStlBinaryOutputFile binOut( filename );

        // check if output file was indeed opened
    if ( binOut->is_open() )
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
  SL_PRINT5 ( "In DbStlFacetManager::fetchVerticesPerShape{}, this = %X, caller = %X, shape = %d\n", this, caller, shape );
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
      _vbuf.clear(); // clear out previous set if it exists
      if ( query->getVertices ( shape, this->_Vsetter ) )
      {
        int numVertices( _vbuf.getData().size() ); // number of points in all sets
        int numSets( _vbuf.getIndicies().size() ); // number of sets

        if ( !_transforms.empty() ) // if there are transforms on the stack
        {
          SlVec3f v; // need nonmutable copy of vector for transform op

          // Transform all vertices w/ current transformations
          for (int i=0; i<numVertices; i++)
          {
            v.setValue( _vbuf.getData()[i] );
            _transforms.multByUpper3x3( v, _vbuf.getData()[i] );
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
            for (int j=offset+2; j<numVerticesInSet; j++)
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
    break;

  case CadKit::POINT_SET:
    break;

  case CadKit::POLYGON_SET:
    break;

  default:
    break;
  }

  // It didn't work.
  return ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), FAILED );
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DbStlFacetManager::TransformStack() : std::stack<SlMatrix44f>::stack()
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

void DbStlFacetManager::TransformStack::push()
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

void DbStlFacetManager::TransformStack::push( const SlMatrix44f &M )
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
: _vertices[0]( SL_VEC3_ZERO ), _vertices[1]( SL_VEC3_ZERO ), _vertices[2]( SL_VEC3_ZERO ), _normal( SL_VEC3_ZERO )
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

bool DbStlFacetManager::DbStlVertexSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then resize.
  if ( index >= DbStlFacetManager::_vbuf.getData().size() )
    if ( false == DbStlFacetManager::_vbuf.getData().resize( index + 1 ) )
      return false;

  // Set the vertex.
  (DbStlFacetManager::_vbuf.getData())[index].set( vec[0], vec[1], vec[2] );

  // It worked.
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.  Note that this operates on the vertex buffer _vbuf owned
//  by parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::DbStlVertexSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  DbStlFacetManager::_vbuf.getData().resize( size );
  return size == DbStlFacetManager::_vbuf.getData().size();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the index to the primitive set.  Note that this operates on the vertex 
//  buffer _vbuf owned by parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::DbStlVertexSetter::setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length )
{
  // If we're out of range then resize.
  if ( index >= DbStlFacetManager::_vbuf.getIndices().size() )
    if ( false == DbStlFacetManager::_vbuf.getIndices().resize( index + 1 ) )
      return false;

  // Set the index to start of primitive set.
  (DbStlFacetManager::_vbuf.getIndices())[index].set( start );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of primitive sets.  Note that this operates on the vertex 
//  buffer _vbuf owned by parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::DbStlVertexSetter::setNumPrimitives ( const unsigned int &num )
{
  SL_ASSERT ( num > 0 );
  DbStlFacetManager::_vbuf.getIndices().resize( num );
  return num == DbStlFacetManager::_vbuf.getIndices().size();
}
