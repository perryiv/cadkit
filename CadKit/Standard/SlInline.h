
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
  // use the bracket operator and overwrite the element we just created with 
  // the "resize()" above.
  for ( unsigned int i = 0; i < num; ++i )
    vec[size+i] = array[i];
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_MATH_FUNCTIONS_H_
