
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
  // Handle trivial case.
  if ( 0 == num || NULL == array )
    return;

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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
  // Handle trivial case.
  if ( 0 == num || NULL == array )
    return;

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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
  // Handle trivial case.
  if ( 0 == num || NULL == array )
    return;

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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
  // Handle trivial case.
  if ( 0 == num || NULL == array )
    return;

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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
  // Handle trivial case.
  if ( 0 == num || NULL == array )
    return;

  // Get the current size.
  unsigned int size = vec.size();

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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

  // Handle trivial case.
  if ( 0 == sizeFrom )
    return;

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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

  // Handle trivial case.
  if ( 0 == sizeFrom )
    return;

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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

  // Handle trivial case.
  if ( 0 == sizeFrom )
    return;

  // Allocate space. Do not use std::vector::reserve() because we also have 
  // to make std::vector::size() return the correct number.
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
