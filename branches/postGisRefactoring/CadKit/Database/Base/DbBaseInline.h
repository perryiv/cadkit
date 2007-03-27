
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseInline: Inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_INLINE_FUNCTIONS_DATABASE_H_
#define _CADKIT_DATABASE_BASE_INLINE_FUNCTIONS_DATABASE_H_

#include "Standard/SlFindExtreme.h"

#include <map>
#include <algorithm>


namespace CadKit {
namespace Utility {


///////////////////////////////////////////////////////////////////////////////
//
//  Find the data associated with the key.
//
///////////////////////////////////////////////////////////////////////////////

template <class KeyType, class MapType> 
inline const void *find ( const KeyType key, const MapType &theMap )
{
  // See if it's in our map.
  typename MapType::const_iterator i = theMap.find ( key );

  // Return the result (which may be null).
  return ( i != theMap.end() ) ? i->second : NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the width that the ascii columns need to be.
//
///////////////////////////////////////////////////////////////////////////////

template < typename Scalar > struct CalculateWidth
{
  template < class Container > unsigned int operator () ( const Container &values ) const
  {
    // Find the max and min scalar number in the list of triangles.
    SlFindMax<Scalar> max = std::for_each ( values.begin(), values.end(), SlFindMax<Scalar>() );
    SlFindMin<Scalar> min = std::for_each ( values.begin(), values.end(), SlFindMin<Scalar>() );

    // Take the max of the absolute value.
    Scalar extreme = std::max ( CadKit::abs ( max.getMax() ), CadKit::abs ( min.getMin() ) );

    // Initialize the width.
    unsigned int width ( 4 );

    // How many times can we divide by 10?
    while ( extreme > 10 )
    {
      extreme /= 10.0f;
      ++width;
    }

    // Return the width.
    return width;
  }
};


}; // namespace Utility
}; // namespace CadKit


#endif // _CADKIT_DATABASE_BASE_INLINE_FUNCTIONS_DATABASE_H_
