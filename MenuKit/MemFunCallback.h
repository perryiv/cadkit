
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Callback for a button that wraps a member function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_MEMBER_FUNCTION_CALLBACK_H_
#define _MENUKIT_MEMBER_FUNCTION_CALLBACK_H_

#include "MenuKit/Item.h"


namespace MenuKit {


///////////////////////////////////////////////////////////////////////////////
//
// Callback class for member functions that take two arguments.
//
///////////////////////////////////////////////////////////////////////////////

template < class O, class F > class MemFunCallback2 : public MenuKit::Callback
{
public:

  MemFunCallback2 ( O o, F f ) : _o ( o ), _f ( f )
  {
  }
  ~MemFunCallback2()
  {
  }
  void operator () ( MenuKit::Message m, MenuKit::Item *item )
  {
    if ( _o && _f )
      ((*_o).*_f) ( m, item );
  }
protected:
  MemFunCallback2 ( const MemFunCallback2 & );
  MemFunCallback2 &operator = ( const MemFunCallback2 & );
  O _o;
  F _f;
};


///////////////////////////////////////////////////////////////////////////////
//
// Callback class for member functions that do not take arguments.
//
///////////////////////////////////////////////////////////////////////////////

template < class O, class F > class MemFunCallback0 : public MenuKit::Callback
{
public:

  MemFunCallback0 ( O o, F f ) : _o ( o ), _f ( f )
  {
  }
  ~MemFunCallback0()
  {
  }
  void operator () ( MenuKit::Message m, MenuKit::Item *item )
  {
    if ( _o && _f )
      ((*_o).*_f)();
  }
protected:
  MemFunCallback0 ( const MemFunCallback0 & );
  MemFunCallback0 &operator = ( const MemFunCallback0 & );
  O _o;
  F _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to create button callbacks.
//
///////////////////////////////////////////////////////////////////////////////

template < class O, class F > MemFunCallback2 < O, F > *memFunCB2 ( O o, F f )
{
  return new MemFunCallback2 < O, F > ( o, f );
}
template < class O, class F > MemFunCallback0 < O, F > *memFunCB0 ( O o, F f )
{
  return new MemFunCallback0 < O, F > ( o, f );
}


}; // namespace MenuKit


#endif // _MENUKIT_MEMBER_FUNCTION_CALLBACK_H_
