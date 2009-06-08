
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlScopedSet: Sets/resets a value in the constructor/destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_SET_FOR_THIS_SCOPE_H_
#define _CADKIT_STANDARD_LIBRARY_SET_FOR_THIS_SCOPE_H_


namespace CadKit
{
template < typename T1, typename T2 = T1 > struct SlScopedSet
{
  SlScopedSet ( T1 &t, T2 val ) : _t ( t )
  {
    _orig = t;
    _t = val;
  }

  ~SlScopedSet()
  {
    _t = _orig;
  }

private:

  T1 &_t;
  T1 _orig;
};
}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_SET_FOR_THIS_SCOPE_H_
