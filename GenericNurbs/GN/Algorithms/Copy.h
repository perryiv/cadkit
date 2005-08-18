
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Copy algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_ALGORITHMS_COPY_H_
#define _GENERIC_NURBS_LIBRARY_ALGORITHMS_COPY_H_

#include <algorithm>


namespace GN {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Copy 2D array to 2D container.
//
///////////////////////////////////////////////////////////////////////////////

template < class ArrayType, class ContainerType > inline
void copy2dTo2d ( const ArrayType source,
                  typename ContainerType::size_type rows,
                  typename ContainerType::size_type columns,
                  ContainerType &target )
{
  typedef typename ContainerType::size_type SizeType;

  // Make sure target is big enough.
  target.resize ( rows );

  // Loop through rows.
  for ( SizeType i = 0; i < rows; ++i )
  {
    // Make sure this row is big enough.
    target.at(i).resize ( columns );

    // Copy the values.
    std::copy ( source[i], source[i] + columns, target.at(i).begin() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy 1D array to 2D container.
//
///////////////////////////////////////////////////////////////////////////////

template < class ArrayType, class ContainerType > inline
void copy1dTo2d ( const ArrayType source,
                  typename ContainerType::size_type rows,
                  typename ContainerType::size_type columns,
                  ContainerType &target )
{
  typedef typename ContainerType::size_type SizeType;

  // Make sure target is big enough.
  target.resize ( rows );

  // Initialize the starting point. Note: using pointer syntax was difficult 
  // with generic template parameter ArrayType. Need this to be generic to 
  // accommodate something like double array[5].
  SizeType offset ( 0 );

  // Loop through rows.
  for ( SizeType i = 0; i < rows; ++i )
  {
    // Make sure this row is big enough.
    target.at(i).resize ( columns );

    // Copy the values.
    std::copy ( &(source[offset]), &(source[offset + columns]), target.at(i).begin() );

    // Go to next row.
    offset += columns;
  }
}


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_ALGORITHMS_COPY_H_
