
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms related to menu interaction.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_ALGORITHMS_
#define _MENUKIT_ALGORITHMS_

#include "MenuKit/Item.h"


namespace MenuKit {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Return the next item. Wrap to the beginning if necessary.
//
///////////////////////////////////////////////////////////////////////////////

template < class Iter > Item *nextItem ( Iter ii, Iter beg, Iter end )
{
  // If we are at the end then return the beginning.
  if ( end == ii )
    return beg->get();

  // Increment.
  ++ii;

  // Increment to the next non-separator that is enabled.
  while ( end != ii && (ii->get()->separator() || !(ii->get()->enabled())) )
    ++ii;

  // If we are now at the end then return the beginning.
  return ( end == ii ) ? beg->get() : ii->get();
}


}; // namespace Algorithms
}; // namespace MenuKit


#endif // _MENUKIT_ALGORITHMS_
