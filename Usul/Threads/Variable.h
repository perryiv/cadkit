
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenience class for variables that need an accompanying mutex.
//  It does no locking or guarding, that is up to the client.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_VARIABLE_WITH_MUTEX_CLASS_H_
#define _USUL_THREADS_VARIABLE_WITH_MUTEX_CLASS_H_

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

namespace Usul {
namespace Threads {


template < class ValueType > struct Variable
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ValueType value_type;
  typedef Usul::Threads::Mutex MutexType;
  typedef Usul::Threads::Guard<MutexType> GuardType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors
  //
  /////////////////////////////////////////////////////////////////////////////

  Variable() : _value(), _mutex ( 0x0 )
  {
  }
  Variable ( const ValueType &v ) : _value ( v ), _mutex ( 0x0 )
  {
  }
  Variable ( const Variable &v ) : _value ( v._value ), _mutex ( 0x0 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Variable()
  {
    delete _mutex;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment
  //
  /////////////////////////////////////////////////////////////////////////////

  Variable &operator = ( const Variable &v )
  {
    _value = v._value;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment
  //
  /////////////////////////////////////////////////////////////////////////////

  Variable &operator = ( const ValueType &v )
  {
    _value = v;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  const ValueType &value() const
  {
    return _value;
  }
  ValueType &value()
  {
    return _value;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access to the mutex. Delayed creation.
  //
  /////////////////////////////////////////////////////////////////////////////

  MutexType &mutex() const
  {
    if ( 0x0 == _mutex )
      _mutex = MutexType::create();
    return *_mutex;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typecast operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  operator ValueType &()
  {
    return this->value();
  }
  operator const ValueType &() const
  {
    return this->value();
  }

private:

  ValueType _value;
  mutable MutexType *_mutex;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_VARIABLE_WITH_MUTEX_CLASS_H_
