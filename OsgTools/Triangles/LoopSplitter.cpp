/*
 *  LoopSplitter.cpp
 *  OsgTools
 *
 *  Created by Michael A Jackson on 9/1/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "LoopSplitter.h"
#include "OsgTools/Triangles/Loop.h"
#include "Usul/Interfaces/IAddSharedVertex.h"
#include "Usul/Interfaces/IGetVertex.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Predicates/EqualVector.h"

#include <iostream>
#include <string>
#include <algorithm>

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  // Class to represent an edge on the boundary of two planes.
  struct Edge
  {
    osg::Vec3 pointOne;
    osg::Vec3 pointTwo;
  };

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Function to build a co planer loop.  Returns false if it reaches a
  //  transition point that is incorrect.
  //
  ///////////////////////////////////////////////////////////////////////////////

  bool buildLoop ( const Loop& loop, Loop &fixedLoop, unsigned int start, unsigned int stop, const LoopSplitter::TransitionPoints &transitions, LoopSplitter::UsedIndices& used )
  {
    //Copy from old loop into new loop
    for ( Loop::const_iterator iter = loop.begin() + start; iter != loop.begin() + stop + 1; iter++) 
    {
      if( iter == loop.end() )
        iter = loop.begin();
      
      unsigned int index ( iter - loop.begin() );

      fixedLoop.append(*iter);
      
      if ( start != index && stop != index ) 
      {
        // Is index a transition point that we aren't looking for?
        if ( transitions.end() != std::find ( transitions.begin(), transitions.end(), index ) )
        {
          //std::cout << "Unexpected transition point " << index << std::endl << "Expecting " << stop << std::endl;
          return false;
        }

        used.push_back( index );
      }
    }

    return true;
  }
  

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Return the average length of a segment for given loop.
  //
  ///////////////////////////////////////////////////////////////////////////////

  float avgSegmentLength( const Loop& loop, Usul::Interfaces::IUnknown *_caller ) 
  {
    float average ( 0.0f );
    const unsigned int size = loop.size();
    for( unsigned int i = 0; i < size -1; ++i)
    {
     osg::Vec3 v1 ( loop.vertex( i, _caller ) );
     osg::Vec3 v2 ( loop.vertex( i+1, _caller ) );
     average += (v1-v2).length();
    }
    average = average / size;
    return average;
  }
 
  struct Finder
  {
    Finder( const osg::Vec3& v, Usul::Interfaces::IUnknown* caller ) : 
      _caller( caller ),
      _v ( v )
    {  }

    bool operator () ( const SharedVertex* sv ) const
    {
      Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( _caller );

      typedef Usul::Predicates::CloseFloat< float > CloseFloat;
      CloseFloat cf;
      Usul::Predicates::EqualVector < CloseFloat, 3 > equalVector ( cf );

      osg::Vec3 v ( getVertex->getVertex( sv->index() ) );

      return equalVector ( v, _v );
      
    }

  private:
    Usul::Interfaces::IUnknown *_caller;
    osg::Vec3 _v;
  };

}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the axis (x, y, or z) to use for the sorting
//
///////////////////////////////////////////////////////////////////////////////

int LoopSplitter::_getSortAxis(int edge) const
{
  switch (edge) 
  {
    case ZMIN_YMIN:
    case ZMAX_YMIN:
    case ZMIN_YMAX:
    case ZMAX_YMAX:
      return X_AXIS;
              
    case XMIN_YMIN:
    case XMAX_YMIN:
    case XMIN_YMAX:
    case XMAX_YMAX:
      return Z_AXIS;

    case XMAX_ZMIN:
    case XMIN_ZMIN:
    case XMIN_ZMAX:    
    case XMAX_ZMAX:
      return Y_AXIS;
      
    default:
      throw std::runtime_error("Error 94630293920: getSortAxis() - Invalid Edge value.");
  }

}


///////////////////////////////////////////////////////////////////////////////
//
// Returns the corner point of the bounding box based on the total of the 
//  three edges
//
///////////////////////////////////////////////////////////////////////////////

int LoopSplitter::_getBoundBoxIndex(int edgeTotal) const 
{
  switch(edgeTotal) 
  {
      case CE_XMIN_YMIN_ZMIN: 
        return XMIN_YMIN_ZMIN;
      case CE_XMAX_YMIN_ZMIN: 
        return XMAX_YMIN_ZMIN;
      case CE_XMIN_YMAX_ZMIN: 
        return XMIN_YMAX_ZMIN;
      case CE_XMAX_YMAX_ZMIN: 
        return XMAX_YMAX_ZMIN;
      case CE_XMIN_YMIN_ZMAX: 
        return XMIN_YMIN_ZMAX;
      case CE_XMAX_YMIN_ZMAX: 
        return XMAX_YMIN_ZMAX;
      case CE_XMIN_YMAX_ZMAX: 
        return XMIN_YMAX_ZMAX;
      case CE_XMAX_YMAX_ZMAX:
        return XMAX_YMAX_ZMAX;
      default:
        throw std::runtime_error("Error 94630293930: _getCornerPoint() - Invalid Edge Total.");
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns the corner point of the bounding box based on a pair of edges
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f LoopSplitter::_getCornerPoint(int edge1, int edge2) const
{
 
  if( _makeCornerDecision(ZMIN_YMIN, XMIN_YMIN, XMIN_ZMIN, edge1, edge2) ) {
      return _boundBox.corner(0);
  } else if (_makeCornerDecision(XMIN_YMIN,ZMAX_YMIN,XMIN_ZMAX, edge1, edge2 ) ) {
      return _boundBox.corner(4);
  } else if (_makeCornerDecision(ZMAX_YMIN, XMAX_YMIN, XMAX_ZMAX, edge1, edge2 ) ) {
      return _boundBox.corner(5);  
  } else if (_makeCornerDecision(XMAX_YMIN,ZMIN_YMIN ,XMAX_ZMIN, edge1, edge2 ) ) {
      return _boundBox.corner(1);  
  } else if (_makeCornerDecision(ZMIN_YMAX,XMIN_YMAX ,XMIN_ZMIN, edge1, edge2 ) ) {
      return _boundBox.corner(2);  
  } else if (_makeCornerDecision(XMIN_YMAX,ZMAX_YMAX ,XMIN_ZMAX, edge1, edge2 ) ) {
      return _boundBox.corner(6);  
  } else if (_makeCornerDecision(ZMAX_YMAX,XMAX_YMAX,XMAX_ZMAX, edge1, edge2 ) ) {
      return _boundBox.corner(7);  
  } else if (_makeCornerDecision(XMAX_YMAX,XMAX_ZMIN ,ZMIN_YMAX, edge1, edge2 ) ) {
      return _boundBox.corner(3);  
  }
         
  throw std::runtime_error ( "Error 94630293921: getCornerPoint() - Invalid edge values." );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Helper method for getCornerPoint
//
///////////////////////////////////////////////////////////////////////////////

bool LoopSplitter::_makeCornerDecision(int a, int b, int c,int e1, int e2) const
{
  
  if ( (e1 == a && e2 == b) || (e1 == b && e2 == c) || (e1 == c && e2 == a) ) 
  {
    return true;
  }
  
  if ( (e2 == a && e1 == b) || (e2 == b && e1 == c) || (e2 == c && e1 == a) )
  {
  return true;
  }
  return false;
}

void LoopSplitter::_printEdgeMap(EdgeMap edgeMap)
{
  std::cout << "EdgeMap" << std::endl;
  for (EdgeMap::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i) {
    std::cout << i->first << std::endl;
  }
  std::cout << "---" << std::endl;
}




///////////////////////////////////////////////////////////////////////////////
//
// Sort Predicate that will compare the osg::Vec3f based on a single Axis
// component.
//
///////////////////////////////////////////////////////////////////////////////

struct AxisSort
{
  
  AxisSort(int a) {
    axis = a;
  }
  
  bool operator () ( const osg::Vec3f& p1, const osg::Vec3f &p2 ) const
  {
      if (axis == LoopSplitter::X_AXIS) {
        return (p1.x()<p2.x());
      } else if (axis == LoopSplitter::Y_AXIS ) {
        return (p1.y()<p2.y());
      } else if (axis == LoopSplitter::Z_AXIS ) {
        return (p1.z()<p2.z());
      } 
        throw std::runtime_error("Error 894587302 - Axis invalid for AxisSort");
  }
  
  private:
    int axis;
};


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

LoopSplitter::LoopSplitter(osg::BoundingBox box, Usul::Interfaces::IUnknown *caller) :
   _boundBox ( box ), _caller ( caller )
{
}

///////////////////////////////////////////////////////////////////////////////
//
// The main entry point for this class. This will split a parent loop into smaller
//  loops that are split on the edges of the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

std::vector<OsgTools::Triangles::Loop> LoopSplitter::split( const OsgTools::Triangles::Loop& loop )
{

  //add in a vector to get the shared vertex objects that are transition points 
  /**
  * Key = the Enumeration value for the edge that the SharedVertex is on
  * Value = A vector of SharedVertex objects that lie on the given edge
  */
  std::map<int, std::vector<osg::Vec3f> >  edgeMap;
  /**
  * Key = SharedVertex that lies on an edge
  * Value = The index in the Loop object where that SharedVertex is
  */
  std::map<osg::Vec3f, unsigned int> tPointIndices;
  
  std::vector<OsgTools::Triangles::Loop> loops;
  
  _findTransitionPoints(loop, edgeMap, tPointIndices);
  
  _createLoops(loop, edgeMap, tPointIndices, loops);
  
  return loops;
}



