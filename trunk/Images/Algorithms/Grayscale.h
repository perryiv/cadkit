
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_GRAYSCALE_H__
#define __IMAGES_GRAYSCALE_H__

#include "usul/Exceptions/Thrower.h"

#include <algorithm>
#include <numeric>


namespace Images {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to grayscale. Changes the pixel values in place, and 
//  returns the end of the valid values.
//
///////////////////////////////////////////////////////////////////////////////

template < class ValueContainer > 
typename ValueContainer::iterator toGrayScale ( ValueContainer &values, unsigned int channels, bool alpha )
{
  typedef typename ValueContainer::iterator Itr;
  typedef typename ValueContainer::value_type ValueType;

  // handle trivial case.
  if ( values.empty() )
    return values.end();

  // If there are not enough channels.
  if ( ( 0 == channels ) || ( 1 == channels ) || ( ( true == alpha ) && ( 2 == channels ) ) )
    return values.end();

  // Should be true.
  if ( 0 == ( values.size() % channels ) )
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 3161031277: number of values (", values.size(), ") is not divisible by the number of channels (", channels, ")" );

  // Iterator to the gray values.
  Itr gray ( values.begin() );

  // Number of values to average.
  unsigned int numColors ( ( alpha ) ? channels - 1 : channels );

  // This makes the compiler happy for the various data types.
  const ValueType zero  ( static_cast < ValueType > ( 0 ) );
  const ValueType three ( static_cast < ValueType > ( 3 ) );

  // Loop through the values.
  for ( Itr i = values.begin(); i != values.end(); ++i )
  {
    // Calculate the gray value.
    ValueType average ( ( std::accumulate ( i, i + numColors, zero ) ) / three );

    // Move iterator forward one less than the number of channels. 
    // The "++i" in the loop above will increment the rest of the way.
    std::advance ( i, ( channels - 1 ) );

    // Write the gray value to the container.
    (*gray) = average;

    // Increment the iterator to the gray value.
    ++gray;
  }

  // Return the new end of the gray values.
  return gray;
}


} // namespace Algorithms
} // namespace Images


#endif // __IMAGES_GRAYSCALE_H__
