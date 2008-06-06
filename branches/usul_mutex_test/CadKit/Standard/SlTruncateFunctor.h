
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlTruncateFunctor: Functor that truncates numbers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TRUNCATE_FUNCTOR_CLASS_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TRUNCATE_FUNCTOR_CLASS_CLASS_H_

#include "SlAssert.h"
#include "SlVec4.h"
#include "SlVec3.h"
#include "SlVec2.h"
#include "SlTruncate.h"


namespace CadKit
{
template<class T> class SlTruncateFunctor
{
public:

  SlTruncateFunctor ( const T &low, const T &value, const T &high ) : 
    _low   ( low ), 
    _value ( value ), 
    _high  ( high )
  {
    SL_ASSERT ( _low <= _value && _value <= _high );
  }

  SlTruncateFunctor ( const SlTruncateFunctor &t ) : 
    _low   ( t._low ), 
    _value ( t._value ), 
    _high  ( t._high )
  {
    SL_ASSERT ( _low <= _value && _value <= _high );
  }

  SlTruncateFunctor &operator = ( const SlTruncateFunctor &t )
  {
    _low   = t._low; 
    _value = t._value; 
    _high  = t._high;
    SL_ASSERT ( _low <= _value && _value <= _high );
    return *this;
  }

  void operator () ( T &value )
  {
    _truncate ( value );
  }

  void operator () ( SlVec2<T> &vec )
  {
    _truncate ( vec[0] );
    _truncate ( vec[1] );
  }

  void operator () ( SlVec3<T> &vec )
  {
    _truncate ( vec[0] );
    _truncate ( vec[1] );
    _truncate ( vec[2] );
  }

  void operator () ( SlVec4<T> &vec )
  {
    _truncate ( vec[0] );
    _truncate ( vec[1] );
    _truncate ( vec[2] );
    _truncate ( vec[3] );
  }

  const T &getLow()   const { return _low; }
  const T &getValue() const { return _value; }
  const T &getHigh()  const { return _high; }

  void setLow   ( const T &low )   { _low   = low; }
  void setValue ( const T &value ) { _value = value; }
  void setHigh  ( const T &high )  { _high  = high; }

protected:

  T _low;
  T _value;
  T _high;

  void _truncate ( T &value )
  {
    SL_ASSERT ( _low <= _value && _value <= _high );
    CadKit::truncate ( _low, _value, _high, value );
  }
};


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_TRUNCATE_FUNCTOR_CLASS_CLASS_H_
