
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

#include "Usul/Trace/Print.h"
#include "Usul/Trace/Scope.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Turn tracing on/off everywhere with this switch.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
#ifndef _USUL_TRACE
#define _USUL_TRACE
#endif
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Define macros for tracing.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_TRACE_1(exp)
#define USUL_TRACE_SCOPE
#define USUL_TRACE_SCOPE_STATIC

#ifdef _USUL_TRACE 
#ifndef _COMPILING_USUL

#undef USUL_TRACE_1
#undef USUL_TRACE_SCOPE
#undef USUL_TRACE_SCOPE_STATIC

#define USUL_TRACE_1(exp)\
  Usul::Trace::Print::execute ( exp )

#define USUL_TRACE_SCOPE\
  Usul::Trace::Scope trace_scope ( this, __FUNCTION__ )

#define USUL_TRACE_SCOPE_STATIC\
  Usul::Trace::Scope trace_scope ( __FUNCTION__ )

#endif
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  More macros based on above.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_TRACE_2(exp1,exp2)\
  USUL_TRACE_1(exp1); USUL_TRACE_1(exp2)

#define USUL_TRACE_3(exp1,exp2,exp3)\
  USUL_TRACE_1(exp1); USUL_TRACE_2(exp2,exp3)

#define USUL_TRACE_4(exp1,exp2,exp3,exp4)\
  USUL_TRACE_1(exp1); USUL_TRACE_3(exp2,exp3,exp4)

#define USUL_TRACE_5(exp1,exp2,exp3,exp4,exp5)\
  USUL_TRACE_1(exp1); USUL_TRACE_4(exp2,exp3,exp4,exp5)


#endif // _USUL_DEBUG_TRACE_MACROS_H_