///////////////////////////////////////////////////////////////////////////////
//
// This will find the transition points where the loop moves from one plane 
//  to another.
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_findTransitionPoints( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices) 
{

    float xMin, xMax, yMin, yMax, zMin, zMax;
    xMin = _boundBox.xMin();
    xMax = _boundBox.xMax();
    yMin = _boundBox.yMin();
    yMax = _boundBox.yMax();
    zMin = _boundBox.zMin();
    zMax = _boundBox.zMax();

    for( unsigned int i = 0; i < loop.size(); ++i)
    {
       const osg::Vec3 & v ( loop.vertex( i, _caller ) );
       if (v.z() == zMin && v.y() == yMin ) { 
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMIN].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.z() == zMin && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.z() == zMax && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[ZMAX_YMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.y() == yMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_YMAX ].push_back(v);
       tPointIndices.insert(TPointIndices::value_type(v,i) );
       }  else if (v.x() == xMax && v.z() == zMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) );
       } else if (v.x() == xMin && v.z() == zMin ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMIN ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       } else if (v.x() == xMin && v.z() == zMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMIN_ZMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       }  else if (v.x() == xMax && v.z() == zMax ) {
        // std::cout << "There is a transition at point: " << i  << std::endl;
        edgeMap[XMAX_ZMAX ].push_back(v);
        tPointIndices.insert(TPointIndices::value_type(v,i) ); 
       }
    }
    return;
}

