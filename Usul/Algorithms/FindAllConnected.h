
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



template
<
  class AdjacencyMap,
  class IndexSequence, 
  class Functor,
  class UpdateFunctor,
  class CancelFunctor
>
inline void findAllConnected ( AdjacencyMap& map, IndexSequence& answer, unsigned int selectedPolygon, UpdateFunctor& updater, const CancelFunctor& cancel )
{
  typedef typename AdjacencyMap::Polygons Polygons;
  typedef typename Polygons::value_type Polygon;
  typedef typename AdjacencyMap::SharedVertex SharedVertex;
  typedef typename IndexSequence::iterator IndexIterator;
  typedef std::vector< Functor > TodoStack;
  typedef TodoStack::iterator TodoStackItr;

  Polygons polygons ( map.polygons() );

  map.setAllUnvisited();

  //Is the selected polygon outside of _polygons' range?
  if ( selectedPolygon >= polygons.size() )
  {
    std::ostringstream message;
    message << "Error 3575595664, selected polygon is " << selectedPolygon << " but there are only " << polygons.size() << " polygons";
    throw std::runtime_error ( message.str() );
  }

  //Todo stack to simulate recursive function calls
  TodoStack todoStack;

  //Reserve enough room
  todoStack.reserve( map.size() );

  //put the functor for the selected polygon on the stack
  polygons.at( selectedPolygon ).visited ( true );
  todoStack.push_back( Functor ( answer, todoStack, &polygons.at( selectedPolygon ) ) );

  TodoStackItr todoIterator ( todoStack.begin() );

  //loop through the todo stack
  while( todoIterator != todoStack.end() )
  {
    //Do we need to stop?
    if( cancel() )
      return;

    //Do the functor's thing
    (*todoIterator)();

    //Go to the next one
    ++todoIterator;

    //Send a progress upate
    updater ( answer );
  }

  //Send a progress upate, make sure it updates
  updater ( answer, true ); 
}

}

}

#endif //__USUL_ALGORITHMS_CAP_POLYGONS_H__

