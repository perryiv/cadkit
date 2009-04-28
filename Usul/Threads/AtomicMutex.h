
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_THREADS_ATOMIC_MUTEX_H__
#define __USUL_THREADS_ATOMIC_MUTEX_H__

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

namespace Usul {
namespace Threads {

template<class T>
class Atomic
{
public:

  Atomic() : _mutex(),
    _value()
  {
  }

  Atomic ( const Atomic& rhs ) : _mutex(),
    _value()
  {
    _value = rhs;
  }

  Atomic& operator= ( const Atomic& rhs )
  {
    _value = rhs;
  }

  ~Atomic()
  {
  }

  T fetch_and_store ( T value )
  {
    Guard guard ( _mutex );
    _value = value;
    return _value;
  }

  T fetch_and_increment()
  {
    Guard guard ( _mutex );
    ++_value;
    return _value;
  }

  T fetch_and_decrement()
  {
    Guard guard ( _mutex );
    --_value;
    return _value;
  }

  operator T() const
  {
    Guard guard ( _mutex );
    return _value;
  }

  T operator--()
  {
    return this->fetch_and_decrement();
  }

private:

  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

  mutable Mutex _mutex;
  T _value;
};


}
}

#endif // __USUL_THREADS_ATOMIC_H__
