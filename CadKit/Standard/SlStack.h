
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
//  SlStack: A stack class.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_STACK_H_
#define _CADKIT_STANDARD_LIBRARY_STACK_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stack>
#endif

#ifdef _WIN32
# define _sequence c
#elif __GNUC__
# define _sequence _M_c
#else
TODO
#endif


namespace CadKit
{
template<class T> class SlStack : public std::stack<T>
{
public:

  SlStack();
  ~SlStack();

  //  Return the bottom of the stack.
  T &                     bottom()        { SL_ASSERT ( _sequence.size() > 0 ); return ( _sequence.front() ); }
  const T &               bottom() const  { SL_ASSERT ( _sequence.size() > 0 ); return ( _sequence.front() ); }

  //  Clear the stack.
  void                    clear() { _sequence.clear(); }

  //  Access to the internal deque.
  const std::deque<T> &   deque() const { return _sequence; }
  std::deque<T> &         deque()       { return _sequence; }

  //  Is "val" anywhere on the stack?
  bool                    isOnStack ( const T &val ) const;

  //  Return the i'th element on the stack. 0 is the bottom of the stack.
  T &                     operator [] ( SlInt32 i )       { SL_ASSERT ( i >= 0 && i < _sequence.size() ); return _sequence[i]; }
  const T &               operator [] ( SlInt32 i ) const { SL_ASSERT ( i >= 0 && i < _sequence.size() ); return _sequence[i]; }

  //  Pop the top element off of the stack. This does the same thing as 
  //  std::stack::pop() with the exception of the assert.
  void                    pop() { SL_ASSERT ( _sequence.size() > 0 ); _sequence.pop_back(); }

  //  Push the top element onto the stack one more time. There has to be at 
  //  least one element already on the stack.
  void                    push() { SL_ASSERT ( _sequence.size() > 0 ); _sequence.push_back ( _sequence.back() ); }

  //  Push the given element onto the stack. This does the same 
  //  thing as std::push ( const T &val ) but the above CadKit::SlStack::push() 
  //  hides the base class's function. This is here to please VC++.
  void                    push ( const T &val );

  //  Remove the first occurance of "val" from the stack, no matter where it 
  //  is. Return true if "val" was found and removed.
  bool                    remove ( const T &val );

  //  Remove all occurances of "val" from the stack, Return the number of 
  //  times "val" was removed.
  SlInt32                 removeAll ( const T &val );

  //  Return the top of the stack. These do the same thing as std::stack::top() 
  //  with the exception of the assert.
  T &                     top();
  const T &               top() const;
};


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlStack<T>::SlStack()
{
  // Empty.
}


//////////////////////////////////////////////////////////////////////////
//
//  Inline destructor so that the stack will be cleared.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlStack<T>::~SlStack()
{
  // Clear the list.
  this->clear();
}


//////////////////////////////////////////////////////////////////////////
//
//  Push the given element onto the stack. This does the same 
//  thing as std::push ( const T &val ) but the above CadKit::SlStack::push() 
//  hides the base class's function. This is here to please VC++.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlStack<T>::push ( const T &val )
{
#ifdef _WIN32
  _sequence.push_back ( val );
#else
  std::stack<T>::push ( val ); // Can't you do this VC++?
#endif
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove "val" from the stack, no matter where it is. Return true if 
//  "val" was found and removed.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlStack<T>::remove ( const T &val )
{
  for ( std::deque<T>::iterator i = _sequence.begin(); i < _sequence.end(); ++i )
  {
    if ( *i == val )
    {
      _sequence.erase ( i );
      return true;
    }
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove all occurances of "val" from the stack, Return the number of 
//  times "val" was removed.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlInt32 SlStack<T>::removeAll ( const T &val )
{
  SlInt32 count = 0;
  std::deque<T>::iterator i = _sequence.begin();

  // See note 3813d8e0-8836-11d3-9843-0040054c86c7.

  while ( i != _sequence.end() )
  {
    if ( *i == val )
    {
      i = _sequence.erase ( i );
      ++count;
    }

    else ++i;
  }

  return count;
}


//////////////////////////////////////////////////////////////////////////
//
//  Is it anywhere on the stack?
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlStack<T>::isOnStack ( const T &val ) const
{
  for ( std::deque<T>::const_iterator i = _sequence.begin(); i < _sequence.end(); ++i )
  {
    if ( *i == val ) return true;
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the top of the stack. These do the same thing as std::stack::top() 
//  with the exception of the assert.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlStack<T>::top()
{
  SL_ASSERT ( _sequence.size() > 0 ); 
  return ( _sequence.back() );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the top of the stack. These do the same thing as std::stack::top() 
//  with the exception of the assert.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlStack<T>::top() const
{
  SL_ASSERT ( _sequence.size() > 0 );
  return ( _sequence.back() );
}


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_STACK_H_
