
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This header file should not be included in client code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _POSTGRES_CONNECTION_COMPILE_GUARD_H_
#define _POSTGRES_CONNECTION_COMPILE_GUARD_H_

#ifndef _COMPILING_POSTGRES_CONNECTION
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#endif // _POSTGRES_CONNECTION_COMPILE_GUARD_H_
