
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

  SharedVertex *v1 ( p->vertex1() );

  if( !v1->visited() )
  {
    visitSharedVertex( polygons, uncapped, loop, v1 );
  }

  SharedVertex *v2 ( p->vertex2() );
  if( !v2->visited() )
  {
    visitSharedVertex( polygons, uncapped, loop, v2 );
  }

  SharedVertex *v3 ( p->vertex3() );
  if( !v3->visited() )
  {
    visitSharedVertex( polygons, uncapped, loop, v3 );
  }

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
      //(*poly)->visited( true );
    }
  }

  //Mark the shared vertex as visited
  sv->visited( true );
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

  map.setAllUnvisited();

  Polygons polygons ( map.polygons() );

  //Walk through all the polygons
  for( typename Polygons::iterator iter = polygons.begin(); iter != polygons.end(); ++iter )
  { 
    //Return now if we need to cancel
    if( cancel() )
      return;
    
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
    }

    //Send a progress upate
    updater ( uncapped );
  }

  updater( uncapped, true );

  uncapped.sort( std::less<unsigned int>() );
  
  while( !uncapped.empty() )
  {
    typename IndexSequence::iterator i ( uncapped.begin() );

    //Create a cluster
    Loop loop;
    //loop.push_back( Loop::value_type ( *i,  );

    //Get the polygon
    Polygon p ( polygons.at( *i ) );
    
    //No longer needed, remove so we don't come back
    //uncapped.erase( i );

    //this will loop around the gap and build the proper loop
    Detail::visitPolygon( polygons, uncapped, loop, p.get() );

    loops.push_back( loop );
  }
}

}

}

#endif //__USUL_ALGORITHMS_CAP_POLYGONS_H__

