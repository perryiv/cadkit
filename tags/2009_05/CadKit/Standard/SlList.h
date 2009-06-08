
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlList.h: A list class with array-like access.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_LINKED_LIST_H_
#define _CADKIT_STANDARD_LIBRARY_LINKED_LIST_H_

#include <list>
#include <assert.h>


namespace CadKit {


template
<
  class Value,
  class ErrorPolicy
>
class SlList : public std::list<Value>
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Overloaded bracket operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type &operator [] ( size_type index )
  {
    assert ( 0 ); // Changed but didn't test. Take out when confident.
    _errorPolicy ( this && index >= 0 && index < this->size() );
    iterator i = std::find_if ( this->begin(), this->end(), Find ( index ) );
    _errorPolicy ( i != this->end() );
    return *i;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Overloaded bracket operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  const value_type &operator [] ( size_type index ) const
  {
    assert ( 0 ); // Changed but didn't test. Take out when confident.
    _errorPolicy ( this && index >= 0 && index < this->size() );
    const_iterator i = std::find_if ( this->begin(), this->end(), Find ( index ) );
    _errorPolicy ( i != this->end() );
    return *i;
  }


protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Small predicate class to help find the correct iterator.
  //
  /////////////////////////////////////////////////////////////////////////////

  struct Find
  {
    Find ( size_type index ) : _index ( index ), _count ( 0 ){}
    template < class T > bool operator() ( T & ) { return _index == _count++; }
  private:
    size_type _index, _count;
  };


  ErrorPolicy _errorPolicy;
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_LINKED_LIST_H_
