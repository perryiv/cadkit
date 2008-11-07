
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class with virtual type-id member.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TYPED_BASE_CLASS_H_
#define _USUL_TYPED_BASE_CLASS_H_

#include "Usul/Config/Config.h"
#include "Usul/Export/Export.h"

#include <typeinfo>


namespace Usul {
namespace Base {


struct USUL_EXPORT Typed
{
  static const std::type_info &     classTypeId();
  virtual const std::type_info &    typeId() const = 0;

protected:

  // Make gcc happy.
  virtual ~Typed(){}
};


} // namespace Base
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience macros so that classes can easily implement typeId().
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_TYPE_ID(class_name) \
  static const std::type_info &classTypeId(); \
  virtual const std::type_info &typeId() const

#define USUL_IMPLEMENT_TYPE_ID(class_name) \
  const std::type_info &class_name::classTypeId() \
  { \
    return typeid ( class_name ); \
  } \
  const std::type_info &class_name::typeId() const \
  { \
    return class_name::classTypeId(); \
  }

#define USUL_IMPLEMENT_INLINE_TYPE_ID(class_name) \
  static const std::type_info &classTypeId() \
  { \
    return typeid ( class_name ); \
  } \
  virtual const std::type_info &typeId() const \
  { \
    return class_name::classTypeId(); \
  }


#endif // _USUL_TYPED_BASE_CLASS_H_
