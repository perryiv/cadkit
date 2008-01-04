
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This header file should not be included in client code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MEASURE_TOOL_COMPILE_GUARD_H_
#define _MEASURE_TOOL_COMPILE_GUARD_H_

#ifndef _COMPILING_MEASURE_TOOL
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#endif // _MEASURE_TOOL_COMPILE_GUARD_H_
