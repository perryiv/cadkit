
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_KEEP_ALL_CONNECTED_H__
#define __USUL_ALGORITHMS_KEEP_ALL_CONNECTED_H__


namespace Usul {
namespace Algorithms {


template
<
  class Polygons,
  class IndexSequence, 
  class Functor,
  class UpdateFunctor
>
inline void findAllConnected ( Polygons& polygons, IndexSequence& answer, unsigned int selectedPolygon, UpdateFunctor& updater, bool showProgress )
{
  typedef typename Polygons::value_type Polygon;
  typedef typename IndexSequence::iterator IndexIterator;
  typedef std::vector< Functor > TodoStack;
  typedef typename TodoStack::iterator TodoStackItr;


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
  todoStack.reserve( polygons.size() * 2 );

  //put the functor for the selected polygon on the stack
  polygons.at( selectedPolygon )->visited ( true );
  todoStack.push_back( Functor ( answer, todoStack, polygons.at( selectedPolygon ).get() ) );

  TodoStackItr todoIterator ( todoStack.begin() );

  //loop through the todo stack
  while( todoIterator != todoStack.end() )
  {
    //Do the functor's thing
    (*todoIterator)();

    //Go to the next one
    ++todoIterator;

    //Send a progress upate
    if (showProgress)
    {
        updater ( answer );
    }
  }

  //Send a progress upate, make sure it updates
  if (showProgress)
  {
    updater ( answer, true ); 
  }
}

}

}

#endif //__USUL_ALGORITHMS_KEEP_ALL_CONNECTED_H__

