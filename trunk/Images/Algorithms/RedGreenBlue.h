
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Preey L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_ALGORITHMS_TO_RED_GREEN_BLUE_H_
#define _IMAGES_ALGORITHMS_TO_RED_GREEN_BLUE_H_

#include "usul/Exceptions/Thrower.h"

#include <algorithm>
#include <numeric>


namespace Images {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to red-green-blue-alpha.
//
///////////////////////////////////////////////////////////////////////////////

template < class C1, class C2 > 
bool toRedGreenBlue ( unsigned int channels, bool alpha, const C1 &input, C2 &output )
{
  // Handle trivial case.
  if ( input.empty() )
    return false;

  // If there is an alpha...
  if ( alpha )
  {
    // Make sure the input is grey-scale.
    if ( 2 != channels )
      return false;

    // Make space.
    const unsigned int size ( input.size() / 2 );
    output.resize ( size * 4 );

    // Loop through the input.
    for ( unsigned int i = 0; i < size; ++i )
    {
      // Set the values.
      output[i  ] = input[i];
      output[i+1] = input[i];
      output[i+2] = input[i];
      output[i+3] = input[i];
    }
  }

  // Otherwise...
  else
  {
    // Make sure the input is grey-scale.
    if ( 1 != channels )
      return false;

    // Make space.
    const unsigned int size ( input.size() );
    output.resize ( size * 3 );

    // Loop through the input.
    for ( unsigned int i = 0; i < size; ++i )
    {
      // Set the values.
      output[i  ] = input[i];
      output[i+1] = input[i];
      output[i+2] = input[i];
    }
  }

  // It worked.
  return true;
}


} // namespace Algorithms
} // namespace Images


#endif // _IMAGES_ALGORITHMS_TO_RED_GREEN_BLUE_H_
