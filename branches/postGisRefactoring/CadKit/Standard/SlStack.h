
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlStack: A stack class. Some platforms do not have std::stack.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_STACK_H_
#define _CADKIT_STANDARD_LIBRARY_STACK_H_

#include <list>


namespace CadKit {

template
<
  class Value,
  class ErrorPolicy,
  class Sequence = std::list<Value>
>
class SlStack
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef typename Sequence::const_iterator const_iterator;
  typedef typename Sequence::iterator iterator;
  typedef typename Sequence::value_type value_type;
  typedef typename Sequence::size_type size_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor and destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  SlStack(){}
  ~SlStack(){}


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the number of elements on the stack.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    return _sequence.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is the stack empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty() const
  {
    return _sequence.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear the stack.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    // Do this safely, some containers are fussy.
    if ( !_sequence.empty() )
      _sequence.erase ( _sequence.begin(), _sequence.end() );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Iterator access. Ideal stack usage does not require these functions. :)
  //
  /////////////////////////////////////////////////////////////////////////////

  const_iterator begin() const { return _sequence.begin(); }
  iterator       begin()       { return _sequence.begin(); }
  const_iterator end()   const { return _sequence.end(); }
  iterator       end()         { return _sequence.end(); }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Pop the top element off the stack.
  //
  /////////////////////////////////////////////////////////////////////////////

  void pop()
  {
    _errorPolicy ( !_sequence.empty(), "SlStack::pop(), stack is empty" );
    _sequence.pop_back();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push the given element onto the stack.
  //
  /////////////////////////////////////////////////////////////////////////////

  void push ( const value_type &val )
  {
    _sequence.push_back ( val );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the top of the stack.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type &top()
  {
    _errorPolicy ( !_sequence.empty(), "SlStack::top(), stack is empty" );
    return _sequence.back();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the top of the stack.
  //
  /////////////////////////////////////////////////////////////////////////////

  const value_type &top() const
  {
    _errorPolicy ( !_sequence.empty(), "SlStack::top(), stack is empty" );
    return _sequence.back();
  }


protected:

  Sequence _sequence;
  ErrorPolicy _errorPolicy;
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_STACK_H_
