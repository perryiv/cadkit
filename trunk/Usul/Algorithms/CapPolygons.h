
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

template < class Polygons, class IndexSequence, class Loop, class Polygon >
void visitPolygon( Polygons& polygons, IndexSequence& uncapped, Loop& loop, Polygon& p )
{
  typedef typename Polygon::SharedVertex SharedVertex;

  if( p.visited() )
    return;

  SharedVertex *v1 ( p.vertexOne() );

  if( !v1->visited() )
  {
    visitSharedVertex( polygons, uncapped, loop, *v1 );
  }

  SharedVertex *v2 ( p.vertexTwo() );
  if( !v2->visited() )
  {
    visitSharedVertex( polygons, uncapped, loop, *v2 );
  }

  SharedVertex *v3 ( p.vertexThree() );
  if( !v3->visited() )
  {
    visitSharedVertex( polygons, uncapped, loop, *v3 );
  }

  p.visited( true );
}

template < class Polygons, class IndexSequence, class Loop, class SharedVertex >
void visitSharedVertex( Polygons& polygons, IndexSequence& uncapped, Loop& loop, SharedVertex& sv )
{
  typedef typename SharedVertex::PolygonList PolygonList;
  typedef typename Polygons::value_type Polygon;

  PolygonList p1 ( sv.getPolygons() );
  for( PolygonList::iterator poly = p1.begin(); poly != p1.end(); ++poly )
  {
    if( !(*poly)->visited() )
    {
      if( std::binary_search( uncapped.begin(), uncapped.end(), (*poly)->index() ) )
      {
        loop.push_back( (*poly)->index() );
        uncapped.remove ( (*poly)->index() );
        Polygon p ( polygons.at( (*poly)->index() ) );
        visitPolygon( polygons, uncapped, loop, p );
      }
      (*poly)->visited( true );
    }
  }
  sv.visited( true );
}

}


template
<
  class AdjacencyMap,
  class IndexSequence, 
  class Loops, 
  class AdjacencyTest,
  class UpdateFunctor,
  class CancelFunctor
>
inline void capPolygons ( AdjacencyMap& map, IndexSequence& uncapped, Loops& loops, const AdjacencyTest& adjacent, unsigned int vertsPerPoly, UpdateFunctor& updater, const CancelFunctor& cancel)
{
  typedef typename AdjacencyMap::Polygons Polygons;
  typedef typename Polygons::value_type Polygon;
  typedef typename AdjacencyMap::SharedVertex SharedVertex;
  typedef typename SharedVertex::PolygonList PolygonList;
  typedef typename Loops::value_type Loop;

  Polygons polygons ( map.polygons() );

  map.setAllUnvisited();

  //Walk through all the polygons
  for( Polygons::iterator iter = polygons.begin(); iter != polygons.end(); ++iter )
  { 
    if( cancel() )
      return;

    PolygonList neighbors ( iter->getNeighbors() );

    PolygonList adjacentPolygons;

    //Loop through all this polygon's neighbors
    for( PolygonList::iterator i = neighbors.begin(); i != neighbors.end(); ++i )
    {
      //Self check...
      if( iter->index() != (*i)->index() )
      {
        //If these two polygons are adjacent...
        if( adjacent ( &*iter, *i ) )
          adjacentPolygons.push_back( *i );
      }
    }

    //If we don't have the right number of adjacent polygons...
    if( adjacentPolygons.size() < vertsPerPoly )
    {
      uncapped.push_back( iter->index() );  
    }

    //Send a progress upate
    updater ( uncapped );
  }

  updater( uncapped, true );

  uncapped.sort( std::less<unsigned int>() );
  
  while( !uncapped.empty() )
  {
    IndexSequence::iterator i ( uncapped.begin() );

    //Create a cluster
    Loop loop;
    loop.push_back( *i );

    //Get the polygon
    Polygon p ( polygons.at( *i ) );
    
    //No longer needed, remove so we don't come back
    uncapped.erase( i );

    //this will loop around the gap and build the proper loop
    Detail::visitPolygon( polygons, uncapped, loop, p );

    loops.push_back( loop );
  }
}

}

}

#endif //__USUL_ALGORITHMS_CAP_POLYGONS_H__

