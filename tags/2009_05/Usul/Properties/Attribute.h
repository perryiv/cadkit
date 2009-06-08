
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Attribute classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PROPERTIES_ATTRIBUTE_CLASSES_H_
#define _USUL_PROPERTIES_ATTRIBUTE_CLASSES_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


namespace Usul {
namespace Properties {


template
<
  class T, 
  class BaseClassType = Usul::Base::Referenced,
  class RefConfig = Usul::Pointers::Configs::RefCountingNullOk
>
class Attribute : public BaseClassType
{
public:

  /// Typedefs.
  typedef BaseClassType BaseClass;
  typedef T ValueType;
  typedef Attribute < ValueType, BaseClass, RefConfig > ThisType;
  typedef Usul::Pointers::SmartPointer < Attribute, RefConfig > RefPtr;

  /// Smart-pointer definitions.
  //USUL_DECLARE_REF_POINTERS ( Attribute );

  // Constructors
  explicit Attribute() : BaseClass()  {}
  Attribute ( const Attribute &m ) : BaseClass ( m ), _value ( m._value ){}
  Attribute ( const T &t ) : BaseClass(), _value ( t ){}

  /// Type id.
  const std::type_info &  typeId() const { return typeid ( *this ); }

  // Set/get to the data.
  const ValueType &       value() const { return _value; }
  ValueType &             value()       { return _value; }
  void                    value ( const ValueType &v ) { _value = v; }

protected:

  virtual ~Attribute()
  {
  }

private:

  // No assignment.
  Attribute &             operator = ( const Attribute &rhs );

  ValueType _value;
};


} // namespace Properties
} // namespace Usul


#endif // _USUL_PROPERTIES_ATTRIBUTE_CLASSES_H_
