
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

#include "Interfaces/IMessageNotify.h"

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

void DbStlFacetManager::clearAll( )
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

void DbStlFacetManager::init( )
{
  clearAll();
	_transforms.init();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Adds a facet
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::addFacet( SlVec3f vertices[3], const SlVec3f &normal, TransformStack *tsp /* = NULL */ )
{
  facet f( vertices, normal, tsp );
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

void DbStlFacetManager::addFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, TransformStack *tsp /* = NULL */ )
{
  SlVec3f v1( vertex2 - vertex1 );
  SlVec3f v2( vertex3 - vertex1 );
  SlVec3f normal( v1.cross(v2) );
  normal.normalize();

  facet f( vertex1, vertex2, vertex3, normal, tsp );
  _facets.push_back( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Adds a facet
//
///////////////////////////////////////////////////////////////////////////////

void DbStlFacetManager::addFacet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal, TransformStack *tsp /* = NULL */ )
{
  facet f( vertex1, vertex2, vertex3, normal, tsp );
  _facets.push_back( f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the facet information to an STL file.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::storeData ( const std::string &filename, const StlFileMode &mode )
{
//  Facets::iterator i;
  FmgrIndex i;
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

bool DbStlFacetManager::fetchVerticesPerShape( SlQueryPtr<IQueryShapeVerticesVec3f> query, ShapeHandle shape )
{
//  SL_PRINT5 ( "In DbStlFacetManager::fetchVerticesPerShape{}, this = %X, caller = %X, shape = %d\n", this, caller, shape );
  SL_ASSERT ( query.isValid() );

      _vbuf.clear(); // clear out previous set if it exists
      if ( query->getVertices ( shape, this->_vSetter ) )
      {
        int numVertices( _vbuf.getData().size() ); // number of points in all sets
        int numSets( _vbuf.getIndices().size() ); // number of sets
/*DEBUG*/_tnv+=numVertices;
/*DEBUG*/_tns+=numSets;
        if ( !_transforms.empty() ) // if there are transforms on the stack
        {

//TODO - hook transforms to first facet in group
        /*          SlVec3f vIn, vOut; // need nonmutable copy of vector for transform op

          // Transform all vertices w/ current transformations
          for (int i=0; i<numVertices; i++)
          {
            vIn.setValue( _vbuf.getData()[i] );

            _transforms.applyTransforms( vIn, vOut );
            _vbuf.getData()[i].setValue( vOut );
          }*/
        }

        std::list<facet>::iterator mark( _facets.end() );
        if ( !_facets.empty() ) // grab the end of the list
          mark--;

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
        
        TransformStack *ts = new TransformStack( _transforms ); // make a copy of the current stack
        SlMatrix44f matrix( ts->top() ); // cache current transforms
        ts->pop(); // pop the top and collapse the rest down to one matrix
        ts->collapse();
        ts->push( matrix ); // push the current back

        if ( _facets.empty() )
          return false;
        else if (mark == _facets.end()) // this is first time through
          mark = _facets.begin();
        else // we are somewhere in the middle of the list
          mark++;
 
        mark->setTransformStack( ts ); // point start facet to new transform stack
        
        // It worked.
        return true;
      }
  // It didn't work.
  return false; // TODO - get this working... ERROR ( FORMAT ( "Failed to get vertices for shape %X.", shape ), FAILED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  To handle instance, we are going to create a copy of the _facets list
//  between the indicies "start" and "end", replace the top of "start"'s 
//  transform with the instance's transform, and append the whole works onto
//  the end of the _facets list.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::processInstance( const FmgrIndex &start, const FmgrIndex &end, const SlMatrix44f &matrix )
{
  SL_ASSERT( !_facets.empty() );
  SL_ASSERT( start != _facets.end() );
  SL_ASSERT( start <= end );

  // TODO - check to make sure that we get a valid transform stack back
  _facets.insert( _facets.end(), start, end );
  TransformStack *ts = new TransformStack( *(start->getTransformStack()) );
  ts->pop(); // get rid of original part/assembly transforms
  ts->collapse(); // collapse the rest down to one matrix
  ts->push( matrix ); // push the instance transforms on top
  start->setTransformStack( ts ); // point start facet to new transform stack

  return true;
}

void DbStlFacetManager::applyTransforms( )
{
  FmgrIndex i( _facets.begin() ); // grab the head of the list
  TransformStack *ts = i->getTransformStack();

  SL_ASSERT( ts != NULL ); // first facet MUST have a transform or we are hosed

  if ( ts != NULL )
  {
    SlMatrix44f matrix(true); 
    SlVec3f v1in, v2in, v3in, v1out, v2out, v3out, n, vIn, vOut; // need nonmutable copy of vector for transform op


    std::list<facet>::iterator i( _facets.begin() );

    // Transform all vertices w/ current transformations
    while ( i != _facets.end() )
    {
      if ( ( ts = i->getTransformStack() )!= NULL )
        ts = i->getTransformStack();

      i->getVertices( v1in, v2in, v3in );
//      i->getNormal( n );

      ts->applyTransforms( v1in, v1out );
      ts->applyTransforms( v2in, v2out );
      ts->applyTransforms( v3in, v3out );
      i->setVertices( v1out, v2out, v3out );
      i++;
    }
  }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TransformStack::TransformStack() : _product(true) // init _product w/ identity
{
  this->init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the existing top matrix and recalculate stack product.  No effect if
//  stack is empty.
//
///////////////////////////////////////////////////////////////////////////////

void TransformStack::push()
{
  SL_ASSERT ( !_stack.empty() ); 
  if ( !_stack.empty() )
  {
    SlMatrix44f tm( _stack.top() );
    _stack.push( tm );
    _recalcProduct();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the matrix and recalculate stack product.  
//
///////////////////////////////////////////////////////////////////////////////

void TransformStack::push( const SlMatrix44f &M )
{
    _stack.push( M );
    _recalcProduct();
}


///////////////////////////////////////////////////////////////////////////////
//
// recalculate product and store in _product  
// this gets a little ugly because we have to access the internal deque of the
// stack... probably would be cleaner to implement stack as a list or vector,
// but we will save that for another day.
//
///////////////////////////////////////////////////////////////////////////////

void TransformStack::_recalcProduct()
{
  std::deque<SlMatrix44f>::iterator i;

  if ( !_stack.empty() )
  {
    _product.identity();
    for ( i=_stack.deque().begin(); i<_stack.deque().end(); i++ ) // iterate through the stack
      _product.multRight( *i );
  }
}

  
  
///////////////////////////////////////////////////////////////////////////////
//
// apply transforms to points.. uses _product which is identity if stack is 
// empty, so still ok... just no effect 
//
///////////////////////////////////////////////////////////////////////////////

bool TransformStack::applyTransforms( const SlVec3f &input, SlVec3f &output )
{
  output[0]  = _product[0]  * input[0]  + _product[4]  * input[1]  + _product[8]  * input[2]  + _product[12];
  output[1]  = _product[1]  * input[0]  + _product[5]  * input[1]  + _product[9]  * input[2]  + _product[13];
  output[2]  = _product[2]  * input[0]  + _product[6]  * input[1]  + _product[10] * input[2]  + _product[14];

  // TODO - doublecheck this return
  return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// this stuff used to be in header file... but got linker errors
   
///////////////////////////////////////////////////////////////////////////////
//
//  facet::facet
//
//  constructor
//
///////////////////////////////////////////////////////////////////////////////
facet::facet( ) : _tsp( NULL )
//????: _vertices[0]( SL_VEC3_ZERO ), _vertices[1]( SL_VEC3_ZERO ), _vertices[2]( SL_VEC3_ZERO ), _normal( SL_VEC3_ZERO )
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  facet::facet
//
//  constructor
//
///////////////////////////////////////////////////////////////////////////////
facet::facet( const SlVec3f vertices[3], const SlVec3f &normal, TransformStack *tsp /* = NULL */ )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
  _normal.setValue( normal );
  _tsp = tsp; // may be NULL
}


///////////////////////////////////////////////////////////////////////////////
//
//  facet::facet
//
//  constructor
//
///////////////////////////////////////////////////////////////////////////////
facet::facet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal, TransformStack *tsp /* = NULL */ )
{
  _vertices[0].setValue( vertex1 );
  _vertices[1].setValue( vertex2 );
  _vertices[2].setValue( vertex3 );
  _normal.setValue( normal );
  _tsp = tsp; // may be NULL
}


///////////////////////////////////////////////////////////////////////////////
//
//  facet::setValue
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////
void facet::setValue( const SlVec3f vertices[3], const SlVec3f &normal )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
  _normal.setValue( normal );
}

///////////////////////////////////////////////////////////////////////////////
//
//  facet::setVertices
//
//  Set the value
//
///////////////////////////////////////////////////////////////////////////////
void facet::setVertices( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 )
{
  _vertices[0].setValue( vertex1 );
  _vertices[1].setValue( vertex2 );
  _vertices[2].setValue( vertex3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  facet::setValue
//
//  Set the value of the normal
//
///////////////////////////////////////////////////////////////////////////////

void facet::setNormal( const SlVec3f &normal )
{
  _normal.setValue( normal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  facet::setValue
//
//  Set the value of the vertices
//
///////////////////////////////////////////////////////////////////////////////
void facet::setValue( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal )
{
  _vertices[0].setValue( vertex1 );
  _vertices[1].setValue( vertex2 );
  _vertices[2].setValue( vertex3 );
  _normal.setValue( normal );
}
    

///////////////////////////////////////////////////////////////////////////////
//
//  facet::setVertices
//
//  Set the value of the vertices
//
///////////////////////////////////////////////////////////////////////////////

void facet::setVertices( const SlVec3f vertices[3] )
{
  _vertices[0].setValue( vertices[0] );
  _vertices[1].setValue( vertices[1] );
  _vertices[2].setValue( vertices[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  facet::getNormal
//
//  Gets the normal
//
///////////////////////////////////////////////////////////////////////////////

void facet::getNormal( SlVec3f &normal )
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

void facet::getVertices( SlVec3f vertices[3] )
{
  vertices[0].setValue( _vertices[0] );
  vertices[1].setValue( _vertices[1] );
  vertices[2].setValue( _vertices[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  facet::getVertices
//
//  Gets the vertices
//
///////////////////////////////////////////////////////////////////////////////
void  facet::getVertices( SlVec3f &vertex1, SlVec3f &vertex2, SlVec3f &vertex3 )
{
  vertex1.setValue( _vertices[0] );
  vertex2.setValue( _vertices[1] );
  vertex3.setValue( _vertices[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Similar to DbBaseObject::_notifyError(), but since this class doesn't 
//  inherit from DbBaseObject we have to do this.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlFacetManager::_notifyError ( const std::string &message, const unsigned long &id, IUnknown *controller )
{
  // Try to get the interface.
  SlQueryPtr<IMessageNotify> notify ( controller );
  if ( notify.isNull() )
    return false;

  // Call the message notification function.
  return notify->messageNotify ( message, id, CadKit::MESSAGE_ERROR );
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
