
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Calls push in constructor and pop.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPE_PUSH_POP_H_
#define _USUL_SCOPE_PUSH_POP_H_


namespace Usul {
namespace Scope {


template < class StackType > struct PushPop
{
  typedef typename StackType::value_type ValueType;

  PushPop ( StackType &s, ValueType &value ) : _stack ( s )
  {
    _stack.push ( value );
  }

  ~PushPop()
  {
    _stack.pop();
  }

private:

  StackType &_stack;
};


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPE_PUSH_POP_H_
