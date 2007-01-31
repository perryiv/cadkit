
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error policy classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_ERROR_POLICY_CLASSES_H_
#define _XML_ERROR_POLICY_CLASSES_H_

#ifdef _WIN32
# include <crtdbg.h>
# define XML_ASSERT_MACRO _ASSERT
#else
# include <cassert>
# define XML_ASSERT_MACRO assert
#endif


namespace XML {
namespace Error {


///////////////////////////////////////////////////////////////////////////////
//
//  Assert policy.
//
///////////////////////////////////////////////////////////////////////////////

template < bool doit > struct Assert;
template <> struct Assert < true >
{
  void operator () ( bool state ) const
  {
    XML_ASSERT_MACRO ( state );
  }
};
template <> struct Assert < false >
{
  void operator () ( bool state ) const {}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Exception class.
//
///////////////////////////////////////////////////////////////////////////////

struct Exception{};


///////////////////////////////////////////////////////////////////////////////
//
//  Thrower policy.
//
///////////////////////////////////////////////////////////////////////////////

template < bool doit > struct Thrower;
template <> struct Thrower < true >
{
  void operator () ( bool state ) const
  {
    if ( !state )
    {
      throw Exception();
    }
  }
};
template <> struct Thrower < false >
{
  void operator () ( bool state ) const {}
};


#if 0 // ( _MSC_VER > 1200 ) // More recent than VC++ 6.0 SP 5


///////////////////////////////////////////////////////////////////////////////
//
//  Callback policy. Uses partial template specialization. 
//  Won't work with VC6.
//
///////////////////////////////////////////////////////////////////////////////

template < class Function, class Data, bool doit > struct Callback;
template < class Function, class Data > struct Callback < Function, Data, true >
{
  Callback<true> : _function ( 0 ), _data ( 0 ){}
  void operator () ( bool state ) const
  {
    if ( !state && _function )
    {
      _function ( _data );
    }
  }
  void setCallback ( Function function, const Data &data )
  {
    _function = function;
    _data = data;
  }
protected:
  Function _function;
  Data _data;
};
template < class Function, class Data > struct Callback < Function, Data, false >
{
  void operator () ( bool state ) const {}
  void setCallback ( Function function, const Data &data ){}
};


#endif // _MSC_VER


///////////////////////////////////////////////////////////////////////////////
//
//  A pair of policies.
//
///////////////////////////////////////////////////////////////////////////////

template < class PolicyA, class PolicyB > struct Pair
{
  void operator () ( bool state ) const
  {
    if ( !state )
    {
      PolicyA() ( state );
      PolicyB() ( state );
    }
  }
};


}; // namespace Error
}; // namespace XML


#undef XML_ASSERT_MACRO


#endif // _XML_ERROR_POLICY_CLASSES_H_
