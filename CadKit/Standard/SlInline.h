
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlInline: Misc inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INLINE_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_INLINE_FUNCTIONS_H_

#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Copy the given array to the vector.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void copy ( const unsigned int &num, const T *array, std::vector<T> &vec )
{
  SL_ASSERT ( array );

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  vec.resize ( num );

  // Loop through the array and assign the elements to the vector.
  for ( unsigned int i = 0; i < num; ++i )
    vec[i] = array[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the given array to the vector.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void append ( const unsigned int &num, const T *array, std::vector<T> &vec )
{
  SL_ASSERT ( array );

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  vec.resize ( size + num );

  // Loop through the array and assign the elements to the vector.
  // Note: we cannot use vec.insert ( vec.end(), array[i] ) because that will
  // append to the end (i.e, starting at the (size+num)'th place). We have to 
  // use the bracket operator and overwrite the elements we just created with 
  // the "resize()" above.
  for ( unsigned int i = 0; i < num; ++i )
    vec[size+i] = array[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the given array to the vector of 2D vectors.
//  Note: array size is assumed to be 2 * num.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void append2D ( const unsigned int &num, const T *array, std::vector<SlVec2<T> > &vec )
{
  SL_ASSERT ( array );

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  vec.resize ( size + num );

  // Loop through the array and assign the elements to the vector.
  // Note: we cannot use vec.insert ( vec.end(), array[i] ) because that will
  // append to the end (i.e, starting at the (size+num)'th place). We have to 
  // use the bracket operator and overwrite the elements we just created with 
  // the "resize()" above.
  unsigned int j = 0;
  for ( unsigned int i = 0; i < num; ++i )
  {
    vec[size+i].setValue ( array[j], array[j+1] );
    j += 2;
  }

  // Should be true.
  SL_ASSERT ( j == num * 2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the given array to the vector of 3D vectors.
//  Note: array size is assumed to be 3 * num.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void append3D ( const unsigned int &num, const T *array, std::vector<SlVec3<T> > &vec )
{
  SL_ASSERT ( array );

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  vec.resize ( size + num );

  // Loop through the array and assign the elements to the vector.
  // Note: we cannot use vec.insert ( vec.end(), array[i] ) because that will
  // append to the end (i.e, starting at the (size+num)'th place). We have to 
  // use the bracket operator and overwrite the elements we just created with 
  // the "resize()" above.
  unsigned int j = 0;
  for ( unsigned int i = 0; i < num; ++i )
  {
    vec[size+i].setValue ( array[j], array[j+1], array[j+2] );
    j += 3;
  }

  // Should be true.
  SL_ASSERT ( j == num * 3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the given array to the vector of 4D vectors.
//  Note: array size is assumed to be 4 * num.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void append4D ( const unsigned int &num, const T *array, std::vector<SlVec4<T> > &vec )
{
  SL_ASSERT ( array );

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  vec.resize ( size + num );

  // Loop through the array and assign the elements to the vector.
  // Note: we cannot use vec.insert ( vec.end(), array[i] ) because that will
  // append to the end (i.e, starting at the (size+num)'th place). We have to 
  // use the bracket operator and overwrite the elements we just created with 
  // the "resize()" above.
  unsigned int j = 0;
  for ( unsigned int i = 0; i < num; ++i )
  {
    vec[size+i].setValue ( array[j], array[j+1], array[j+2], array[j+3] );
    j += 4;
  }

  // Should be true.
  SL_ASSERT ( j == num * 4 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the "from" vector to the end of the "to" vector.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void append2D ( const std::vector<SlVec2<T> > &from, 
                                          std::vector<T> &to )
{
  // Get the current sizes.
  unsigned int sizeFrom = from.size();
  unsigned int sizeTo = to.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  to.resize ( sizeTo + sizeFrom * 2 );

  // We start here in the "to" vector.
  unsigned int index = sizeTo;

  // Loop through and assign.
  for ( unsigned int i = 0; i < sizeFrom; ++i )
  {
    to[index] = from[i][0]; ++index;
    to[index] = from[i][1]; ++index;
  }

  // Should be true.
  SL_ASSERT ( index == to.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the "from" vector to the end of the "to" vector.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void append3D ( const std::vector<SlVec3<T> > &from, 
                                          std::vector<T> &to )
{
  // Get the current sizes.
  unsigned int sizeFrom = from.size();
  unsigned int sizeTo = to.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  to.resize ( sizeTo + sizeFrom * 3 );

  // We start here in the "to" vector.
  unsigned int index = sizeTo;

  // Loop through and assign.
  for ( unsigned int i = 0; i < sizeFrom; ++i )
  {
    to[index] = from[i][0]; ++index;
    to[index] = from[i][1]; ++index;
    to[index] = from[i][2]; ++index;
  }

  // Should be true.
  SL_ASSERT ( index == to.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the "from" vector to the end of the "to" vector.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void append4D ( const std::vector<SlVec4<T> > &from, 
                                          std::vector<T> &to )
{
  // Get the current sizes.
  unsigned int sizeFrom = from.size();
  unsigned int sizeTo = to.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() returns the correct number.
  to.resize ( sizeTo + sizeFrom * 4 );

  // We start here in the "to" vector.
  unsigned int index = sizeTo;

  // Loop through and assign.
  for ( unsigned int i = 0; i < sizeFrom; ++i )
  {
    to[index] = from[i][0]; ++index;
    to[index] = from[i][1]; ++index;
    to[index] = from[i][2]; ++index;
    to[index] = from[i][3]; ++index;
  }

  // Should be true.
  SL_ASSERT ( index == to.size() );
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_MATH_FUNCTIONS_H_
