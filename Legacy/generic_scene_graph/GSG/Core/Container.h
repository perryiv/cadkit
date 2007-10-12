
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
