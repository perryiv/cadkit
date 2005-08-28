
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms to fill containers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_ALGORITHMS_FILL_H_
#define _GENERIC_NURBS_LIBRARY_ALGORITHMS_FILL_H_


namespace GN {
namespace Algorithms {


/////////////////////////////////////////////////////////////////////////////
//
//  Fill the container.
//
/////////////////////////////////////////////////////////////////////////////

template < class ContainerType > inline void fill
( 
  ContainerType &c, 
  typename ContainerType::value_type minimum, 
  typename ContainerType::value_type maximum
)
{
  typedef typename ContainerType::value_type value_type;
  typedef typename ContainerType::size_type size_type;
  typedef typename ContainerType::iterator iterator;

  // Handle bad input.
  if ( c.empty() )
    return;

  // Get the size.
  const size_type num ( c.size() );

  // Handle values at either end.
  if ( 1 <= num )
  {
    c.front() = minimum;
    if ( 1 == num )
      return;
  }
  if ( 2 <= num )
  {
    c.back() = maximum;
    if ( 2 == num )
      return;
  }
  if ( 3 == num )
  {
    *(++c.begin()) = ( maximum - minimum ) * value_type ( 0.5 );
    return;
  }

  // Get the iterators.
  iterator first = c.begin();
  iterator last = c.end();
  ++first;
  --last;

  // Needed in the loop.
  const value_type denom ( num - 1 );
  const value_type invDenom ( value_type ( 1 ) / denom );
  const value_type range ( maximum - minimum );
  value_type loop;
  size_type count ( 1 );

  // Loop through the interior.
  for ( iterator i = first; i != last; ++i )
  {
    loop = count++;
    loop = minimum + range * loop * invDenom;
    (*i) = loop;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Fill the container.
//
/////////////////////////////////////////////////////////////////////////////

template < class ContainerType > inline void fill
( 
  ContainerType &c, 
  typename ContainerType::size_type num,
  typename ContainerType::value_type minimum, 
  typename ContainerType::value_type maximum
)
{
  c.resize ( num );
  GN::Algorithms::fill ( c, minimum, maximum );
}



}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_ALGORITHMS_FILL_H_
