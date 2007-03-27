
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This header file should not be included in client code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ANIMATE_COMPILE_GUARD_H_
#define _ANIMATE_COMPILE_GUARD_H_

#ifndef _COMPILING_ANIMATE
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#endif // _ANIMATE_COMPILE_GUARD_H_
