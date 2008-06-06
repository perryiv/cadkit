
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

  typedef typename std::vector<SlCallback<FT,DT,CT> >::size_type SizeType;

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

template<class FT, class DT, class CT> inline typename SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::getCount ( const FT &function ) const
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

template<class FT, class DT, class CT> inline typename SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::getCount ( const FT &function, const DT &clientData ) const
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

template<class FT, class DT, class CT> inline typename SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::removeAll ( const FT &function )
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

template<class FT, class DT, class CT> inline typename SlCallbackList<FT,DT,CT>::SizeType SlCallbackList<FT,DT,CT>::removeAll ( const FT &function, const DT &clientData )
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
