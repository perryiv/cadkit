
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


namespace Usul {
namespace Threads {


template < class ValueType, class MutexType > struct Variable
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ValueType value_type;
  typedef MutexType mutex_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructors
  //
  /////////////////////////////////////////////////////////////////////////////

  Variable() : _value(), _mutex()
  {
  }
  Variable ( const ValueType &v ) : _value ( v ), _mutex()
  {
  }
  Variable ( const Variable &v ) : _value ( v._value ), _mutex()
  {
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
  //  Access to the mutex.
  //
  /////////////////////////////////////////////////////////////////////////////

  MutexType &mutex() const
  {
    return _mutex;
  }


private:

  ValueType _value;
  mutable MutexType _mutex;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_VARIABLE_WITH_MUTEX_CLASS_H_