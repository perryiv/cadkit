
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

// when i put this after the SlPartitionedVector header, I got an internal compiler error
// C1001 in the IEntityNotify.h header file... so moved it up here
#include "Standard/SlQueryPtr.h" 

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
# include <list>
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
// This is a stack of 4x4 transforms.  A call to the collapse function will
// return a single matrix which is the multiplication product from bottom to
// top.  For instance, we have a stack like this:
//      top->[C][B][A]<-bottom
// collapse() would return the product [C]*[B]*[A].
//
/////////////////////////////////////////////////////////////////////////////

class TransformStack
//  class TransformStack : public std::stack<SlMatrix44f>
{
public:
	
  TransformStack( );

  void init( ) { _stack.clear(); pushIdentity(); } // clear and push identity
  void push( );
	void push ( const SlMatrix44f &val );
  void pop( ) { if ( !_stack.empty() ) { _stack.pop(); _recalcProduct(); } } // pop the stack
  void pushIdentity( ) { SlMatrix44f id( true ); _stack.push( id ); } // push identity - don't need to recalc
  void clear( ) { _stack.clear(); _product.identity(); }
  bool empty( ) const { return _stack.empty(); }
  bool applyTransforms( const SlVec3f &input, SlVec3f &output ); // apply top transforms to input vector and return in output

  // "output" is set to the stack product, identity if stack is empty.
  // Return true if stack is not empty, false if empty ("output" will be valid either way)
  // Example:
  //      top->[C][B][A]<-bottom
  // getProduct() would return the product [A]*[B]*[C] through "output"
  bool getProduct( SlMatrix44f &output ) { output = _product; return !_stack.empty(); }

  // collapses the stack into one matrix which is the product of all the matrices on the stack
  // see description of getProduct() for multiplicatoin sequence 
  void collapse( ) { SlMatrix44f temp(_product); _stack.clear(); _stack.push( temp ); }

  SlStack<SlMatrix44f> &getStack( ) { return _stack; } // get the stack
  SlMatrix44f &top( ) { return _stack.top(); } // get the top of the stack
protected:
  SlStack<SlMatrix44f> _stack;
  SlMatrix44f _product; // cached copy of matrix stack product... for efficiency.  Set to identity if stack empty
  void _recalcProduct(); // recalculate product and store in _product
};

/////////////////////////////////////////////////////////////////////////////
// DbStlFacetManager member class "facet"
// This class organizes geometry data into triangular facets w/ normals
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// class "facet"
//
// This class organizes geometry data into triangular facets w/ normals
// Pointer is provided to transformation stack if needed (may be NULL)
//
/////////////////////////////////////////////////////////////////////////////

class facet
{
public:
  facet( );
  facet( const SlVec3f vertices[3], const SlVec3f &normal, TransformStack *tsp = NULL );
  facet( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal, TransformStack *tsp = NULL );
  void setTransformStack( TransformStack *tsp ) { _tsp = tsp; } // may be NULL
  void setNormal( const SlVec3f &normal );
  void setVertices( const SlVec3f vertices[3] );
  void setVertices( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3 );
  void setValue( const SlVec3f vertices[3], const SlVec3f &normal );
  void setValue( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal );
  void getNormal( SlVec3f &normal );
  void getVertices( SlVec3f &vertex1, SlVec3f &vertex2, SlVec3f &vertex3 );
  void getVertices( SlVec3f vertices[3] );
  TransformStack *getTransformStack( ) { return _tsp; }

protected:
	SlVec3f _vertices[3];
	SlVec3f _normal;
  TransformStack *_tsp;
};


// For convenience
typedef std::list<facet> Facets;
typedef Facets::iterator FmgrIndex;

///////////////////////////////////////////////////////////////////////////////
//  class "DbStlVertexSetter"
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
//  to data source IQueryShapeNormalsVec3f... it has no data members of its
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

/*class DbOsgNormalSetter : public IQueryShapeNormalsVec3f::NormalSetter
{
public:
  
  typedef osg::Geometry::AttributeBinding Binding;

  DbOsgNormalSetter ( const VertexBinding &binding ) : _binding ( CadKit::getBinding ( binding ) ), _normals ( new osg::Vec3Array ){}

  osg::Vec3Array *      getNormals() { return _normals.get(); }
  Binding               getBinding() { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  Binding _binding;
  osg::ref_ptr<osg::Vec3Array> _normals;
};
*/
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
	// DbStlFacetManager member functions
  /////////////////////////////////////////////////////////////////////////////

  // Store the data.
  bool storeData ( const std::string &filename, const StlFileMode &mode );

  // Clear all data structures
  void clearAll( );

  // Clear all data structures
  void init( );

  // Fetch all vertices for a given shape and use to populate _facets list
  bool fetchVerticesPerShape( SlQueryPtr<IQueryShapeVerticesVec3f> query, ShapeHandle shape );

  // Add a facet - pick your flavor
  void addFacet ( SlVec3f vertices[3], const SlVec3f &normal, TransformStack *tsp = NULL );
  void addFacet ( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, TransformStack *tsp = NULL );
  void addFacet ( const SlVec3f &vertex1, const SlVec3f &vertex2, const SlVec3f &vertex3, const SlVec3f &normal, TransformStack *tsp = NULL );

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

  // Return  current endpoint of _facets list as an opaque index
  //TODO - should rework to have return a bool true/false depending on empty() condition.
  bool getIndex( FmgrIndex &i ) { i = _facets.end(); if( _facets.empty() ) return false; else i--; return true; }

  // Increment FmgrIndex type in a safe manner and flag if not successful
  bool incrementIndex( FmgrIndex &index ) { if ( _facets.empty() || index == _facets.end() ) return false; else index++; return ( index == _facets.end() );  }

  // Process instances
  bool processInstance( const FmgrIndex &start, const FmgrIndex &end, const SlMatrix44f &matrix );

  // Apply transforms
  void applyTransforms( );

  int indexPos( const FmgrIndex &index ) 
  { 
    if ( _facets.empty() || index == _facets.end() ) 
      return -1; 
    else 
    { 
      int i(0); 
      FmgrIndex fi( _facets.begin() ); 
      while ( (fi != index) && (fi != _facets.end()) )
      {
        fi++;
        i++;
      }
      return i; 
    }
  }


protected:
  Facets _facets;
	TransformStack _transforms;
/*DEBUG*/int _tnv, _tns;

  Vertices _vbuf;
  Normals _nbuf;
  // interfaces and buffers for fetching vertices and normals from data source
  DbStlVertexSetter _vSetter; 
  DbStlNormalSetter _nSetter; // Eric, does this need to be a member? It needs a default constructor if that is the case. Otherwise, make it a pointer and pass in the argument(s) to the constructor in the constructor's initializer list (that I commented out).
//  Binding _binding; //TODO

  bool _notifyError ( const std::string &message, const unsigned long &id, IUnknown *caller );
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_FACET_MANAGER_H_
