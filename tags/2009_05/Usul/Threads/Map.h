
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A map class that is guarded with a mutex.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_GUARDED_MAP_CLASSES_H_
#define _USUL_THREADS_GUARDED_MAP_CLASSES_H_

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <map>


namespace Usul {
namespace Threads {


template
<
  class KeyType,
	class MappedType,
  class PredicateType = std::less < KeyType >,
  class AllocatorType = std::allocator < std::pair < const KeyType, MappedType > >
>
class Map
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;
  typedef std::map < KeyType, MappedType, PredicateType, AllocatorType > MapType;
  typedef typename MapType::key_type key_type;
  typedef typename MapType::mapped_type mapped_type;
  typedef typename MapType::value_type value_type;
  typedef typename MapType::key_compare key_compare;
  typedef typename MapType::size_type size_type;
  typedef typename MapType::pointer pointer;
  typedef typename MapType::const_pointer const_pointer;
  typedef typename MapType::reference reference;
  typedef typename MapType::const_reference const_reference;
  typedef typename MapType::iterator iterator;
  typedef typename MapType::const_iterator const_iterator;
  typedef typename MapType::reverse_iterator reverse_iterator;
  typedef typename MapType::const_reverse_iterator const_reverse_iterator;
  typedef typename MapType::size_type size_type;
  typedef typename MapType::difference_type difference_type;
  typedef typename MapType::allocator_type allocator_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors/destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Map() : _mutex(), _map()
  {
  }
  Map ( const Map &m ) : _mutex(), _map ( m._map )
  {
  }
  ~Map()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  Map &operator = ( const Map & m )
  {
    Guard guard ( _mutex );
    _map = m._map;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty()
  {
    Guard guard ( _mutex );
    return _map.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the map.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    Guard guard ( _mutex );
    return _map.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the maximum size of the map.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type max_size() const
  {
    Guard guard ( _mutex );
    return _map.max_size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterator access.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator begin()
  {
    Guard guard ( _mutex );
    return _map.begin();
  }
  const_iterator begin() const
  {
    Guard guard ( _mutex );
    return _map.begin();
  }
  iterator end()
  {
    Guard guard ( _mutex );
    return _map.end();
  }
  const_iterator end() const
  {
    Guard guard ( _mutex );
    return _map.end();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reverse-iterator access.
  //
  /////////////////////////////////////////////////////////////////////////////

  reverse_iterator rbegin()
  {
    Guard guard ( _mutex );
    return _map.rbegin();
  }
  const_reverse_iterator rbegin() const
  {
    Guard guard ( _mutex );
    return _map.rbegin();
  }
  reverse_iterator rend()
  {
    Guard guard ( _mutex );
    return _map.rend();
  }
  const_reverse_iterator rend() const
  {
    Guard guard ( _mutex );
    return _map.rend();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Bracket operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  mapped_type &operator [] ( const key_type &key )
  {
    Guard guard ( _mutex );
    return _map[key];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear the map.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    Guard guard ( _mutex );
    return _map.clear();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Insert.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::pair<iterator,bool> insert ( const value_type &x )
  {
    Guard guard ( _mutex );
    return _map.insert ( x );
  }
  iterator insert ( iterator position, const value_type &x )
  {
    Guard guard ( _mutex );
    return _map.insert ( position, x );
  }
  template <class II> void insert ( II first, II last)
  {
    Guard guard ( _mutex );
    return _map.insert ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Erase.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator erase ( iterator position )
  {
    Guard guard ( _mutex );
    return _map.erase ( position );
  }
  size_type erase ( const key_type &key )
  {
    Guard guard ( _mutex );
    return _map.erase ( key );
  }
  iterator erase ( iterator first, iterator last )
  {
    Guard guard ( _mutex );
    return _map.erase ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Find.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator find ( const key_type &key )
  {
    Guard guard ( _mutex );
    return _map.find ( key );
  }
  const_iterator find ( const key_type &key ) const
  {
    Guard guard ( _mutex );
    return _map.find ( key );
  }


private:

  Mutex _mutex;
  MapType _map;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_GUARDED_MAP_CLASSES_H_
