
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


namespace Usul {
namespace Threads {


template < class Config_ > class Map
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Config_ Config;
  typedef typename Config::mutex_type mutex_type;
  typedef typename Config::guard_type guard_type;
  typedef typename Config::map_type map_type;
  typedef typename map_type::key_type key_type;
  typedef typename map_type::mapped_type mapped_type;
  typedef typename map_type::value_type value_type;
  typedef typename map_type::key_compare key_compare;
  typedef typename map_type::size_type size_type;
  typedef typename map_type::pointer pointer;
  typedef typename map_type::const_pointer const_pointer;
  typedef typename map_type::reference reference;
  typedef typename map_type::const_reference const_reference;
  typedef typename map_type::iterator iterator;
  typedef typename map_type::const_iterator const_iterator;
  typedef typename map_type::reverse_iterator reverse_iterator;
  typedef typename map_type::const_reverse_iterator const_reverse_iterator;
  typedef typename map_type::size_type size_type;
  typedef typename map_type::difference_type difference_type;
  typedef typename map_type::allocator_type allocator_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors/destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Map() : _mutex ( mutex_type::create() ), _map()
  {
  }
  Map ( const Map &m ) : _mutex ( mutex_type::create() ), _map ( m._map )
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

  Map &operator = ( const Map & )
  {
    guard_type guard ( *_mutex );
    _map = m._map;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty()
  {
    guard_type guard ( *_mutex );
    return _map.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the map.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    guard_type guard ( *_mutex );
    return _map.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the maximum size of the map.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type max_size() const
  {
    guard_type guard ( *_mutex );
    return _map.max_size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterator access.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator begin()
  {
    guard_type guard ( *_mutex );
    return _map.begin();
  }
  const_iterator begin() const
  {
    guard_type guard ( *_mutex );
    return _map.begin();
  }
  iterator end()
  {
    guard_type guard ( *_mutex );
    return _map.end();
  }
  const_iterator end() const
  {
    guard_type guard ( *_mutex );
    return _map.end();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reverse-iterator access.
  //
  /////////////////////////////////////////////////////////////////////////////

  reverse_iterator rbegin()
  {
    guard_type guard ( *_mutex );
    return _map.rbegin();
  }
  const_reverse_iterator rbegin() const
  {
    guard_type guard ( *_mutex );
    return _map.rbegin();
  }
  reverse_iterator rend()
  {
    guard_type guard ( *_mutex );
    return _map.rend();
  }
  const_reverse_iterator rend() const
  {
    guard_type guard ( *_mutex );
    return _map.rend();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Bracket operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  mapped_type &operator [] ( const key_type &key )
  {
    guard_type guard ( *_mutex );
    return _map[key];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear the map.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    guard_type guard ( *_mutex );
    return _map.clear();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Insert.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::pair<iterator,bool> insert ( const value_type &x )
  {
    guard_type guard ( *_mutex );
    return _map.insert ( x );
  }
  iterator insert ( iterator position, const value_type &x )
  {
    guard_type guard ( *_mutex );
    return _map.insert ( position, x );
  }
  template <class II> void insert ( II first, II last)
  {
    guard_type guard ( *_mutex );
    return _map.insert ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Erase.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator erase ( iterator position )
  {
    guard_type guard ( *_mutex );
    return _map.erase ( position );
  }
  size_type erase ( const key_type &key )
  {
    guard_type guard ( *_mutex );
    return _map.erase ( key );
  }
  iterator erase ( iterator first, iterator last )
  {
    guard_type guard ( *_mutex );
    return _map.erase ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Find.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator find ( const key_type &key )
  {
    guard_type guard ( *_mutex );
    return _map.find ( key );
  }
  const_iterator find ( const key_type &key ) const
  {
    guard_type guard ( *_mutex );
    return _map.find ( key );
  }


private:

  mutex_type *_mutex;
  map_type _map;
};


}; // namespace Threads
}; // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring config structs.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_MAP_CONFIG(struct_name,the_mutex_type,the_guard_type,the_map_type) \
struct struct_name \
{ \
  typedef the_mutex_type mutex_type; \
  typedef the_guard_type guard_type; \
  typedef the_map_type map_type; \
}



#endif // _USUL_THREADS_GUARDED_MAP_CLASSES_H_
