
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Assert.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ASSERT_H_
#define _USUL_ASSERT_H_


#ifdef _MSC_VER
# include <crtdbg.h>
# define USUL_ASSERT _ASSERT
#elif __sgi
# include <assert.h>
# define USUL_ASSERT assert
#elif __APPLE__
#include <stdlib.h>
#include <stdio.h>
#undef assert
#undef __assert
#define assert(e)  \
    ((void) ((e) ? 0 : __assert (#e, __FILE__, __LINE__)))
#define __assert(e, file, line) \
    (printf ("%s:%u: failed assertion `%s'\n", file, line, e), abort (), 0)
#define USUL_ASSERT assert
#else
# include <cassert>
# define USUL_ASSERT assert
#endif


#endif // _USUL_ASSERT_H_
