
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

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_FACET_MANAGER_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_FACET_MANAGER_H_

#include "DbStlApi.h"

#include "Standard/SlVec3.h"
#include "Standard/SlStack.h"
#include "Standard/SlMatrix44.h"
#include "Standard/SlMatrix44IO.h"
#include "Standard/SlPartitionedVector.h"

#include "Interfaces/IEntityNotify.h"
#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdio.h>
# include <iostream>
# include <vector>
# include <set>
# include <stack>
#endif


namespace CadKit
{


// for convenience/readability
typedef SlPartitionedVector<unsigned int, SlVec3f> Vertices;
typedef SlPartitionedVector<unsigned int, SlVec3f> Normals;

//  typedef SlStack<SlMatrix44f> DbStlStackM44f
  

/////////////////////////////////////////////////////////////////////////////
// class "TransformStack"
//
// This is a stack which stores the "current" transformation matrix
// for the points.  This class is basically the same as SlStack with
// one exception - the "push" function is modified to multiply the 
// new matrix by the one on top of the stack.  That way the top matrix
// always contains the cumulative transformations for the points we are
// operating on.
/////////////////////////////////////////////////////////////////////////////

class TransformStack
//  class TransformStack : public std::stack<SlMatrix44f>
{
public:
	
  TransformStack( );

  //  Override the default push function so that the new matrix is multiplied
	//  by the matrix on top of the stack.
  void init( ) { _stack.clear(); pushIdentity(); } // clear and push identity
  void push( );
  void pop( ) { if ( !_stack.empty() ) _stack.pop(); } // pop the stack
  void pushIdentity( ) { SlMatrix44f id; id.identity(); _stack.push( id ); } // push identity
  void clear( ) { _stack.clear(); }
  bool empty( ) const { return _stack.empty(); }
	void push ( const SlMatrix44f &val );
  bool applyTransforms( const SlVec3f &input, SlVec3f &output ); // apply top transforms to input vector and return in output
  SlStack<SlMatrix44f> &getStack( ) { return _stack; } // get the stack
  SlMatrix44f &top( ) { return _stack.top(); } // get the stack
protected:
  SlStack<SlMatrix44f> _stack;
};


///////////////////////////////////////////////////////////////////////////////
//  DbStlFacetManager member class "DbStlVertexSetter"
//
//  We are using this class exclusively as an interface
//  to data source IQueryShapeVerticesVec3f... it has no data members of its
//  own.  It operates on vertex buffer _vbuf which is owned by parent class
//  DbStlFacetManager.  It is responsible solely for fetching vertex data
//  from data source and placing that data into _vbuf.  Any other manipulation
//  of _vbuf is the sole responsibility of the parent class DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

class DbStlVertexSetter : public IQueryShapeVerticesVec3f::VertexSetter
{
public:

  DbStlVertexSetter ( ) : _vertices( NULL ) { }
//  DbStlVertexSetter ( Vertices* vertices ) : _vertices( vertices ) { }

  void setBuffer ( Vertices *vertices ) { _vertices = vertices; }
  bool setData ( const unsigned int &index, const SlVec3f &vec );
  bool setSize ( const unsigned int &size );

  bool setNumPrimitives  ( const unsigned int &num );
  bool setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length );

protected:
  Vertices *_vertices;
};


///////////////////////////////////////////////////////////////////////////////
//  DbStlFacetManager member class "DbStlNormalSetter"
//  
//  We are using this class exclusively as an interface
//  to data source IQueryShapeVerticesVec3f... it has no data members of its
//  own.  It operates on normal buffer _nbuf which is owned by parent class
//  DbStlFacetManager.  It is responsible solely for fetching normal data
//  from data source and placing that data into _nbuf.  Any other manipulation
//  of _nbuf is the sole responsibility of the parent class DbStlFacetManager.
///////////////////////////////////////////////////////////////////////////////

