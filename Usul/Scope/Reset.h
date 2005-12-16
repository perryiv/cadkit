
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Sets/resets a value in the constructor/destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SET_FOR_THIS_SCOPE_H_
#define _USUL_SET_FOR_THIS_SCOPE_H_

#include "Usul/Adaptors/MemberFunction.h"


namespace Usul {
namespace Scope {


template < typename T1, typename T2 = T1, typename T3 = T1 > struct Reset
{
  Reset ( T1 &t, const T2 &v1, const T3 &v2 ) : 
    _t     ( t ),
    _orig  ( v1 ),
    _final ( v2 )
  {
    _t = _orig;
  }

  ~Reset()
  {
    _t = _final;
  }

private:

  T1 &_t;
  T2 _orig;
  T3 _final;
};

#if 0
//It would be nice if these worked

template < class Function, typename T1, typename T2 = T1 >
struct FunctionReset
{
  FunctionReset( Function *fun, const T1& v1, const T2& v2 ) :
    _fun   ( fun ),
    _orig  ( v1 ),
    _final ( v2 )
  {
    (*_fun) ( _orig );
  }
  ~FunctionReset()
  {
    (*_fun) ( _final );
  }
private:
  Function *_fun;
  T1 _orig;
  T2 _final;
};


template < class ObjectType, class FunctionType, typename T1, typename T2 = T1 >
struct MemberFunctionReset 
{
  MemberFunctionReset ( ObjectType o, FunctionType f, const T1& v1, const T2& v2 ) :
  _fun ( o, f ),
  _orig ( v1 ),
  _final ( v2 )
  {
    _fun ( _orig );
  }
  ~MemberFunctionReset()
  {
    _fun ( _final );
  }
private:
  Usul::Adaptors::MemberFunction<ObjectType, FunctionType> _fun;
  T1 _orig;
  T2 _final;
};
#endif

}; // namespace Scope
}; // namespace Usul


#endif // _USUL_SET_FOR_THIS_SCOPE_H_