template < class Vertices >
void bisect ( const osg::Vec3& p1, const osg::Vec3& p2, float distance, Vertices &vertices )
{
  float length ( (p1 - p2).length() );

  if ( length > distance )
  {
    osg::Vec3 v ( (p1 + p2)/2 );
    typename Vertices::iterator i = std::find ( vertices.begin(), vertices.end(), p1 );
    vertices.insert( i + 1, v );
  }
}

template < class Vertices >
bool good ( float distance, const Vertices& vertices )
{
  typedef typename Vertices::value_type Vertex;

  for( typename Vertices::const_iterator iter = vertices.begin(); iter != vertices.end() - 1; ++iter )
  {
    Vertex v1 ( *iter );
    Vertex v2 ( *(iter + 1 ) );

    float length ( (v1-v2).length() );

    if( length > distance )
      return false;
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the loop up into n co-planer loops.
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_createLoops( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Loops & loops) 
{  
  //Start figuring out what we have. There are a few special cases that we know how to handle.

  Edges edges;
  
  /// There is only 1 Edge Involved in the loop
  if ( edgeMap.size() == 1 ) {
    _singleEdge(loop, edgeMap, tPointIndices, edges, loops);
  }
  
  /// There are two edges involved in this loop.
  if ( edgeMap.size() == 2 ) {
    _doubleEdge(loop, edgeMap, tPointIndices, edges, loops );
  }
  
  //If there are 3 edges involved and only 3 transition points, then we are
  // going to have to add the corner point into the mix as a transition point
  if ( edgeMap.size() == 3) {
    _tripleEdge(loop, edgeMap, tPointIndices, edges, loops);
  }

  const float distance ( Detail::avgSegmentLength( loop, _caller ) );

  typedef std::vector< osg::Vec3 > Vertices;
  typedef std::vector< SharedVertex::ValidRefPtr > SharedVertices;
  typedef std::pair< osg::Vec3, osg::Vec3 > EdgePair;
  typedef std::pair< Vertices, SharedVertices > VerticesPair;
  typedef std::map < EdgePair, VerticesPair > Map;

  Map map;
  Usul::Interfaces::IAddSharedVertex::ValidQueryPtr addSharedVertex ( _caller );
  
  for( Edges::const_iterator iter = edges.begin(); iter != edges.end(); ++iter )
  {
    Vertices v;
    v.push_back( iter->pointOne );
    v.push_back( iter->pointTwo );

    while ( !good( distance, v ) )
    {
      Vertices copy ( v );
      for ( Vertices::iterator vertexIter = copy.begin(); vertexIter != copy.end() - 1; ++vertexIter )
        bisect( *vertexIter, *(vertexIter + 1), distance, v );
    }

    // Remove the first and last points ( the transition points ).
    v.erase( v.begin() );
    v.erase( v.end() - 1 );

    SharedVertices shared;

    for( Vertices::const_iterator vertex = v.begin(); vertex != v.end(); ++vertex )
    {
      shared.push_back ( addSharedVertex->addSharedVertex ( *vertex ) );
    }
    
    map [ EdgePair( iter->pointOne, iter->pointTwo ) ] = VerticesPair( v, shared );
  }
  
  // Go through our new loops
  for( Loops::iterator iter = loops.begin(); iter != loops.end(); ++iter )
  {
    //std::cout << "Loop: " << iter-loops.begin() << std::endl;
    // Go through out added points
    for( Map::const_iterator j = map.begin(); j != map.end(); ++j )
    {
      EdgePair edge ( j->first );
      const Vertices &vertices ( j->second.first );
      SharedVertices shared ( j->second.second );

      if ( !vertices.empty() )
      {
        // Does the loop contain both transition points?
       
        Detail::Finder f1( edge.first, _caller );
        Loop::iterator first = std::find_if ( iter->begin(), iter->end(), f1 );
        
        Detail::Finder f2( edge.second, _caller );
        Loop::iterator second = std::find_if ( iter->begin(), iter->end(), f2 );

        if ( first != iter->end() && second != iter->end() ) 
        {
          Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( _caller );
          osg::Vec3 v1 ( getVertex->getVertex( (*first)->index() ) );
          osg::Vec3 v2 ( getVertex->getVertex( (*second)->index() ) );

          float dOne ( ( v1 - vertices[0]).length() );
          float dTwo ( ( v2 - vertices[0]).length() );

          if( first == (iter->end() - 1) || second == (iter->end() - 1) )
          {
            osg::Vec3 v ( getVertex->getVertex( (*(iter->end() - 1))->index() ) );

            float d1 ( ( v - vertices[0]).length() );
            float d2 ( ( v - vertices.back()).length() );

            if( d1 > d2 )
            {
              std::reverse( shared.begin(), shared.end() );
            }

            iter->insert( iter->end(), shared.begin(), shared.end() );
          }

          // If the first transition point is closer to the begining.
          else if ( first < second )
          {
            // If vertices[0] is closer to second than first, then reverse
            if( dOne > dTwo )
              std::reverse( shared.begin(), shared.end() );

            // Insert before the second transition point.
            iter->insert( second, shared.begin(), shared.end() );
          }
          else
          {
            // If vertices[0] is closer to first than second, then reverse
            if( dTwo > dOne )
              std::reverse( shared.begin(), shared.end() );

            // Insert after second
            iter->insert( second + 1, shared.begin(), shared.end() );
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// This method takes care of at least one case where the loop crosses 3 edges.
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_tripleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Edges& e, Loops & loops) 
{
 // std::cout << "----- _tripleEdge -----" << std::endl;
  bool ok = true;
  int edgeTotal = 0;
  int bbIndex = -1;
  std::vector<osg::Vec3f> v; //Create a new Vector to hold the Vec3f
  std::vector<unsigned int> edges; //Create a Vector to hold the Edge Numbers
  
  //Now test the size of each vector to make sure it is one.
  for (EdgeMap::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i) {
     if (i->second.size() != 1) { ok = false; }
     else { 
      edgeTotal += i->first;
      //Good place to add extra points
      v.push_back( i->second.front() );
      edges.push_back(i->first); //add the edge number
     }
  }
  if (ok == false) {
     // We have some other configuration that needs to be reduced to 3 points on three edges first
     throw std::runtime_error("Error 293480987 - 3 Edges with Transition Point Size > 1 on an edge.");
  } 
  
  //Make a new Loop
  bbIndex = _getBoundBoxIndex(edgeTotal);
  osg::Vec3f cornerVert (_boundBox.corner(bbIndex) );
  
  /// Now we know which corner point to add, Add a new Shared Vertex to the Document
  Usul::Interfaces::IAddSharedVertex::ValidQueryPtr    addSharedVertex ( _caller );
  SharedVertex* sv ( addSharedVertex->addSharedVertex( cornerVert ) );  
 // std::cout << "Adding Vertex succeeded" << std::endl;

  
  /// Now lets make some loops
  /** Probably need a helper function.....  */
  TransitionPoints transitionPoints;
  // Place all transition points in a vector to test against.
  for ( TPointIndices::iterator iter = tPointIndices.begin(); iter != tPointIndices.end(); ++iter )
  {
    transitionPoints.push_back( iter->second );

    Detail::Edge edge;

    edge.pointOne = loop.vertex( iter->second, _caller );
    edge.pointTwo = cornerVert;

    e.push_back( edge );
  }

  // Start and stop values for building the loop.
  int startIndex = 0;
  int stopIndex = 0;
  
//  std::cout << "Pairs that should be together:" << std::endl;
  for ( unsigned int i = 0; i < v.size();++i) 
  {
    Loop fixedLoop;
    
    TPointIndices::iterator it = tPointIndices.find ( v[i] );
    if ( tPointIndices.end() != it ) 
    {
      startIndex = it->second;
    }
    
    if (i == v.size()-1) {
      TPointIndices::iterator ite = tPointIndices.find ( v[0] );
      if ( tPointIndices.end() != ite ) 
      {
        stopIndex = ite->second;
      }
    } else {
      TPointIndices::iterator ite = tPointIndices.find ( v[i+1] );
      if ( tPointIndices.end() != ite ) 
      {
        stopIndex = ite->second;
      }
    }

   // std::cout << "[ " << startIndex << ", " << stopIndex << " ]" << std::endl;
    UsedIndices local;
    
    fixedLoop.append(sv); //Add the corner point to the loop

    if ( Detail::buildLoop( loop, fixedLoop, startIndex, stopIndex, transitionPoints, local ) )
    {
       loops.push_back(fixedLoop);
    }
    else
    {
      local.clear();
      fixedLoop.clear();
      fixedLoop.append(sv); //Add the corner point to the loop first
      // try it again with opposite starting points.
      if ( false == Detail::buildLoop( loop, fixedLoop, stopIndex, startIndex, transitionPoints, local ) )
        throw std::runtime_error ("Cannot split this loop." );
      loops.push_back( fixedLoop );
    }

  }  //End for loop 

}

///////////////////////////////////////////////////////////////////////////////
//
// This method handles 2 edges on a loop
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_doubleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Edges& e, Loops & loops) 
{
    //std::cout << "----- _doubleEdge ----" << std::endl;
    UsedIndices usedIndices; //Store indices that need to be erased from the loop object
   /// This is where it will break. we need this in a loop
    for (EdgeMap::iterator edgeIter = edgeMap.begin(); edgeIter != edgeMap.end(); ++edgeIter)
    {
      std::vector<osg::Vec3f> & v = edgeIter->second;
      int edge = edgeIter->first;

      if (v.size()%2 == 0) 
      {      
        _edgeHelper(loop,edge,v,tPointIndices, e, loops, usedIndices);
       } else {
        //This is really bad.
        throw std::runtime_error("Error:2039402349 - Odd Number of Transition points on 1 Edge. Not Handled.");
       }
      }
      //Erase the indices from a copy of the parent loop
      _parentLoopCopier(loop,loops, usedIndices);
 
}


///////////////////////////////////////////////////////////////////////////////
//
// This method handles the case where there is one edge involved in the loop
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_singleEdge( const OsgTools::Triangles::Loop& loop, EdgeMap & edgeMap, TPointIndices & tPointIndices, Edges &e, Loops & loops) 
{
  //std::cout << "------ _singleEdge --------" << std::endl;
  std::vector<osg::Vec3f> & v = edgeMap.begin()->second;
  int edge = edgeMap.begin()->first;

  UsedIndices usedIndices; //Store indices that need to be erased from the loop object
  if (v.size()%2 == 0) 
  {   

   /// Trying to move common code out to another method
    _edgeHelper(loop, edge, v ,tPointIndices, e, loops, usedIndices);
    
    //Erase the indices from a copy of the parent loop
    _parentLoopCopier(loop, loops, usedIndices);
    
  } else {
    //This is really bad.
    throw std::runtime_error("Error: 2039402349 - Odd Number of Transition points on 1 Edge. Not Handled.");
  }

}
  
///////////////////////////////////////////////////////////////////////////////
//
// This method will copy from the parent loop just the indices that it needs 
//  to make a coplanar loop and then push that new loop onto the vector of
//  loops
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_parentLoopCopier(const OsgTools::Triangles::Loop & loop, Loops & loops, UsedIndices & usedIndices) 
{
  //std::cout << "------     _parentLoopCopier --------" << std::endl;

  // Make a copy of the loop that we are splitting.
  Loop parentCopy ( loop ); //Copy Constructor
    
  // Sort from least to greatest.
  std::sort(usedIndices.begin(), usedIndices.end() );
    
  // Erase all the indices that were used above.  Every thing that is left is our final co-planar loop.
  for ( UsedIndices::reverse_iterator it = usedIndices.rbegin();it != usedIndices.rend(); it++ ) 
  {
    parentCopy.erase( *it );
  }

  loops.push_back(parentCopy);

}


///////////////////////////////////////////////////////////////////////////////
//
// Helper method to actually split up some types of loops
//
///////////////////////////////////////////////////////////////////////////////

void LoopSplitter::_edgeHelper(const OsgTools::Triangles::Loop& loop, const unsigned int edge, std::vector<osg::Vec3> & v, TPointIndices & tPointIndices, Edges &edges, Loops & loops, UsedIndices & usedIndices  )
{
  //std::cout << "------     _edgeHelper --------" << std::endl;
  /// The number of transition points must be even
    if (v.size()%2 != 0) 
    {
      throw std::runtime_error("Error: 2039402350 - _edgeHelper() Non even number of Transition Points");
    }
    
    std::sort(v.begin(), v.end(), AxisSort( _getSortAxis(edge) ) );

    TransitionPoints transitionPoints;

    // Place all transition points in a vector to test against.
    for ( TPointIndices::iterator iter = tPointIndices.begin(); iter != tPointIndices.end(); ++iter )
      transitionPoints.push_back( iter->second );

    for ( unsigned int i = 0; i < v.size();i+=2) 
    {
      Loop fixedLoop;
      
      // Start and stop values for building the loop.
      const unsigned int startIndex ( tPointIndices[ v[i] ] );
      const unsigned int stopIndex  ( tPointIndices[ v[i + 1] ] );

      UsedIndices local;

      if ( false == Detail::buildLoop( loop, fixedLoop, startIndex, stopIndex, transitionPoints, local ) )
      {
        local.clear();
        fixedLoop.clear();

        // try it again with opposite starting points.
        if ( false == Detail::buildLoop( loop, fixedLoop, stopIndex, startIndex, transitionPoints, local ) )
          throw std::runtime_error ("Cannot split this loop." );
        
        loops.push_back( fixedLoop );
      }

      // Insert the new loop
      loops.push_back( fixedLoop );

      usedIndices.insert( usedIndices.end(), local.begin(), local.end() );
    }

    for( TransitionPoints::iterator iter = transitionPoints.begin(); iter != transitionPoints.end(); iter += 2 )
    {
      Detail::Edge e;
      
      e.pointOne = loop.vertex( *iter, _caller );
      e.pointTwo = loop.vertex( *(iter + 1 ), _caller );

      edges.push_back( e );
    }
}

