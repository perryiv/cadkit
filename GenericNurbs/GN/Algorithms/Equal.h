
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Equality algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_EQUALITY_ALGORITHMS_CLASS_H_
#define _GENERIC_NURBS_LIBRARY_EQUALITY_ALGORITHMS_CLASS_H_

#include <algorithm>


namespace GN {
namespace Algorithms {
namespace Equal {


///////////////////////////////////////////////////////////////////////////////
//
//  See if the floating-point data contained in the nested containers are 
//  equal. Container types are assumed to be something like:
//
//    std::vector < std::vector < double > >
//
//  Note: The predicate is passed by value because Cygwin (and perhaps gcc 
//  in general) can not find this function otherwise.
//
///////////////////////////////////////////////////////////////////////////////

template < class CT, class Pred > inline
bool nestedContainers ( const CT &c1, const CT &c2, Pred pred )
{
  typedef typename CT::size_type size_type;

  // Unnesting the containers to make Cygwin happy.
  if ( c1.size() != c2.size() )
    return false;

  // Test each internal container.
  for ( size_type i = 0; i < c1.size(); ++i )
  {
    if ( c1[i].size() != c2[i].size() )
      return false;
    if ( false == std::equal ( c1[i].begin(), c1[i].end(), c2[i].begin(), pred ) )
      return false;
  }

  // If we get this far then they are equal.
  return true;
}


}; // namespace Equal
}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_EQUALITY_ALGORITHMS_CLASS_H_
