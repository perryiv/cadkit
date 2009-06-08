
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This header file should not be included in client code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERAL_SHADER_COMPILE_GUARD_H_
#define _GENERAL_SHADER_COMPILE_GUARD_H_

#ifndef _COMPILING_GENERAL_SHADER
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#endif // _GENERAL_SHADER_COMPILE_GUARD_H_
