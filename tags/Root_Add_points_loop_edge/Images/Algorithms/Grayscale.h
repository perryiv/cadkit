
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

template < class Channels > void grayScale ( bool hasAlpha, Channels &channels )
{
  typedef typename Channels::value_type Channel;

  // Handle trivial case.
  if ( channels.empty() )
    return;

  // Needed below.
  const unsigned int size ( channels.at(0).size() );

  // RGBA
  if ( true == hasAlpha && 4 == channels.size() )
  {
    // Get shortcuts to the channels.
    Channel &c0 ( channels.at(0) );
    Channel &c1 ( channels.at(1) );
    Channel &c2 ( channels.at(2) );
    Channel &c3 ( channels.at(3) );

    // Make sure they are the proper size.
    c0.resize ( size );
    c1.resize ( size );
    c2.resize ( size );
    c3.resize ( size );

    // Loop through the values.
    for ( unsigned int i = 0; i < size; ++i )
    {
      // Red gets average color.
      c0[i] = ( c0[i] + c1[i] + c2[i] ) / 3;

      // Assign alpha.
      c1[i] = c3[i];
    }

    // Now resize.
    channels.resize ( 2 );
  }

  // RGB
  else if ( false == hasAlpha && 3 == channels.size() )
  {
    // Get shortcuts to the channels.
    Channel &c0 ( channels.at(0) );
    Channel &c1 ( channels.at(1) );
    Channel &c2 ( channels.at(2) );

    // Make sure they are the proper size.
    c0.resize ( size );
    c1.resize ( size );
    c2.resize ( size );

    // Loop through the values.
    for ( unsigned int i = 0; i < size; ++i )
    {
      // Red gets average color.
      c0[i] = ( c0[i] + c1[i] + c2[i] ) / 3;
    }

    // Now resize.
    channels.resize ( 1 );
  }
}


} // namespace Algorithms
} // namespace Images


#endif // __IMAGES_GRAYSCALE_H__
