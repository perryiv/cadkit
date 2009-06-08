
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Extraction algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_EXTRACTION_H_
#define _USUL_ALGORITHMS_EXTRACTION_H_

#include <algorithm>


namespace Usul {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Function to extract elements from container "all" and append them to 
//  container "sub", according to the given predicate.
//
///////////////////////////////////////////////////////////////////////////////

template 
<
  class Pred, 
  class Container
>
inline void extract ( const Pred &pred, Container &all, Container &sub, bool remove )
{
  // Partition the list into strings that match (the first group)
  // and strings that do not match (the second group).
  typename Container::iterator end =
    std::stable_partition ( all.begin(), all.end(), pred );

  // Copy the matching strings.
  std::copy ( all.begin(), end, std::back_inserter ( sub ) );

  // Remove the files that matched if we are supposed to. Keep this last.
  if ( remove )
    all.erase ( all.begin(), end );
}


} // namespace Algorithms
} // namespace Usul


#endif // _USUL_ALGORITHMS_EXTRACTION_H_
