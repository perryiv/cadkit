
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This header file should not be included in client code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGULATE_COMPILE_GUARD_H_
#define _TRIANGULATE_COMPILE_GUARD_H_

#ifndef _COMPILING_TRIANGULATE
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#endif // _TRIANGULATE_COMPILE_GUARD_H_