
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Some handy smart-pointer definitions and macros.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_HANDY_SMART_POINTER_DEFINITIONS_H_
#define _USUL_HANDY_SMART_POINTER_DEFINITIONS_H_

#include "Usul/Pointers/QueryPointer.h"
#include "Usul/Pointers/Intrusive.h"
#include "Usul/Pointers/RefCountPolicy.h"
#include "Usul/Pointers/ArrayDeletePolicy.h"
#include "Usul/Pointers/NullPolicy.h"
#include "Usul/Pointers/Exceptions.h"

#include "Usul/Errors/ThrowingPolicy.h"


namespace Usul {
namespace Pointers {
namespace Configs {


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Deletes array pointers.
//  - Null is ok.
//
///////////////////////////////////////////////////////////////////////////////

struct ArrayNullOk
{
  typedef Usul::Pointers::ArrayDeletePolicy reference_policy;
  typedef Usul::Pointers::NullOkPolicy null_policy_access;
  typedef null_policy_access null_policy_assign;
  typedef null_policy_access null_policy_construct;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Deletes array pointers.
//  - Null throws an exception.
//
///////////////////////////////////////////////////////////////////////////////

struct ArrayNullThrows : public ArrayNullOk
{
  typedef Usul::Pointers::Exceptions::NullPointer exception_type;
  typedef Usul::Errors::ThrowingPolicy < exception_type > throwing_policy;
public:
  typedef Usul::Pointers::NullBadPolicy < throwing_policy > null_policy_access;
  typedef null_policy_access null_policy_assign;
  typedef null_policy_access null_policy_construct;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null is ok.
//
///////////////////////////////////////////////////////////////////////////////

struct RefCountingNullOk
{
  typedef Usul::Pointers::RefCountPolicy reference_policy;
  typedef Usul::Pointers::NullOkPolicy null_policy_access;
  typedef null_policy_access null_policy_assign;
  typedef null_policy_access null_policy_construct;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null throws an exception.
//
///////////////////////////////////////////////////////////////////////////////

class RefCountingNullThrows : public RefCountingNullOk
{
  typedef Usul::Pointers::Exceptions::NullPointer exception_type;
  typedef Usul::Errors::ThrowingPolicy < exception_type > throwing_policy;
public:
  typedef Usul::Pointers::NullBadPolicy < throwing_policy > null_policy_access;
  typedef null_policy_access null_policy_assign;
  typedef null_policy_access null_policy_construct;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null access throws an exception.
//
///////////////////////////////////////////////////////////////////////////////

class RefCountingNullAccessThrows : public RefCountingNullOk
{
  typedef Usul::Pointers::Exceptions::NullPointer exception_type;
  typedef Usul::Errors::ThrowingPolicy < exception_type > throwing_policy;
public:
  typedef Usul::Pointers::NullBadPolicy < throwing_policy > null_policy_access;
  typedef Usul::Pointers::NullOkPolicy null_policy_assign;
  typedef Usul::Pointers::NullOkPolicy null_policy_construct;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Smart-pointer configuration:
//  - Reference counting.
//  - Null throws an exception.
//  - No interface throws an exception.
//
///////////////////////////////////////////////////////////////////////////////

class RefCountingNoInterfaceThrows : public RefCountingNullOk
{
  typedef Usul::Pointers::Exceptions::NoInterface exception_type;
  typedef Usul::Errors::ThrowingPolicy < exception_type > throwing_policy;
public:
  typedef Usul::Pointers::NullBadPolicy < throwing_policy > null_policy_access;
  typedef null_policy_access null_policy_assign;
  typedef null_policy_access null_policy_construct;
};


}; // namespace Configs
}; // namespace Pointers
}; // namespace Usul



///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring an array pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_ARRAY_POINTER(class_name) \
  Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::ArrayNullOk >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring a valid reference pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_VALID_ARRAY_POINTER(class_name) \
  Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::ArrayNullThrows >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring ref-counting smart-pointer members.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_ARRAY_POINTERS(class_name) \
  typedef USUL_ARRAY_POINTER(class_name) ArrayPtr; \
  typedef USUL_VALID_ARRAY_POINTER(class_name) ValidArrayPtr


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring a reference pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REF_POINTER(class_name) \
  Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::RefCountingNullOk >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring a valid reference pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_VALID_REF_POINTER(class_name) \
  Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::RefCountingNullThrows >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring a valid-access reference pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_VALID_ACCESS_REF_POINTER(class_name) \
  Usul::Pointers::SmartPointer < class_name, Usul::Pointers::Configs::RefCountingNullAccessThrows >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring ref-counting smart-pointer members.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_REF_POINTERS(class_name) \
  typedef USUL_REF_POINTER(class_name) RefPtr; \
  typedef USUL_VALID_REF_POINTER(class_name) ValidRefPtr; \
  typedef USUL_VALID_ACCESS_REF_POINTER(class_name) ValidAccessRefPtr


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring a query-pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_QUERY_POINTER(class_name) \
  Usul::Pointers::QueryPointer < class_name, Usul::Pointers::Configs::RefCountingNullOk >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring valid query-pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_VALID_QUERY_POINTER(class_name) \
  Usul::Pointers::QueryPointer < class_name, Usul::Pointers::Configs::RefCountingNoInterfaceThrows >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring valid-access query-pointer.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_VALID_ACCESS_QUERY_POINTER(class_name) \
  Usul::Pointers::QueryPointer < class_name, Usul::Pointers::Configs::RefCountingNullAccessThrows >


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring query-pointer members.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_QUERY_POINTERS(class_name) \
  USUL_DECLARE_REF_POINTERS(class_name); \
  typedef USUL_QUERY_POINTER(class_name) QueryPtr; \
  typedef USUL_VALID_QUERY_POINTER(class_name) ValidQueryPtr; \
  typedef USUL_VALID_ACCESS_QUERY_POINTER(class_name) ValidAccessQueryPtr


#endif // _USUL_HANDY_SMART_POINTER_DEFINITIONS_H_
