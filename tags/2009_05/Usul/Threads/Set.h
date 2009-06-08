
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A set class that is guarded with a mutex.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_GUARDED_SET_CLASSES_H_
#define _USUL_THREADS_GUARDED_SET_CLASSES_H_


namespace Usul {
namespace Threads {


template < class Config_ > class Set
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
  typedef typename Config::set_type set_type;
  typedef typename set_type::key_type key_type;
  typedef typename set_type::value_type value_type;
  typedef typename set_type::key_compare key_compare;
  typedef typename set_type::size_type size_type;
  typedef typename set_type::pointer pointer;
  typedef typename set_type::const_pointer const_pointer;
  typedef typename set_type::reference reference;
  typedef typename set_type::const_reference const_reference;
  typedef typename set_type::iterator iterator;
  typedef typename set_type::const_iterator const_iterator;
  typedef typename set_type::reverse_iterator reverse_iterator;
  typedef typename set_type::const_reverse_iterator const_reverse_iterator;
  typedef typename set_type::difference_type difference_type;
  typedef typename set_type::allocator_type allocator_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors/destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Set() : _mutex ( mutex_type::create() ), _set()
  {
  }
  ~Set()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty() const
  {
    guard_type guard ( *_mutex );
    return _set.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the set.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    guard_type guard ( *_mutex );
    return _set.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the maximum size of the set.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type max_size() const
  {
    guard_type guard ( *_mutex );
    return _set.max_size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterator access.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator begin()
  {
    guard_type guard ( *_mutex );
    return _set.begin();
  }
  const_iterator begin() const
  {
    guard_type guard ( *_mutex );
    return _set.begin();
  }
  iterator end()
  {
    guard_type guard ( *_mutex );
    return _set.end();
  }
  const_iterator end() const
  {
    guard_type guard ( *_mutex );
    return _set.end();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reverse-iterator access.
  //
  /////////////////////////////////////////////////////////////////////////////

  reverse_iterator rbegin()
  {
    guard_type guard ( *_mutex );
    return _set.rbegin();
  }
  const_reverse_iterator rbegin() const
  {
    guard_type guard ( *_mutex );
    return _set.rbegin();
  }
  reverse_iterator rend()
  {
    guard_type guard ( *_mutex );
    return _set.rend();
  }
  const_reverse_iterator rend() const
  {
    guard_type guard ( *_mutex );
    return _set.rend();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear the set.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    guard_type guard ( *_mutex );
    return _set.clear();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Insert.
  //
  /////////////////////////////////////////////////////////////////////////////

  std::pair<iterator,bool> insert ( const value_type &x )
  {
    guard_type guard ( *_mutex );
    return _set.insert ( x );
  }
  iterator insert ( iterator position, const value_type &x )
  {
    guard_type guard ( *_mutex );
    return _set.insert ( position, x );
  }
  template <class II> void insert ( II first, II last)
  {
    guard_type guard ( *_mutex );
    return _set.insert ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Erase.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator erase ( iterator position )
  {
    guard_type guard ( *_mutex );
    return _set.erase ( position );
  }
  size_type erase ( const key_type &key )
  {
    guard_type guard ( *_mutex );
    return _set.erase ( key );
  }
  iterator erase ( iterator first, iterator last )
  {
    guard_type guard ( *_mutex );
    return _set.erase ( first, last );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Find.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator find ( const key_type &key )
  {
    guard_type guard ( *_mutex );
    return _set.find ( key );
  }
  const_iterator find ( const key_type &key ) const
  {
    guard_type guard ( *_mutex );
    return _set.find ( key );
  }


private:

  // No copying or assignment.
  Set ( const Set & );
  Set &operator = ( const Set & );

  mutex_type *_mutex;
  set_type _set;
};


} // namespace Threads
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring config structs.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_SET_CONFIG(struct_name,the_mutex_type,the_guard_type,the_set_type) \
struct struct_name \
{ \
  typedef the_mutex_type mutex_type; \
  typedef the_guard_type guard_type; \
  typedef the_set_type set_type; \
} \



#endif // _USUL_THREADS_GUARDED_SET_CLASSES_H_
