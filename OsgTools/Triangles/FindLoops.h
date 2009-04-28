
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_TRIANGLES_FIND_LOOPS__
#define __OSG_TOOLS_TRIANGLES_FIND_LOOPS__

#include "Usul/Types/Types.h"

#include "OsgTools/Triangles/Predicates.h"
#include "OsgTools/Triangles/TriangleFunctor.h"

#include <iostream>
#include <vector>

using namespace Usul::Types;

namespace OsgTools {
namespace Triangles {
namespace Detail {

  std::vector< unsigned int > cache;

///////////////////////////////////////////////////////////////////////////////
//
//  Estimates the number of seconds to completion of a process
//
///////////////////////////////////////////////////////////////////////////////
    
Uint32 timeLeft (Uint64 startTime, Uint64 now, Uint32 current, Uint32 size) 
{
    Float32 percent ( (float)current/ (float)size );
    return (Uint32) ( ( (now-startTime)/percent ) - (now - startTime) )/ 1000;
}
    
//////////////////////////////////////////////////////////////////////////////
//
//  Does the loop already contain this vertex
//
///////////////////////////////////////////////////////////////////////////////

template < class Loop, class Vertex >
inline bool containsVertex ( const Loop& loop, Vertex* vertex )
{
  typename Vertex::FindVertex find ( vertex );

  return ( std::find_if ( loop.begin(), loop.end(), find ) != loop.end() );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Find which vertex doesn't have any other polygons connected to it
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygons, class IndexSequence, class Loop, class Polygon, class SharedVertex >
inline void findEmptySharedVertex( Polygons& polygons, IndexSequence& uncapped, Loop& loop, Polygon* p, SharedVertex *v1, SharedVertex *v2 )
{
  //Polygon list will always contain at least one polygon
  if( v1->numTriangles() == 1 )
  {
    //Add the shared vertex to the current loop
    loop.append ( v1 );

    v1->visited ( true );

    visitSharedVertex( polygons, uncapped, loop, v2 );
  }

  else if( v2->numTriangles() == 1 )
  {
    //Add the shared vertex to the current loop
    loop.append ( v2 );

    v2->visited( true );

    visitSharedVertex( polygons, uncapped, loop, v1 );
  }
  else
  {
    if ( v1->numTriangles() < v2->numTriangles() )
      visitSharedVertex( polygons, uncapped, loop, v1 );
    else
      visitSharedVertex( polygons, uncapped, loop, v2 );
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Visit a polygon
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygons, class IndexSequence, class Loop, class Polygon >
inline void visitPolygon( Polygons& polygons, IndexSequence& uncapped, Loop& loop, Polygon* p )
{
  typedef typename Polygon::SharedVertex SharedVertex;

  //Return now if already visited
  if( p->visited() )
    return;

  SharedVertex *v1 ( p->vertex0() );
  SharedVertex *v2 ( p->vertex1() );
  SharedVertex *v3 ( p->vertex2() );

  //Check to see if all three shared vertices are on the edge
  if( v1->onEdge() && v2->onEdge() && v3->onEdge() )
  {
    const bool containsV1 ( containsVertex ( loop, v1 ) );
    const bool containsV2 ( containsVertex ( loop, v2 ) );
    const bool containsV3 ( containsVertex ( loop, v3 ) );

    if( containsV1 && containsV2 ) 
    {
      visitSharedVertex( polygons, uncapped, loop, v3 );
    }
    else if( containsV1 && containsV3 ) 
    {
      visitSharedVertex( polygons, uncapped, loop, v2 );
    }
    else if( containsV2 && containsV3 ) 
    {
      visitSharedVertex( polygons, uncapped, loop, v1 );
    }
    //Check to see if v1 was added already
    if( containsV1 )
    {
      findEmptySharedVertex ( polygons, uncapped, loop, p, v2, v3 );
    }
    //Check to see if v2 was added already
    else if( containsV2 )
    {
      findEmptySharedVertex ( polygons, uncapped, loop, p, v1, v3 );
    }
    //Check to see if v3 was added already
    else if( containsV3 )
    {
      findEmptySharedVertex ( polygons, uncapped, loop, p, v1, v2 );
    }
    //No shared vertices added yet
    else
    {
      //Polygon list will always contain at least one polygon
      if( v1->numTriangles() != 1 )
      {
        visitSharedVertex( polygons, uncapped, loop, v1 );
      }

      else if( v2->numTriangles() != 1 )
      {
        visitSharedVertex( polygons, uncapped, loop, v2 );
      }
      else if( v3->numTriangles() != 1 )
      {
        visitSharedVertex( polygons, uncapped, loop, v3 );
      }
    }

  }
  else
  {
    visitSharedVertex( polygons, uncapped, loop, p->vertex0() );

    visitSharedVertex( polygons, uncapped, loop, p->vertex1() );
    
    visitSharedVertex( polygons, uncapped, loop, p->vertex2() );
  }

  p->visited( true );
}


struct PolygonSort
{
  template < class PolygonPtr > bool operator () ( const PolygonPtr& p1, const PolygonPtr &p2 ) const
  {
    typedef typename PolygonPtr::element_type Polygon;
    typedef typename Polygon::PolygonSet PolygonSet;
#if 0
    PolygonSet s1, s2;

    p1->getNeighbors( s1 );
    p2->getNeighbors( s2 );
#endif
    unsigned int c1 ( cache.at( p1->index() ) );
    unsigned int c2 ( cache.at( p2->index() ) );
#if 0
    Usul::Polygons::TriangleTest adjacent;

    //Loop through all this polygon's neighbors
    for( typename PolygonSet::iterator i = s1.begin(); i != s1.end(); ++i )
    {
      //If these two polygons are adjacent...
      if( adjacent ( *p1, *(*i) ) )
        ++c1;
    }
  
    //Loop through all this polygon's neighbors
    for( typename PolygonSet::iterator i = s2.begin(); i != s2.end(); ++i )
    {
      //If these two polygons are adjacent...
      if( adjacent ( *p2, *(*i) ) )
        ++c2;
    }
#endif
    if( c1 == c2 )
    {
      // If the two counts are equal, more tests are needed to decide which polygon
      // to visit.  A line test may be needed.
    }

    return c1 < c2;
  }
};

//////////////////////////////////////////////////////////////////////////////
//
//  Visit a shared vertex
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygons, class IndexSequence, class Loop, class SharedVertex >
inline void visitSharedVertex( Polygons& polygons, IndexSequence& uncapped, Loop& loop, SharedVertex* sv )
{
  typedef typename SharedVertex::TriangleSequence PolygonList;
  typedef typename Polygons::value_type PolygonPtr;

  //Return now if already visited
  if( sv->visited() )
    return;

  //Return now if not on edge
  if( !sv->onEdge() )
    return;

  //Mark the shared vertex as visited
  sv->visited( true );

  PolygonList polyCandidates;
  
  //Loop through the shared vertex's polygons
  for( typename PolygonList::iterator poly = sv->begin(); poly != sv->end(); ++poly )
  {
    //If we haven't visited already...
    if( !(*poly)->visited() )
    {
      //Used below
      const unsigned index ( (*poly)->index() );
          
      //Is the index in the list of candidates?
      if( std::binary_search( uncapped.begin(), uncapped.end(), index ) )
      {
        //Add the Polygon to the Candidates to check out
        polyCandidates.push_back( (*poly) );
      }
    }
  }

  //Check if polyCandidates.size() > 1
  
  //If size==1, do the recurse
  if ( polyCandidates.size() == 1 ) 
  {
    //Add the vertex to the current loop
    loop.append ( sv );
    
    // Get the index.
    const unsigned index ( polyCandidates.front()->index() );
    
    // Remove it from candidates so we don't add it again.
    uncapped.remove ( index );
    
    // Get the polygon.
    PolygonPtr p ( polygons.at( index ) );
    
    // Visit this polygon.
    visitPolygon( polygons, uncapped, loop, p.get() );
  }
  else if ( polyCandidates.size() > 1) //else check for proper Poly
  {
    // Add the vertex to the current loop.
    loop.append ( sv );

    // Sort the polygons based on the order to visit.
    std::sort ( polyCandidates.begin(), polyCandidates.end(), PolygonSort() );

    // Loop through the polygons.
    for ( typename PolygonList::iterator i = polyCandidates.begin(); i != polyCandidates.end(); ++ i )
    {  
      // Get the index.
      const unsigned index ( (*i)->index() );
      
      // Remove it from candidates so we don't add it again.
      uncapped.remove ( index );
      
      // Get the polygon.
      PolygonPtr p ( polygons.at( index ) );
      
      // Visit this polygon.
      visitPolygon( polygons, uncapped, loop, p.get() );
    }
    
  }

  //if size is 0 then STOP recursion.. do nothing

}


//////////////////////////////////////////////////////////////////////////////
//
//  Mark the shared vertices as on edge if they don't share two adjacent polygons
//
///////////////////////////////////////////////////////////////////////////////

template < class PolygonList, class Polygon >
inline void findEdge ( PolygonList& polygons, Polygon* check )
{
  typedef typename Polygon::SharedVertex SharedVertex;

  SharedVertex *v1 ( check->vertex0() );
  SharedVertex *v2 ( check->vertex1() );
  SharedVertex *v3 ( check->vertex2() );

  unsigned int v1Count( 0 );
  unsigned int v2Count( 0 );
  unsigned int v3Count( 0 );

  for( typename PolygonList::iterator i = polygons.begin(); i != polygons.end(); ++i )
  {
    if( *i != check )
    {
      SharedVertex *t1 ( (*i)->vertex0() );
      SharedVertex *t2 ( (*i)->vertex1() );
      SharedVertex *t3 ( (*i)->vertex2() );

      if( v1 == t1 || v1 == t2 || v1 == t3 )
        ++v1Count;
      if( v2 == t1 || v2 == t2 || v2 == t3 )
        ++v2Count;
      if( v3 == t1 || v3 == t2 || v3 == t3 )
        ++v3Count;
    }
  }

  if( v1Count < 2 )
    v1->onEdge( true );
  if( v2Count < 2 )
    v2->onEdge( true );
  if( v3Count < 2 )
    v3->onEdge( true );
}


} // namespace Detail.


//////////////////////////////////////////////////////////////////////////////
//
//  Walk the graph and find all polygons that need to be capped.
//
///////////////////////////////////////////////////////////////////////////////

template
<
class Polygons,
class Loops, 
class AdjacencyTest,
class UpdateFunctor
>
inline void capPolygons ( Polygons& polygons, Loops& loops, const AdjacencyTest& adjacent, unsigned int vertsPerPoly, UpdateFunctor& updater )
{
    // Typedefs.
    typedef typename Polygons::value_type PolygonPtr;
    typedef typename PolygonPtr::element_type Polygon;
    typedef typename Polygon::PolygonSet  PolygonSet;
    typedef typename Loops::value_type Loop;
    typedef std::list < unsigned int > IndexSequence;
    
    IndexSequence uncapped;
    
    // Needed for user feedback.
    unsigned int size ( polygons.size() );
    unsigned int current ( 0 ); //reusable variable
    Usul::Types::Uint64 now (Usul::System::Clock::milliseconds());
    Usul::Types::Uint64 startTime (Usul::System::Clock::milliseconds());
    
    // Make the cache big enough.
    Detail::cache.resize ( size );
    
    int total = 0;
    //Walk through all the polygons
    for( typename Polygons::iterator iter = polygons.begin(); iter != polygons.end(); ++iter )
    {    
        //Get list of neighbors that share one point
        PolygonSet neighbors; 
        
        /** ************************
        *** HOT SPOT - This call takes a large amount of time
        ** ************************* */
        (*iter)->getNeighbors( neighbors );
        
        PolygonSet adjacentPolygons;
        
        //Loop through all this polygon's neighbors
        for( typename PolygonSet::iterator i = neighbors.begin(); i != neighbors.end(); ++i )
        {
            //If these two polygons are adjacent...
            if( adjacent ( *(*iter), *(*i) ) )
                adjacentPolygons.insert( *i );
        }
        
        //If we don't have the right number of adjacent polygons...
        // It's plus one because iter will allways be added into adjacentPolygons.
        if( adjacentPolygons.size() < vertsPerPoly + 1 )
        {
            uncapped.push_back( (*iter)->index() );
            Detail::findEdge( adjacentPolygons, iter->get() );
            (*iter)->onEdge( true );
        }
        
        Detail::cache.at( (*iter)->index() ) = adjacentPolygons.size();
        
        //Send a progress update
        
        current = iter - polygons.begin() ;

        // Update the Status Bar
        now = Usul::System::Clock::milliseconds();
        Uint32 estimate (  Detail::timeLeft(startTime, now, current, size) );
        std::ostringstream os;
        os << "[" << estimate << " Sec Remain in Step 1 of 3] Edge Polygons Found: ";
        updater ( uncapped, os.str(), current, size );
        // Update the ProgressBar
        //updater ( current, size );
    }
    
    std::cout << "Total Bad Triangles: " << total << std::endl;
    //Sort for binary search
    uncapped.sort( std::less<unsigned int>() );
    size = uncapped.size();
    current = 0;
    startTime = Usul::System::Clock::milliseconds();
    
    while( !uncapped.empty() )
    {
        // Get the first element in the list.
        typename IndexSequence::iterator i ( uncapped.begin() );
        
        //Create a loop.
        Loop loop;
        
        //Get the polygon
        PolygonPtr p ( polygons.at( *i ) );
        
        //this will loop around the gap and build the proper loop
        Detail::visitPolygon( polygons, uncapped, loop, p.get() );
        
        //Only add if the loop's size is greater than or equal to 3
        if( loop.size() >= 3 )
        {
            //Push the loop onto the answer
            loops.push_back( loop );
        }
        current = size - uncapped.size();

        // Update the Status Bar
        now = Usul::System::Clock::milliseconds();
        Uint32 estimate (  Detail::timeLeft(startTime, now, current, size) );
        std::ostringstream os;
        os << "[" << estimate << " Sec Remain in Step 2 of 3] Loops Created: ";
        updater (  loops.size(), os.str(), current, size );
        // Update the ProgressBar
        //updater ( current, size );
    }
    
    // Clear the cache
    Detail::cache.clear();
}


}
}

#endif //__OSG_TOOLS_TRIANGLES_FIND_LOOPS__
