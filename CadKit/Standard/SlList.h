
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