class DbStlNormalSetter : public IQueryShapeNormalsVec3f::NormalSetter
{
public:

//  DbStlNormalSetter ( const VertexBinding &binding )/* : _binding ( CadKit::getBinding ( binding ) )*/ { /* TODO */ }
  DbStlNormalSetter ( ) : _normals( NULL ) { }
//  DbStlNormalSetter ( Normals *normals ) : _normals( normals ) { }

  void setBuffer ( Normals *normals ) { _normals = normals; }
  bool setData ( const unsigned int &index, const SlVec3f &vec ) { SL_ASSERT ( 0 ); return false; /* TODO */ }
  bool setSize ( const unsigned int &size ) { SL_ASSERT ( 0 ); return false; /* TODO */ }

  bool setNumPrimitives  ( const unsigned int &num ) { SL_ASSERT ( 0 ); return false; /* TODO */ }
  bool setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length ) { SL_ASSERT ( 0 ); return false; /* TODO */ }

protected:
  Normals *_normals;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

class DB_STL_API DbStlFacetManager
{
public:


  enum StlFileMode
  {
    STL_ASCII_FILE_MODE,
    STL_BINARY_FILE_MODE
  };

  DbStlFacetManager ( );
  ~DbStlFacetManager ( );


  /////////////////////////////////////////////////////////////////////////////
	// DbStlFacetManager member class "facet"
	// This class organizes geometry data into triangular facets w/ normals
  /////////////////////////////////////////////////////////////////////////////

  class facet
  {
  public:
    facet( );
    facet( const SlVec3f vertices[3], const SlVec3f &normal );
    facet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal );
    void setNormal( const SlVec3f &normal );
    void setVertices( const SlVec3f vertices[3] );
    void setVertices( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 );
    void setValue( const SlVec3f vertices[3], const SlVec3f &normal );
    void setValue( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal );
    void getNormal( SlVec3f &normal );
    void getVertices( SlVec3f &vertex1, SlVec3f &vertex2, SlVec3f &vertex3 );
    void getVertices( SlVec3f vertices[3] );
  
  protected:
		SlVec3f _vertices[3];
		SlVec3f _normal;
  };


  // For convenience
  typedef std::list<facet> Facets;

  /////////////////////////////////////////////////////////////////////////////
	// DbStlFacetManager member functions
  /////////////////////////////////////////////////////////////////////////////

  // Store the data.
  bool storeData ( const char *filename, const StlFileMode &mode );

  // Clear all data structures
  void clearAll( );

  // Clear all data structures
  void init( );

  // Fetch all vertices for a given shape and use to populate _facets list
  bool fetchVerticesPerShape( IUnknown *caller, ShapeHandle shape );

  // Add a facet - pick your flavor
  void addFacet ( SlVec3f vertices[3], const SlVec3f &normal );
  void addFacet ( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 );
  void addFacet ( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal );

  // Clear facets
  void clearFacets ( ) { _facets.clear(); };

  // Get number of facets
  int getNumFacets ( ) { return _facets.size(); }

  // Push a transform onto the stack.
  void pushTransform ( const SlMatrix44f &matrix ) { _transforms.push( matrix ); }

  // Push a transform onto the stack.
  void pushTransformIdentity ( ) { _transforms.pushIdentity( ); }

  // Pop a transform onto the stack.
  void popTransform ( ) { _transforms.pop(); }





protected:



  Facets _facets;
	TransformStack _transforms;

  Vertices _vbuf;
  Normals _nbuf;
  // interfaces and buffers for fetching vertices and normals from data source
  DbStlVertexSetter _vSetter; // Eric, does this need to be a member?
  DbStlNormalSetter _nSetter; // Eric, does this need to be a member? It needs a default constructor if that is the case. Otherwise, make it a pointer and pass in the argument(s) to the constructor in the constructor's initializer list (that I commented out).
//  Binding _binding; //TODO
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_FACET_MANAGER_H_
