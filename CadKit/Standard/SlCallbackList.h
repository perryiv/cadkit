
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
//  SlCallbackList.h: A generic callback list.
//  
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_CALLBACK_LIST_H_
#define _CADKIT_STANDARD_LIBRARY_CALLBACK_LIST_H_

#include "SlCallback.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif


namespace CadKit
{
template<class FT, class DT, class CT> class SlCallbackList : public std::vector<SlCallback<FT,DT,CT> >
{
public:

  typedef std::vector::size_type SizeType;

  void                          add ( const SlCallback<FT,DT,CT> &callback );
  void                          add ( const FT &function, const DT &clientData, const CT &callData );

  SizeType                      getCount ( const FT &function ) const;
  SizeType                      getCount ( const FT &function, const DT &clientData ) const;

  bool                          has ( const FT &function ) const;
  bool                          has ( const FT &function, const DT &clientData ) const;

  SlCallback<FT,DT,CT> &        operator [] ( SizeType i );
  const SlCallback<FT,DT,CT> &  operator [] ( SizeType i ) const;

  SizeType                      removeAll ( const FT &function );
  SizeType                      removeAll ( const FT &function, const DT &clientData );

  bool                          removeFirst ( const FT &function );
  bool                          removeFirst ( const FT &function, const DT &clientData );
};


//////////////////////////////////////////////////////////////////////////
//
//  Add a callback to the list.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline void SlCallbackList<FT,DT,CT>::add ( const SlCallback<FT,DT,CT> &callback )
{
  this->push_back ( callback );
}


//////////////////////////////////////////////////////////////////////////
//
//  Add a callback to the list.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline void SlCallbackList<FT,DT,CT>::add ( const FT &function, const DT &clientData, const CT &callData )
{
  SL_ASSERT ( function ); // No NULL function pointers.
  this->push_back ( SlCallback<FT,DT,CT> ( function, clientData ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the i'th callback.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline const SlCallback<FT,DT,CT> &SlCallbackList<FT,DT,CT>::operator [] ( SizeType i ) const
{
  SL_ASSERT ( i < this->size() );
  return ( *(this->begin() + i ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the i'th callback.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline SlCallback<FT,DT,CT> &SlCallbackList<FT,DT,CT>::operator [] ( SizeType i )
{
  SL_ASSERT ( i < this->size() );
  return ( *(this->begin() + i ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the number of times this function is in the list 
//  (ignores clientData).
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::getCount ( const FT &function ) const
{
  SizeType count = 0;

  for ( std::vector<SlCallback<FT,DT,CT> >::const_iterator i = this->begin(); i != this->end(); ++i )
  {
    if ( i->getFunction() == function ) 
      ++count;
  }

  return count;
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the number of times this function and clientData are in 
//  the list as a pair.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::getCount ( const FT &function, const DT &clientData ) const
{
  SizeType count = 0;

  for ( std::vector<SlCallback<FT,DT,CT> >::const_iterator i = this->begin(); i != this->end(); ++i )
  {
    if ( i->getFunction() == function && i->getClientData() == clientData ) 
      ++count;
  }

  return count;
}


//////////////////////////////////////////////////////////////////////////
//
//  See if this function is in the list (ignores clientData).
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline bool SlCallbackList<FT,DT,CT>::has ( const FT &function ) const
{
  for ( std::vector<SlCallback<FT,DT,CT> >::const_iterator i = this->begin(); i != this->end(); ++i )
  {
    if ( i->getFunction() == function ) 
      return true;
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////
//
//  See if this function and clientData are in the list as a pair.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline bool SlCallbackList<FT,DT,CT>::has ( const FT &function, const DT &clientData ) const
{
  for ( std::vector<SlCallback<FT,DT,CT> >::const_iterator i = this->begin(); i != this->end(); ++i )
  {
    if ( i->getFunction() == function && i->getClientData() == clientData ) 
      return true;
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove all occurrences of this function from the list.
//  Returns the number of occurrences.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::removeAll ( const FT &function )
{
  SizeType count = 0;
  std::vector<SlCallback<FT,DT,CT> >::iterator i = this->begin();

  // A for-loop is not used here because we need to increment i iff the 
  // i'th element is not erased because erase() returns the next element.
  while ( i != this->end() )
  {
    if ( i->getFunction() == function )
    {
      i = this->erase ( i );
      ++count;
    }

    else 
    {
      ++i;
    }
  }

  return count;
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove all occurrences of this pair from the list.
//  Returns the number of occurrences.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::removeAll ( const FT &function, const DT &clientData )
{
  SizeType count = 0;
  std::vector<SlCallback<FT,DT,CT> >::iterator i = this->begin();

  // A for-loop is not used here because we need to increment i iff the 
  // i'th element is not erased because erase() returns the next element.
  while ( i != this->end() )
  {
    if ( i->getFunction() == function && i->getClientData() == clientData )
    {
      i = this->erase ( i );
      ++count;
    }

    else 
    {
      ++i;
    }
  }

  return count;
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove the first occurrences of this callback from the list. 
//  Returns true if one was found.
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline bool SlCallbackList<FT,DT,CT>::removeFirst ( const FT &function )
{
  for ( std::vector<SlCallback<FT,DT,CT> >::iterator i = this->begin(); i != this->end(); ++i )
  {
    if ( i->getFunction() == function )
    {
      this->erase ( i );
      return true;
    }
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove the first occurrences of this callback from the list. 
//  Returns true if one was found..
//
//////////////////////////////////////////////////////////////////////////

template<class FT, class DT, class CT> inline bool SlCallbackList<FT,DT,CT>::removeFirst ( const FT &function, const DT &clientData )
{
  for ( std::vector<SlCallback<FT,DT,CT> >::iterator i = this->begin(); i != this->end(); ++i )
  {
    if ( i->getFunction() == function && i->getClientData() == clientData )
    {
      this->erase ( i );
      return true;
    }
  }

  return false;
}


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_CALLBACK_LIST_H_
