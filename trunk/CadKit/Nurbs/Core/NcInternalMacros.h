
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

#define NCSDTA  class P, class C, class I, class B
#define NCSDCA P, C, I, B
#define NCSDTCD NCSDTA // class P = double, class C = double, class I = unsigned int, class B = unsigned int


///////////////////////////////////////////////////////////////////////////////
//
//  Used to declare the typedefs inside of the template classes.
//
///////////////////////////////////////////////////////////////////////////////

#define DECLATE_TYPEDEFS \
  typedef I IndexType; \
  typedef B BitMaskType; \
  typedef P ParameterType; \
  typedef C ControlPointType; \
  typedef std::vector<IndexType> IndexArray; \
  typedef SlPartitionedVector<I,C> ControlPointArray; \
  typedef SlPartitionedVector<I,P> ParameterArray



#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_DATA_CLASS_H_
