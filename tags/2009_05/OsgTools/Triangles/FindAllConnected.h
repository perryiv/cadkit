
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_TRIANGLES_KEEP_ALL_CONNECTED_H__
#define __OSG_TOOLS_TRIANGLES_KEEP_ALL_CONNECTED_H__

#include "Usul/Exceptions/Thrower.h"

namespace OsgTools {
namespace Triangles {


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
    
    
    //Is the selected polygon outside of polygons' range?
    if ( selectedPolygon >= polygons.size() )
    {
      Usul::Exceptions::Thrower< std::runtime_error > 
        ( "Error 3575595664, selected polygon is ", selectedPolygon, " but there are only ", polygons.size(), " polygons" );
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

#endif //__OSG_TOOLS_TRIANGLES_KEEP_ALL_CONNECTED_H__
