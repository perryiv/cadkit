
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlList.h: A two-way linked list.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TWO_WAY_LINKED_LIST_H_
#define _CADKIT_STANDARD_LIBRARY_TWO_WAY_LINKED_LIST_H_

#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
#endif


namespace CadKit
{
template<class T> class SlList : public std::list<T>
{
public:

  typedef std::list<T>::size_type Index;
  T &              operator [] ( const Index &index );
  const T &        operator [] ( const Index &index ) const;
};


//////////////////////////////////////////////////////////////////////////
//
//  Overloaded bracket operator. So the client can use the list like
//  an array.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlList<T>::operator [] ( const Index &index )
{
  SL_ASSERT ( this );
  SL_ASSERT ( index >= 0 );
  SL_ASSERT ( index < this->size() );

  // Get an iterator that points to the beginning of the list.
  std::list<T>::iterator i = this->begin();

  // Move the iterator to the correct location.
  for ( Index j = 0; j < index; ++j ) 
  {
    ++i;
  }

  // Return the value at this location.
  return (*i);
}


//////////////////////////////////////////////////////////////////////////
//
//  Overloaded bracket operator. So the client can use the list like
//  an array.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlList<T>::operator [] ( const Index &index ) const
{
  SL_ASSERT ( this );
  SL_ASSERT ( index >= 0 );
  SL_ASSERT ( index < this->size() );

  // Get an iterator that points to the beginning of the list.
  std::list<T>::iterator i = this->begin();

  // Move the iterator to the correct location.
  for ( Index j = 0; j <= index; ++j )
  {
    ++i;
  }

  // Return the value at this location.
  return (*i);
}

};

#endif
