
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_FUNCTOR_CLASSES_H_
#define _GENERIC_SCENE_GRAPH_CORE_FUNCTOR_CLASSES_H_

#include "boost/mpl/assert_is_same.hpp"

#include <limits>


namespace GSG {
namespace Detail {


/////////////////////////////////////////////////////////////////////////////
//
//  For inserting the value_type of a container into an std::map as the 
//  key_type. The map's value_type is a counter.
//
/////////////////////////////////////////////////////////////////////////////

template < class Map_ > struct MapInserter
{
  typedef typename Map_::key_type key_type;
  typedef typename Map_::value_type::second_type mapped_type;
  typedef typename Map_::value_type value_type;

  MapInserter ( Map_ &map ) : _map ( map ), _counter ( 0 )
  {
    BOOST_STATIC_ASSERT ( std::numeric_limits<mapped_type>::is_integer );
  }

  MapInserter ( const MapInserter &mi ) : _map ( mi._map ), _counter ( mi._counter )
  {
    BOOST_STATIC_ASSERT ( std::numeric_limits<mapped_type>::is_integer );
  }

  template < class Key > void operator () ( const Key &key )
  {
    BOOST_MPL_ASSERT_IS_SAME ( key_type, Key );
    _map.insert ( value_type ( key, _counter ) );
    ++_counter;
  }

protected:

  Map_ _map;
  mapped_type _counter;
};


/////////////////////////////////////////////////////////////////////////////
//
//  Functor to call Visitor::traverse().
//
/////////////////////////////////////////////////////////////////////////////

template < class Node_ > struct Traverse
{
  Traverse ( Node_ *node )       : _node ( node )   {}
  Traverse ( const Traverse &a ) : _node ( a._node ){}

  template < class Visitor_ > void operator () ( Visitor_ &v )
  {
    v->traverse ( *_node );
  }

protected:

  typename Node_::ValidPtr _node;
};


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_FUNCTOR_CLASSES_H_
