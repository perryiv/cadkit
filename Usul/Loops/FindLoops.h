
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_CAP_POLYGONS_H__
#define __USUL_ALGORITHMS_CAP_POLYGONS_H__

namespace Usul {
namespace Algorithms {


namespace Detail {


//////////////////////////////////////////////////////////////////////////////
//
//  Visit a polygon
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygons, class IndexSequence, class Loop, class Polygon >
void visitPolygon( Polygons& polygons, IndexSequence& uncapped, Loop& loop, Polygon* p )
{
  typedef typename Polygon::SharedVertex SharedVertex;

  //Return now if already visited
  if( p->visited() )
    return;

  
  visitSharedVertex( polygons, uncapped, loop, p->vertex1() );

  visitSharedVertex( polygons, uncapped, loop, p->vertex2() );
  
  visitSharedVertex( polygons, uncapped, loop, p->vertex3() );

  p->visited( true );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Visit a shared vertex
//
///////////////////////////////////////////////////////////////////////////////

template < class Polygons, class IndexSequence, class Loop, class SharedVertex >
void visitSharedVertex( Polygons& polygons, IndexSequence& uncapped, Loop& loop, SharedVertex* sv )
{
  typedef typename SharedVertex::PolygonList PolygonList;
  typedef typename Polygons::value_type PolygonPtr;

  //Return now if already visited
  if( sv->visited() )
    return;

  //Return now if not on edge
  if( !sv->onEdge() )
    return;

  //Mark the shared vertex as visited
  sv->visited( true );

  PolygonList p1 ( sv->polygons() );

  //Loop through the shared vertex's polygons
  for( typename PolygonList::iterator poly = p1.begin(); poly != p1.end(); ++poly )
  {
    //If we haven't visited already...
    if( !(*poly)->visited() )
    {
      //Used below
      const unsigned index ( (*poly)->index() );

      //Is the index in the list of candidates?
      if( std::binary_search( uncapped.begin(), uncapped.end(), index ) )
      {
        //Add the index to the current loop
        loop.push_back( typename Loop::value_type ( index, sv->value() ) );

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
void findEdge ( PolygonList& polygons, Polygon* check )
{
  typedef typename Polygon::SharedVertex SharedVertex;

  SharedVertex *v1 ( check->vertex1() );
  SharedVertex *v2 ( check->vertex2() );
  SharedVertex *v3 ( check->vertex3() );

  unsigned int v1Count( 0 );
  unsigned int v2Count( 0 );
  unsigned int v3Count( 0 );

  for( typename PolygonList::iterator i = polygons.begin(); i != polygons.end(); ++i )
  {
    if( *i != check )
    {
      SharedVertex *t1 ( (*i)->vertex1() );
      SharedVertex *t2 ( (*i)->vertex2() );
      SharedVertex *t3 ( (*i)->vertex3() );

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
  class AdjacencyMap,
  class IndexSequence, 
  class Loops, 
  class AdjacencyTest,
  class UpdateFunctor
>
inline void capPolygons ( AdjacencyMap& map, IndexSequence& uncapped, Loops& loops, const AdjacencyTest& adjacent, unsigned int vertsPerPoly, UpdateFunctor& updater )
{
  typedef typename AdjacencyMap::Polygons Polygons;
  typedef typename Polygons::value_type Polygon;
  typedef typename AdjacencyMap::SharedVertex SharedVertex;
  typedef typename SharedVertex::PolygonList PolygonList;
  typedef typename Loops::value_type Loop;

  map.setAllUnvisited();

  Polygons polygons ( map.polygons() );

  //Walk through all the polygons
  for( typename Polygons::iterator iter = polygons.begin(); iter != polygons.end(); ++iter )
  { 
    //Get list of neighbors that share one point
    PolygonList neighbors ( (*iter)->getNeighbors() );

    PolygonList adjacentPolygons;

    //Loop through all this polygon's neighbors
    for( typename PolygonList::iterator i = neighbors.begin(); i != neighbors.end(); ++i )
    {
      //Self check...
      if( (*iter)->index() != (*i)->index() )
      {
        //If these two polygons are adjacent...
        if( adjacent ( *(*iter), *(*i) ) )
          adjacentPolygons.push_back( *i );
      }
    }

    //If we don't have the right number of adjacent polygons...
    if( adjacentPolygons.size() < vertsPerPoly )
    {
      uncapped.push_back( (*iter)->index() );
      Detail::findEdge( adjacentPolygons, iter->get() );
    }

    //Send a progress upate
    updater ( uncapped );
  }

  updater( uncapped, true );

  //Sort for binary search
  uncapped.sort( std::less<unsigned int>() );
  
  while( !uncapped.empty() )
  {
    typename IndexSequence::iterator i ( uncapped.begin() );

    //Create a cluster
    Loop loop;

    //Get the polygon
    Polygon p ( polygons.at( *i ) );

    //this will loop around the gap and build the proper loop
    Detail::visitPolygon( polygons, uncapped, loop, p.get() );

    //Push the loop onto the answer
    loops.push_back( loop );
  }
}

}

}

#endif //__USUL_ALGORITHMS_CAP_POLYGONS_H__

