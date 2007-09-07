
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Similar to std::vector, but uses less memory when empty.
//  Note: this class has not been tested... TODO
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_CONTAINERS_DYNAMIC_ARRAY_CLASS_H_
#define _USUL_CONTAINERS_DYNAMIC_ARRAY_CLASS_H_

#include "Usul/Errors/Assert.h"

#include <iterator>
#include <limits>
#include <sstream>


namespace Usul {
namespace Containers {


template
<
  class ValueType_,
  class SizeType_
>
class DynamicArray
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ValueType_                                  value_type;
  typedef value_type *                                iterator;
  typedef const value_type *                          const_iterator;
  typedef value_type &                                reference;
  typedef const value_type &                          const_reference;
  typedef SizeType_                                   size_type;
  typedef DynamicArray < value_type, size_type >      this_type;
  typedef std::reverse_iterator < iterator >          reverse_iterator;
  typedef std::reverse_iterator < const_iterator >    const_reverse_iterator;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Resize the array.
  //
  /////////////////////////////////////////////////////////////////////////////

  void resize ( size_type num )
  {
    // Handle trivial case.
    if ( this->size() == num )
      return;

    // Make a new array.
    value_type *a ( new value_type[num] );

    // Safely copy.
    try
    {
      // If we need to grow...
      if ( this->size() < num )
      {
        // Copy existing data.
        std::copy ( this->begin(), this->end(), a );

        // Fill the rest with default values.
        std::fill ( a + this->size(), a + num, value_type() );
      }

      // Otherwise, do we we need to shrink?
      else if ( this->size() > num )
      {
        // Copy existing data that fits.
        std::copy ( this->begin(), this->begin() + num, a );
      }
    }

    // Catch all exceptions.
    catch ( ... )
    {
      delete [] a;
      throw;
    }

    // Set our members.
    delete [] _a;
    _a = a;
    _size = num;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class T, class S > this_type &operator = ( const DynamicArray<T,S> &a )
  {
    this->resize ( a.size() );
    std::copy ( a.begin(), a.end(), this->begin() );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty() const
  {
    return ( 0 == this->size() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the array.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    return _size;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the maximum size of the array.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type max_size() const
  {
    return std::numeric_limits<size_type>::max();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator begin()
  {
    return _a;
  }
  const_iterator begin() const
  {
    return _a;
  }
  iterator end()
  {
    return _a + this->size();
  }
  const_iterator end() const
  {
    return _a + this->size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Reverse iterators.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator rbegin()
  {
    return reverse_iterator ( this->end() );
  }
  const_iterator rbegin() const
  {
    return const_reverse_iterator ( this->end() );
  }
  iterator rend()
  {
    return reverse_iterator ( this->begin() );
  }
  const_iterator rend() const
  {
    return const_reverse_iterator ( this->begin() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Random access.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference at ( size_type i )
  {
    // Return value if in range.
    if ( i < this->size() )
      return _a[i];

    // Report error.
    std::ostringstream out;
    out << "Error 3153444231: index " << i << " is out of range";
    throw std::range_error ( out.str() );
  }
  const_reference at ( size_type i ) const
  {
    // Return value if in range.
    if ( i < this->size() )
      return _a[i];

    // Report error.
    std::ostringstream out;
    out << "Error 3936187790: index " << i << " is out of range";
    throw std::range_error ( out.str() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  DynamicArray syntax.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference operator [] ( size_type i )
  {
    return this->at ( i );
  }
  const_reference operator [] ( size_type i ) const
  {
    return this->at ( i );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access first element.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference front()
  {
    return this->at ( 0 );
  }
  const_reference front() const
  {
    return this->at ( 0 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access last element.
  //
  /////////////////////////////////////////////////////////////////////////////

  reference back()
  {
    return this->at ( this->size() - 1 );
  }
  const_reference back() const
  {
    return this->at ( this->size() - 1 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if the vectors are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const this_type &a ) const
  {
    return ( std::equal ( this->begin(), this->end(), a.begin() ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assign this array.
  //
  /////////////////////////////////////////////////////////////////////////////

  void assign ( const value_type &value )
  {
    std::fill ( this->begin(), this->end(), value );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Erase the element. Returns "end" if not found.
  //
  /////////////////////////////////////////////////////////////////////////////

  iterator erase ( iterator where )
  {
    // Handle trivial case.
    if ( this->end() == where )
      return where;

    // Check input.
    if ( this->empty() || where < this->begin() || where > this->end() )
      throw std::range_error ( "Error 2498164710: iterator is out of range" );

    // Calculate new size.
    const size_type oldSize ( this->size() );
    const size_type newSize ( oldSize - 1 );

    // Handle trivial case.
    if ( 0 == newSize )
    {
      delete [] _a;
      _a = 0x0;
      _size = 0;
      return this->end();
    }

    // Make a new array.
    value_type *a ( new value_type[newSize] );

    // Calculate the position to be erased.
    const size_type pos ( where - this->begin() );

    // Safely copy.
    try
    {
      // Copy existing data.
      value_type *last ( std::copy ( this->begin(), where, a ) );
      last = std::copy ( where + 1, this->end(), last );

      // Check logic.
      USUL_ASSERT ( a + newSize == last );
    }

    // Catch all exceptions.
    catch ( ... )
    {
      delete [] a;
      throw;
    }

    // Set our members.
    delete [] _a;
    _a = a;
    _size = newSize;

    // Return iterator one past erased element (same as std::vector).
    return ( this->begin() + pos );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push an element onto the end.
  //
  /////////////////////////////////////////////////////////////////////////////

  void push_back ( const value_type &value )
  {
    const oldSize ( this->size() );
    this->resize ( oldSize + 1 );
    this->at(oldSize) = value;
  }


private:

  value_type *_a;
  size_type _size;
};


} // namespace Containers
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Equality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class S > inline bool operator == 
  ( const Usul::Containers::DynamicArray<T,S> &c1, const Usul::Containers::DynamicArray<T,S> &c2 )
{
  return c1.equal ( c2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality operator. Cannot be a member.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class S > inline bool operator != 
  ( const Usul::Containers::DynamicArray<T,S> &c1, const Usul::Containers::DynamicArray<T,S> &c2 )
{
  return !( c1.equal ( c2 ) );
}


#endif // _USUL_CONTAINERS_DYNAMIC_ARRAY_CLASS_H_
