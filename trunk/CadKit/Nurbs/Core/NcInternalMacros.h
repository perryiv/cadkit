
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcInternalMacros.h: Macros used throughout the library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_INTERNAL_MACROS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_INTERNAL_MACROS_H_

#include "Standard/SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <memory> // For std::allocator
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Return false if the given expression is false.
//
///////////////////////////////////////////////////////////////////////////////

#define NC_CHECK_EXPRESSION(exp) \
  if ( false == ( exp ) ) \
  { \
    SL_ASSERT ( 0 ); \
    return false; \
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Macro intended for checking function arguments.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_NURBS_CORE_CHECK_FUNCTION_ARGUMENTS
# define NC_CHECK_ARGUMENT NC_CHECK_EXPRESSION
#else
# define NC_CHECK_ARGUMENT SL_ASSERT
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro intended for checking function return values.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_NURBS_CORE_CHECK_FUNCTION_RESULTS
# define NC_VERIFY_RESULTS NC_CHECK_EXPRESSION
#else
# define NC_VERIFY_RESULTS SL_VERIFY
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro intended for checking memory allocation.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_NURBS_CORE_CHECK_MEMORY_ALLOCATION
# define NC_CHECK_ALLOCATION NC_CHECK_EXPRESSION
#else
# define NC_CHECK_ALLOCATION SL_ASSERT
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro intended for checking memory allocation inside of a function.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_NURBS_CORE_CHECK_MEMORY_ALLOCATION
# define NC_VERIFY_ALLOCATION NC_CHECK_EXPRESSION
#else
# define NC_VERIFY_ALLOCATION SL_VERIFY
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro intended for checking memory allocation inside of a function.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_NURBS_CORE_CHECK_THIS
# define NC_CHECK_THIS NC_CHECK_EXPRESSION
#else
# define NC_CHECK_THIS SL_ASSERT
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Used for convenience, template definitions can get long. This makes it 
//  easier to change.
//
//  Note: I want NCSDTCD to have default arguments, so that the client can 
//  just declare a curve without a type. But VC++ 6.0 doesn't seem to like 
//  this. CygWin/g++ doesn't seem to mind, however.
//
//  CYGWIN_NT-5.0 1.3.12(0.54/3/2)
//  gcc version 2.95.3-5 (cygwin special)
//
///////////////////////////////////////////////////////////////////////////////

#define NCSDTA \
  typename ParamType, \
  typename CtrPtType, \
  typename IndexType, \
  typename BitMaskType, \
  typename IndexAllocatorType, \
  typename ParameterAllocatorType, \
  typename ParameterPointerAllocatorType, \
  typename ControlPointAllocatorType, \
  typename ControlPointPointerAllocatorType

#define NCSDCA \
  ParamType, \
  CtrPtType, \
  IndexType, \
  BitMaskType, \
  IndexAllocatorType, \
  ParameterAllocatorType, \
  ParameterPointerAllocatorType, \
  ControlPointAllocatorType, \
  ControlPointPointerAllocatorType

#define NCSDTCD \
  typename ParamType = double, \
  typename CtrPtType = double, \
  typename IndexType = unsigned int, \
  typename BitMaskType = unsigned int, \
  typename IndexAllocatorType = std::allocator<IndexType>, \
  typename ParameterAllocatorType = std::allocator<ParamType>, \
  typename ParameterPointerAllocatorType = std::allocator<ParamType *>, \
  typename ControlPointAllocatorType = std::allocator<CtrPtType>, \
  typename ControlPointPointerAllocatorType = std::allocator<CtrPtType *>


///////////////////////////////////////////////////////////////////////////////
//
//  Used to declare the typedefs inside of the template classes.
//
///////////////////////////////////////////////////////////////////////////////

#define DECLARE_TYPEDEFS \
  typedef IndexType Index; \
  typedef BitMaskType BitMask; \
  typedef ParamType Parameter; \
  typedef CtrPtType ControlPoint; \
  typedef IndexAllocatorType IndexAllocator; \
  typedef ParameterAllocatorType ParameterAllocator; \
  typedef ParameterPointerAllocatorType ParameterPointerAllocator; \
  typedef ControlPointAllocatorType ControlPointAllocator; \
  typedef ControlPointPointerAllocatorType ControlPointPointerAllocator



#endif // _CADKIT_NURBS_CORE_LIBRARY_INTERNAL_MACROS_H_
