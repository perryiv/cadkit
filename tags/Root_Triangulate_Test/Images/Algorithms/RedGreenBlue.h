
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_ALGORITHMS_TO_RED_GREEN_BLUE_H_
#define _IMAGES_ALGORITHMS_TO_RED_GREEN_BLUE_H_

#include <algorithm>
#include <numeric>


namespace Images {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to red-green-blue.
//
///////////////////////////////////////////////////////////////////////////////

template < class Channels > void redGreenBlue ( bool hasAlpha, Channels &channels )
{
  typedef typename Channels::value_type Channel;
  typedef typename Channel::value_type ValueType;

  // Handle trivial case.
  if ( channels.empty() )
    return;

  // Needed below.
  const unsigned int size ( channels.at(0).size() );

  // If there is an alpha...
  if ( hasAlpha )
  {
    // Make the proper number of channels.
    channels.resize ( 4 );

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
      // Grab alpha before we write over it.
      ValueType a ( c1[i] );

      // Green and blue get red's value.
      c1[i] = c0[i];
      c2[i] = c0[i];

      // Assign alpha.
      c1[i] = a;
    }
  }

  // Otherwise...
  else
  {
    // Make the proper number of channels.
    channels.resize ( 3 );

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
      // Green and blue get red's value.
      c1[i] = c0[i];
      c2[i] = c0[i];
    }
  }
}


} // namespace Algorithms
} // namespace Images


#endif // _IMAGES_ALGORITHMS_TO_RED_GREEN_BLUE_H_
