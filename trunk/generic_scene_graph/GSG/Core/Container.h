
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Container adaptors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CONTAINER_ADAPTORS_H_
#define _GENERIC_SCENE_GRAPH_CONTAINER_ADAPTORS_H_

#include "boost/mpl/assert_is_same.hpp"

#include <iterator>


namespace GSG {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic access to container element.
//
///////////////////////////////////////////////////////////////////////////////

template < class IteratorCategory_ > struct ElementAccess
{
  template < class Container, typename SizeType > 
  static typename Container::value_type &reference ( Container &c, SizeType &i )
  {
    BOOST_MPL_ASSERT_IS_SAME ( SizeType, BOOST_DEDUCED_TYPENAME Container::size_type );

    typename Container::iterator b = c.begin();
    std::advance ( b, i );
    return *b;
  }

  template < class Container, typename SizeType > 
  static const typename Container::value_type &constReference ( const Container &c, SizeType &i )
  {
    BOOST_MPL_ASSERT_IS_SAME ( SizeType, BOOST_DEDUCED_TYPENAME Container::size_type );

    typename Container::const_iterator b = c.begin();
    std::advance ( b, i );
    return *b;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Access to container with randon-access iterators.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct ElementAccess < std::random_access_iterator_tag >
{
  template < class Container, typename SizeType > 
  static typename Container::value_type &reference ( Container &c, SizeType &i )
  {
    BOOST_MPL_ASSERT_IS_SAME ( SizeType, BOOST_DEDUCED_TYPENAME Container::size_type );
    return c[i];
  }

  template < class Container, typename SizeType > 
  static const typename Container::value_type &constReference ( const Container &c, SizeType &i )
  {
    BOOST_MPL_ASSERT_IS_SAME ( SizeType, BOOST_DEDUCED_TYPENAME Container::size_type );
    return c[i];
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around element access.
//
///////////////////////////////////////////////////////////////////////////////

template < class Container_ > struct GetElement
{
  typedef Container_ Container;
  typedef typename Container::iterator iterator;
  typedef typename Container::const_iterator const_iterator;
  typedef typename Container::reference reference;
  typedef typename Container::const_reference const_reference;
  typedef typename Container::size_type size_type;
  typedef std::iterator_traits<iterator> Traits;
  typedef std::iterator_traits<const_iterator> ConstTraits;
  typedef typename Traits::iterator_category iterator_category;
  typedef typename ConstTraits::iterator_category const_iterator_category;

  reference operator () ( Container &c, size_type i )
  {
    typedef typename ElementAccess<iterator_category> Element;
    return Element::reference ( c, i );
  }

  const_reference operator () ( const Container &c, size_type i ) const
  {
    typedef typename ElementAccess<const_iterator_category> Element;
    return Element::constReference ( c, i );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Iterator for indexed vector pools.
//
///////////////////////////////////////////////////////////////////////////////
/*
template < class Indices_, class Vector_ > struct IndexedIterator
{
  typedef typename Indices_::size_type size_type;
  typedef typename Vector_::value_type value_type;
  typedef typename Vector_::const_reference const_reference;
  typedef typename Vector_::reference reference;

  IndexedIterator ( const Indices_ &indices, const Vector_ &vector, size_type current = 0 ) : 
    _indices ( indices ), 
    _vector  ( vector ), 
    _current ( current )
  {
    // When current == indices.size() then this is the "end" iterator.
    ErrorChecker ( _current <= _indices.size() );
  }

  IndexedIterator ( const IndexedIterator &ii ) : 
    _indices ( ii._indices ), 
    _vector  ( ii._vector ), 
    _current ( ii._current )
  {
    ErrorChecker ( _indices.empty() || ( _current < _indices.size() ) );
  }

  IndexedIterator &operator ++ ()
  {
    ++_current;
    return *this;
  }

  const IndexedIterator &operator ++ () const
  {
    ++_current;
    return *this;
  }

  IndexedIterator &operator ++ ( int )
  {
    ++_current;
    return *this;
  }

  const IndexedIterator &operator ++ ( int ) const
  {
    ++_current;
    return *this;
  }

  const_reference operator * () const
  {
    ErrorChecker ( _current < _indices.size() );
    ErrorChecker ( _indices[_current] < _vector.size() );
    return _vector[_indices[_current]];
  }

  reference operator * ()
  {
    ErrorChecker ( _current < _indices.size() );
    ErrorChecker ( _indices[_current] < _vector.size() );
    return _vector[_indices[_current]];
  }

  bool equal ( const IndexedIterator &ii ) const
  {
    return 
      (&_indices) == (&(ii._indices)) && // Same address.
      (&_vector)  == (&(ii._vector))  && // Same address.
      _current    == ii._current;        // Same location.
  }

  bool operator == ( const IndexedIterator &right ) const
  {
    return this->equal ( right );
  }

  bool operator != ( const IndexedIterator &right ) const
  {
    return ( false == this->equal ( right ) );
  }

private:

  Indices_ &_indices;
  Vector_ &_vector;
  mutable size_type _current;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor for indexed vector pools.
//
///////////////////////////////////////////////////////////////////////////////

template < class Indices_, class Vector_ > struct IndexedVectorFunctor
{
  typedef typename Indices_::size_type size_type;
  typedef typename Vector_::value_type value_type;

  IndexedVectorFunctor ( Indices_ &indices, Vector_ &vector ) : 
    _indices ( indices ), 
    _vector  ( vector )
  {
  }

  IndexedVectorFunctor ( const IndexedVectorFunctor &f ) : 
    _indices ( f._indices ), 
    _vector  ( f._vector )
  {
  }

  value_type & operator ( size_type i )
  {
    ErrorChecker ( i < _indices.size() );
    ErrorChecker ( _indices[i] < _vector.size() );
    return _vector[_indices[i]];
  }

  const value_type & operator ( size_type i ) const
  {
    ErrorChecker ( i < _indices.size() );
    ErrorChecker ( _indices[i] < _vector.size() );
    return _vector[_indices[i]];
  }

private:

  Indices_ &_indices;
  Vector_ &_vector;
};
*/

}; // namespace Detail


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for element access.
//
///////////////////////////////////////////////////////////////////////////////

template < class Container_ >
inline typename Container_::value_type &reference
  ( Container_ &c, typename Container_::size_type i )
{
  return Detail::GetElement < Container_ > () ( c, i );
}

template < class Container_ >
inline const typename Container_::value_type &constReference
  ( const Container_ &c, typename Container_::size_type i )
{
  return Detail::GetElement < Container_ > () ( c, i );
}


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CONTAINER_ADAPTORS_H_
