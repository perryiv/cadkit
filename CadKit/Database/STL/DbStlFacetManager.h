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
#include <math.h>
//????????????
//#include "DbStlApi.h"
#include <stdio.h>
#include <iostream>
#include <vector>
//#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
//#include "Standard/SlVec4.h"
#include "Standard/SlStack.h"
#include "Standard/SlMatrix4.h"
#include "Standard/SlMatrix4IO.h"


namespace CadKit
{


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlFacetManager.
//
///////////////////////////////////////////////////////////////////////////////

class /*DB_STL_API*/ DbStlFacetManager
{
public:


  enum StlFileMode
  {
    STL_ASCII_FILE_MODE,
    STL_BINARY_FILE_MODE
  };

/* 
  class triangle
  {
  public:
    SlVec3<float> vertices[3];
    triangle( SlVec3<float> v[3] )
 */ 
  
  
  class facet
  {
  public:
    facet( ) { }
    facet( const SlVec3<float> vertices[3], const SlVec3<float> &normal );
    void setNormal( const SlVec3<float> &normal );
    void setVertices( const SlVec3<float> vertices[3] );
    void setValue( const SlVec3<float> vertices[3], const SlVec3<float> &normal );
    void getNormal( SlVec3<float> &normal );
    void getVertices( SlVec3<float> vertices[3] );
  
  protected:
		SlVec3<float> _vertices[3];
		SlVec3<float> _normal;
  };


  // For convenience
  typedef std::vector<facet> Facets;

  
  
  /////////////////////////////////////////////////////////////////////////////
	// TransMatStack
	//
	// This is a stack which stores the "current" transformation matrix
	// for the points.  This class is basically the same as SlStack with
	// one exception - the "push" function is modified to multiply the 
	// new matrix by the one on top of the stack.  That way the top matrix
	// always contains the cumulative transformations for the points we are
	// operating on.
  /////////////////////////////////////////////////////////////////////////////

	class TransMatStack : public SlStack<SlMatrix4f>
	{
	public:
		
		//  Override the default push function so that the new matrix is multiplied
		//  by the matrix on top of the stack.
		void push( );
		void push ( const SlMatrix4f &val );
  };


//	DbStlFacetManager ( ) : _facets ( new Facets ), _transmat ( new TransMatStack ){}
//  ~DbStlFacetManager ( ) { delete _facets; delete _transmat; }
  DbStlFacetManager ( ) { }
  ~DbStlFacetManager ( ) { }

  // Store the data.
  bool storeData ( const char *filename, const StlFileMode &mode );

  void addFacet ( SlVec3<float> vertices[3], SlVec3<float> &normal );
  void clearFacets ( ) { if ( !_facets.empty() ) _facets.erase( _facets.begin(), _facets.end() ); };
  long getNumFacets ( ) { return _facets.size(); }

protected:

	Facets _facets;
	TransMatStack _transmat;
};





}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_FACET_MANAGER_H_
