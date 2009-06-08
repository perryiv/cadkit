
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A class used to wrap the Triangle Code from the Quake project at CMU. This
//  code can be found at http://www.cs.cmu.edu/~quake/triangle.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/Triangulate/TriangulateComponent.h"

// The following header file needs to have REAL defined. We are
// choosing to use 64 bit floats for the triangulation
#define REAL double
#define VOID void

// This is the header for the Quake library. It does the heavy lifting.
#include "Quake/triangle.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

//#define VERBOSE_OUTPUT

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangulateComponent, TriangulateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateComponent::TriangulateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateComponent::~TriangulateComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TriangulateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITriangulateLoop::IID:
    return static_cast < Usul::Interfaces::ITriangulateLoop*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Calls the triangulate function of the Quake based Triangulation code.
//
///////////////////////////////////////////////////////////////////////////////

void TriangulateComponent::triangulateLoop ( const Vertices &inVertices, InnerLoops& inner, Vertices &outVertices, UIntArray& indices )
{
  ::triangulateio in, mid;
  ::memset(&in, sizeof(::triangulateio), 0);
  ::memset(&mid, sizeof(::triangulateio), 0 );
  
  in.numberofpoints = inVertices.size();
  in.numberofpointattributes = 0; 
  std::vector<REAL> pointList;
  pointList.reserve( in.numberofpoints * 2 );
  
  
  for( Vertices::const_iterator iter = inVertices.begin(); iter != inVertices.end(); ++iter )
  {
    const Usul::Math::Vec2d &point = *iter;
    pointList.push_back(point[0]);
    pointList.push_back(point[1]);
  }

  in.pointlist = &pointList.front(); 

  in.numberofsegments = inVertices.size();
  in.numberofholes = 0;
  in.numberofregions = 0;
  
  std::vector<int> segmentList( 0 );
  segmentList.reserve( in.numberofsegments * 2 );
  
  for ( int i = 0; i < in.numberofsegments; ++i ) 
  {
    segmentList.push_back(i);
    if( i == in.numberofsegments - 1 )
      segmentList.push_back(0);
    else
      segmentList.push_back(i+1);
  }

  in.segmentlist = &segmentList[0];

  std::vector<REAL> regionList ( 0 );
  in.regionlist = 0x0; //&regionList[0];

  // init the holelist out here, I think this it was going out of scope
  std::vector<REAL> holeList;
  holeList.reserve ( inner.size() * 2 );
  
  in.numberofholes = inner.size();

  // Do we have any inner loops?
  if( false == inner.empty() )
  {
    for ( InnerLoops::const_iterator iter = inner.begin(); iter != inner.end(); ++iter )
    {
      in.numberofpoints += iter->size();
      
      Usul::Math::Vec2d center;

      // Add the points.
      for( Vertices::const_iterator verticesIter = iter->begin(); verticesIter != iter->end(); ++verticesIter )
      {
        const Usul::Math::Vec2d &point = *verticesIter;
        pointList.push_back(point[0]);
        pointList.push_back(point[1]);

        center = center + point;
      }

      center /= iter->size();

      holeList.push_back( center[0] );
      holeList.push_back( center[1] );
      /// std::cout << "Center of Hole: " << center[0] << " , " << center[1] << std::endl;
      unsigned int start ( in.numberofsegments );
      in.numberofsegments += iter->size();

      // Add the segments
      for (int i = start; i < in.numberofsegments; ++i ) 
      {
        segmentList.push_back( i );
        if( i == in.numberofsegments - 1 )
          segmentList.push_back( start );
        else
          segmentList.push_back( i + 1 );
      }

    }
    
  }

  if ( holeList.size() > 1 )
    in.holelist = &holeList.front();
  else
    in.holelist = 0x0;

  /// The vector gets reallocated and MOVED to a different memory spot!!!
  in.segmentlist = &segmentList[0];
  in.pointlist = &pointList[0];

  //if ( in
  //in.holelist = &holeList[0];

  std::vector<REAL> pointAttributeList ( in.numberofpoints );
  std::fill( pointAttributeList.begin(), pointAttributeList.end(), 0 );
  in.pointattributelist = &pointAttributeList.front();

  std::vector<int> pointmarkerlist ( in.numberofpoints );
  std::fill( pointmarkerlist.begin(), pointmarkerlist.end(), 0 );
  in.pointmarkerlist = &pointmarkerlist.front();
  
  in.numberofpointattributes = 0;
  in.segmentmarkerlist = 0x0;

  // Make sure everything in mid is zero or null
  mid.pointlist = 0x0;
  mid.pointattributelist = 0x0;
  mid.pointmarkerlist = 0x0; 
  mid.trianglelist = 0x0; 
  mid.numberoftriangles = 0;
  mid.triangleattributelist = 0x0;
  mid.neighborlist = 0x0;
  mid.segmentlist = 0x0;
  mid.segmentmarkerlist = 0x0;
  mid.edgelist = 0x0;
  mid.edgemarkerlist = 0x0;
  mid.holelist = 0x0;
  mid.normlist = 0x0;
  mid.trianglearealist = 0x0;

  // Switches that will be sent to the triangulate algorithm.
  std::string switches ( "pzenYq30" );

#ifdef VERBOSE_OUTPUT

  // Turn on verbose output, if not turn on quiet mode.
  switches += "V";

  // Struct to make sure the buffer is flushed if triangulate throws an exception.
  struct Flush { ~Flush() { ::fflush ( stdout ); } } flushBuffer;

#else

  switches += "Q";

#endif
  
  // Triangulate.
  ::triangulate ( const_cast < char* > ( switches.c_str() ), &in, &mid, 0x0 );

  // Number of points that were made.
  unsigned int numberofpoints ( mid.numberofpoints );
  
  // Reserve enough room.
  outVertices.reserve( numberofpoints );
  
  // Populate the out vertices array.
  for( unsigned int i = 0; i < numberofpoints * 2; i += 2 )
  {
    outVertices.push_back( Usul::Math::Vec2d( mid.pointlist[ i ], mid.pointlist[ i + 1 ] ) );
  }

  // Number of triangles that were made.
  unsigned int numTris ( mid.numberoftriangles );

  // Make enough room.
  indices.reserve( numTris * 3 );

  // Populate the indices array.
  for ( unsigned int i = 0; i < numTris * 3; i += 3 )
  {
    indices.push_back( mid.trianglelist[ i ] );
    indices.push_back( mid.trianglelist[ i + 1 ] );
    indices.push_back( mid.trianglelist[ i + 2 ] );
  }

  // Free any memory allocated in triangulate.
  ::trifree(mid.pointlist);
  ::trifree(mid.pointattributelist);
  ::trifree(mid.pointmarkerlist);
  ::trifree(mid.trianglelist);
  ::trifree(mid.triangleattributelist);
  ::trifree(mid.trianglearealist);
  ::trifree(mid.neighborlist);
  ::trifree(mid.segmentlist);
  ::trifree(mid.segmentmarkerlist);
  ::trifree(mid.edgelist);
  ::trifree(mid.edgemarkerlist);
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string TriangulateComponent::getPluginName() const 
{
  return "Triangulate";
}
