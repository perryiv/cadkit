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

//???????????
// I need this up here or else I get this VC++ error (and many others):
// SlTemplateSupport.h(79) : error C2039: 'sqrtl' : is not a member of '`global namespace''
//#include <math.h>
//????????????
#include "DbStlApi.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>
#include <stack>
//#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
//#include "Standard/SlVec4.h"
#include "Standard/SlStack.h"
#include "Standard/SlMatrix44.h"
#include "Standard/SlMatrix44IO.h"
#include "Standard/SlPartitionedVector.h"

#include "Interfaces/IEntityNotify.h"
#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"



namespace CadKit
{


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

class DB_STL_API DbStlFacetManager
{
public:

  // for convenience/readability
  typedef SlPartitionedVector<unsigned int, SlVec3f> Vertices;
  typedef SlPartitionedVector<unsigned int, SlVec3f> Normals;

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
//  typedef SlStack<SlMatrix44f> DbStlStackM44f
  

  /////////////////////////////////////////////////////////////////////////////
	// DbStlFacetManager member class "TransformStack"
	//
	// This is a stack which stores the "current" transformation matrix
	// for the points.  This class is basically the same as SlStack with
	// one exception - the "push" function is modified to multiply the 
	// new matrix by the one on top of the stack.  That way the top matrix
	// always contains the cumulative transformations for the points we are
	// operating on.
  /////////////////////////////////////////////////////////////////////////////

//	class TransformStack : public SlStack<SlMatrix44f>
  class TransformStack : public std::stack<SlMatrix44f>
	{
	public:
		
//    TransformStack( ) : SlStack<SlMatrix44f>::SlStack() { } // constructor
    TransformStack( ) : std::stack<SlMatrix44f>::stack() { } // constructor

    //  Override the default push function so that the new matrix is multiplied
		//  by the matrix on top of the stack.
    void init( ) { SlMatrix44f id; id.identity(); clear(); push( id ); } // clear and push identity
    void push( );
		void push ( const SlMatrix44f &val );
  };

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





protected:

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

    DbStlVertexSetter ( ) { }

    bool setData ( const unsigned int &index, const SlVec3f &vec );
    bool setSize ( const unsigned int &size );

    bool setNumPrimitives  ( const unsigned int &num );
    bool setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length );
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

    DbStlNormalSetter ( const VertexBinding &binding )/* : _binding ( CadKit::getBinding ( binding ) )*/ { /* TODO */ }

    bool setData ( const unsigned int &index, const SlVec3f &vec ) { /* TODO */ }
    bool setSize ( const unsigned int &size ) { /* TODO */ }

    bool setNumPrimitives  ( const unsigned int &num ) { /* TODO */ }
    bool setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length ) { /* TODO */ }
  };


  Facets _facets;
	TransformStack _transforms;

  // interfaces and buffers for fetching vertices and normals from data source
  DbStlVertexSetter _vSetter;
  DbStlNormalSetter _nSetter;
  Vertices _vbuf;
  Normals _nbuf;
//  Binding _binding; //TODO
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_FACET_MANAGER_H_
