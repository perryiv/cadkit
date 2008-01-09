
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Concrete proxy class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TYPES_OBJECT_PROXY_CLASS_
#define _USUL_TYPES_OBJECT_PROXY_CLASS_

#include "Usul/Types/BaseProxy.h"


namespace Usul {
namespace Types {


///////////////////////////////////////////////////////////////////////////////
//
//  Base proxy class.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > class ObjectProxy : public Usul::Types::BaseProxy
{
public:

  typedef Usul::Types::BaseProxy BaseClass;
  typedef T ValueType;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef BaseClass::TypeId TypeId;

  USUL_DECLARE_REF_POINTERS ( ObjectProxy );

  ObjectProxy ( const std::string &name, Mutex &mutex, TypeId type, T &value ) :
    BaseClass ( name, mutex, type ),
    _value ( value )
  {
  }

  ValueType value() const
  {
    Guard guard ( this );
    return _value;
  }

  void value ( const ValueType &t )
  {
    Guard guard ( this );
    _value = t;
  }

protected:

  virtual ~ObjectProxy();

private:

  ValueType &_value;
};


///////////////////////////////////////////////////////////////////////////////
//
//  List of known types. Add others as needed.
//
///////////////////////////////////////////////////////////////////////////////

const BaseProxy::TypeId TYPE_DOUBLE         ( 2905187851 );
const BaseProxy::TypeId TYPE_FLOAT          ( 1641229961 );
const BaseProxy::TypeId TYPE_UNSIGNED_CHAR  ( 1493254747 );
const BaseProxy::TypeId TYPE_UNSIGNED_SHORT ( 4275124916 );
const BaseProxy::TypeId TYPE_UNSIGNED_INT   ( 9357890000 );
const BaseProxy::TypeId TYPE_UNSIGNED_LONG  ( 2345018758 );
const BaseProxy::TypeId TYPE_CHAR           ( 3231123395 );
const BaseProxy::TypeId TYPE_SHORT          ( 2108859000 );
const BaseProxy::TypeId TYPE_INT            ( 2129957361 );
const BaseProxy::TypeId TYPE_LONG           ( 3257193933 );
const BaseProxy::TypeId TYPE_STD_STRING     ( 1613255575 );


} // namespace Types
} // namespace Usul


#endif // _USUL_TYPES_OBJECT_PROXY_CLASS_
