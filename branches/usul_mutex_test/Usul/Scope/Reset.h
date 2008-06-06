
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


} // namespace Scope
} // namespace Usul


#endif // _USUL_SET_FOR_THIS_SCOPE_H_
