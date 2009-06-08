
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Sorting algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_ALGORITHMS_SORT_H_
#define _GENERIC_NURBS_LIBRARY_ALGORITHMS_SORT_H_

#include <iterator>
#include <algorithm>


namespace GN {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class IteratorTag > struct Sort
{
  template < class Sequence > static void ascending ( Sequence &s )
  {
    typedef std::less<typename Sequence::value_type> Compare;
    s.sort ( Compare() );
  }
  template < class Sequence > static void decending ( Sequence &s )
  {
    typedef std::greater<typename Sequence::value_type> Compare;
    s.sort ( Compare() );
  }
  template < class Sequence, class Compare > static void custom ( Sequence &s )
  {
    s.sort ( Compare() );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Containers with random-access iterators.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Sort < std::random_access_iterator_tag >
{
  template < class Sequence > static void ascending ( Sequence &s )
  {
    typedef std::less<typename Sequence::value_type> Compare;
    std::sort ( s.begin(), s.end(), Compare() );
  }
  template < class Sequence > static void decending ( Sequence &s )
  {
    typedef std::greater<typename Sequence::value_type> Compare;
    std::sort ( s.begin(), s.end(), Compare() );
  }
  template < class Sequence, class Compare > static void custom ( Sequence &s )
  {
    std::sort ( s.begin(), s.end(), Compare() );
  }
};


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_ALGORITHMS_SORT_H_
