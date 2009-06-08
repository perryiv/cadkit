
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenient macros to control usage of tracing.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DEBUG_TRACE_MACROS_H_
#define _USUL_DEBUG_TRACE_MACROS_H_

#include "Usul/Strings/Format.h"
#include "Usul/Trace/Print.h"
#include "Usul/Trace/Scope.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Define macros for tracing.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef USUL_USE_TRACING 

#define USUL_TRACE_1(exp)\
  Usul::Trace::Print::execute ( exp )

#define USUL_TRACE_2(exp1,exp2)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2 ) )

#define USUL_TRACE_3(exp1,exp2,exp3)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3 ) )

#define USUL_TRACE_4(exp1,exp2,exp3,exp4)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3, exp4 ) )

#define USUL_TRACE_5(exp1,exp2,exp3,exp4,exp5)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3, exp4, exp5 ) )

#define USUL_TRACE_6(exp1,exp2,exp3,exp4,exp5,exp6)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3, exp4, exp5, exp6 ) )

#define USUL_TRACE_7(exp1,exp2,exp3,exp4,exp5,exp6,exp7)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3, exp4, exp5, exp6, exp7 ) )

#define USUL_TRACE_8(exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3, exp4, exp5, exp6, exp7, exp8 ) )

#define USUL_TRACE_9(exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8,exp9)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3, exp4, exp5, exp6, exp7, exp8, exp9 ) )

#define USUL_TRACE_10(exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8,exp9,exp10)\
  Usul::Trace::Print::execute ( Usul::Strings::format ( exp1, exp2, exp3, exp4, exp5, exp6, exp7, exp8, exp9, exp10 ) )

#ifdef _MSC_VER
#define __USUL_FUNCTION__ __FUNCTION__
#else
#define __USUL_FUNCTION__ __PRETTY_FUNCTION__
#endif

#define USUL_TRACE_SCOPE\
  Usul::Trace::Scope trace_scope ( this, __USUL_FUNCTION__ )

#define USUL_TRACE_SCOPE_STATIC\
  Usul::Trace::Scope trace_scope ( __USUL_FUNCTION__ )

#else

#define USUL_TRACE_1(exp)
#define USUL_TRACE_2(exp1,exp2)
#define USUL_TRACE_3(exp1,exp2,exp3)
#define USUL_TRACE_4(exp1,exp2,exp3,exp4)
#define USUL_TRACE_5(exp1,exp2,exp3,exp4,exp5)
#define USUL_TRACE_6(exp1,exp2,exp3,exp4,exp5,exp6)
#define USUL_TRACE_7(exp1,exp2,exp3,exp4,exp5,exp6,exp7)
#define USUL_TRACE_8(exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8)
#define USUL_TRACE_9(exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8,exp9)
#define USUL_TRACE_10(exp1,exp2,exp3,exp4,exp5,exp6,exp7,exp8,exp9,exp10)
#define USUL_TRACE_SCOPE
#define USUL_TRACE_SCOPE_STATIC

#endif

#endif // _USUL_DEBUG_TRACE_MACROS_H_
