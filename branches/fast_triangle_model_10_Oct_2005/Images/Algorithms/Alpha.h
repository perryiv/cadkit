
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_ALGORITHMS_ALPHA_H_
#define _IMAGES_ALGORITHMS_ALPHA_H_

#include "usul/Exceptions/Thrower.h"

#include <algorithm>
#include <stdexcept>


namespace Images {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Add an alpha channel.
//
///////////////////////////////////////////////////////////////////////////////

template < class C1, class C2 > 
void addAlpha ( unsigned int inputChannels, const C1 &inputData, C2 &outputData )
{
  // Define one for this data type.
  typedef typename C1::value_type ValueType;
  const ValueType one ( static_cast < ValueType > ( 1 ) );

  // Handle trivial case.
  if ( inputData.empty() )
    return;

  // Should be true.
  if ( 0 == ( inputData.size() % inputChannels ) )
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 2258680494: number of values (", inputData.size(), ") is not divisible by the number of channels (", inputChannels, ")" );

  // Make space.
  const unsigned int size ( inputData.size() / inputChannels );
  outputData.resize ( inputData.size() + size );

  // Loop through the given channels.
  for ( unsigned int i = 0; i < inputChannels; ++i )
  {
    // Loop through the input.
    for ( unsigned int j = 0; j < size; ++j )
    {
      // The index needs to skip over the other channels.
      const unsigned int index ( i + j * inputChannels );

      // Set the values.
      outputData.at(index) = inputData.at(index);
    }
  }

  // Loop through the last channel.
  for ( unsigned int i = 0; i < size; ++i )
  {
    // The index needs to skip over the other channels.
    const unsigned int index ( i * inputChannels );

    // Set the alpha value.
    outputData.at(index) = one;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the alpha channel.
//
///////////////////////////////////////////////////////////////////////////////

template < class C1, class C2 > 
void removeAlpha ( unsigned int inputChannels, const C1 &inputData, C2 &outputData )
{
  // Handle trivial case.
  if ( inputData.empty() )
    return;

  // Should be true.
  if ( 0 == ( inputData.size() % inputChannels ) )
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 1405321459: number of values (", inputData.size(), ") is not divisible by the number of channels (", inputChannels, ")" );

  // Make space.
  const unsigned int size ( inputData.size() / inputChannels );
  outputData.resize ( inputData.size() - size );

  // Loop through the output channels.
  const unsigned int outputChannels ( inputChannels - 1 );
  for ( unsigned int i = 0; i < outputChannels; ++i )
  {
    // Loop through the input.
    for ( unsigned int j = 0; j < size; ++j )
    {
      // The index needs to skip over the appropriate channels.
      const unsigned int ii ( i + j * inputChannels  );
      const unsigned int oi ( i + j * outputChannels );

      // Set the values.
      outputData.at(oi) = inputData.at(ii);
    }
  }
}


} // namespace Algorithms
} // namespace Images


#endif // _IMAGES_ALGORITHMS_ALPHA_H_
