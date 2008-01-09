
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functor to return the incremental value.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTORS_INCREMENT_H_
#define _USUL_FUNCTORS_INCREMENT_H_


namespace Usul {
namespace Functors {
namespace General {


template < class T > struct Increment
{
  Increment ( T first ) : _value ( first ){}
  T operator () ()
  {
    T current ( _value );
    ++_value;
    return current;
  }
private:
  T _value;
};


} // namespace General
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTORS_INCREMENT_H_
