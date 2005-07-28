
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_CAP_POLYGONS_H__
#define __USUL_ALGORITHMS_CAP_POLYGONS_H__

#include "Usul/Polygons/Predicates.h"

namespace Usul {
namespace Loops {


namespace Detail {


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
    loop.push_back( v1 );

    v1->visited ( true );

    visitSharedVertex( polygons, uncapped, loop, v2 );
  }

  else if( v2->numTriangles() == 1 )
  {
    //Add the shared vertex to the current loop
    loop.push_back( v2 );

    v2->visited( true );

    visitSharedVertex( polygons, uncapped, loop, v1 );
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
    //Check to see if v1 was added already
    if( containsVertex ( loop, v1 ) )
    {
      findEmptySharedVertex ( polygons, uncapped, loop, p, v2, v3 );
    }
    //Check to see if v2 was added already
    else if( containsVertex ( loop, v2 ) )
    {
      findEmptySharedVertex ( polygons, uncapped, loop, p, v1, v3 );
    }
    //Check to see if v3 was added already
    else if( containsVertex ( loop, v3 ) )
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

  //PolygonList p1 ( sv->polygons() );

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
        //Add the vertex to the current loop
        loop.push_back( sv );

        //Remove it from candidates so we don't add it again
        uncapped.remove ( index );

        //Get the polygon
        PolygonPtr p ( polygons.at( index ) );

        //Visit this polygon
        visitPolygon( polygons, uncapped, loop, p.get() );
      }
    }
  }

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


}


//////////////////////////////////////////////////////////////////////////////
//
//  Walk the graph and find all polygons that need to be capped.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Polygons,
  class IndexSequence, 
  class Loops, 
  class AdjacencyTest,
  class UpdateFunctor
>
inline void capPolygons ( Polygons& polygons, IndexSequence& uncapped, Loops& loops, const AdjacencyTest& adjacent, unsigned int vertsPerPoly, UpdateFunctor& updater )
{
  typedef typename Polygons::value_type PolygonPtr;
  typedef typename PolygonPtr::element_type Polygon;
  typedef typename Polygon::PolygonSet  PolygonSet;
  typedef typename Loops::value_type Loop;

  //Needed for user feedback
  const unsigned int size ( polygons.size() );

  //Walk through all the polygons
  for( typename Polygons::iterator iter = polygons.begin(); iter != polygons.end(); ++iter )
  {    
    //Get list of neighbors that share one point
    PolygonSet neighbors; 
        
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
    if( adjacentPolygons.size() < vertsPerPoly + 1 )
    {
      uncapped.push_back( (*iter)->index() );
      Detail::findEdge( adjacentPolygons, iter->get() );
    }

    //Send a progress upate
    updater ( uncapped );

    unsigned int current ( iter - polygons.begin() );

    updater ( current, size );
  }

  updater( uncapped, true );

  //Sort for binary search
  uncapped.sort( std::less<unsigned int>() );
  
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

    updater( loops.size() );
  }
}

}

}

#endif //__USUL_ALGORITHMS_CAP_POLYGONS_H__

