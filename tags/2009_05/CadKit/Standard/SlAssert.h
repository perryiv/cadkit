
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlAssert: Defines some assertion macros.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_ASSERTION_DEFINITIONS_H_
#define _CADKIT_STANDARD_LIBRARY_ASSERTION_DEFINITIONS_H_


//////////////////////////////////////////////////////////////////////////
//
// Define assert and verify.
//
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
# ifdef _WIN32
#  include <crtdbg.h>
#  define SL_ASSERT _ASSERT	  // Default assertion behavior is ok.
# else // Unix.
#  include <stdio.h>
#  define SL_ASSERT(exp) if ( !(exp) ) { ::fprintf ( stderr, "Assertion failed at line: %d, file: %s\n", __LINE__, __FILE__ ); ::fflush ( stderr ); }
# endif
# define SL_VERIFY SL_ASSERT    // Make verify do the same thing as assert.
#else // Release.
# define SL_ASSERT(exp) ((void)0)  // Assert goes away.
# define SL_VERIFY(f) ((void)(f))  // Verify executes the function.
#endif


//////////////////////////////////////////////////////////////////////////
//
// Use this like SL_VERIFY but with functions that return void.
//
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
# define SL_DEBUG(exp) exp
#else
# define SL_DEBUG(exp) ((void)0)
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Use this instead of SL_VERIFY ( SUCCEEDED ( exp ) ), because you don't want 
// the SUCCEEDED macro included in the code in Release builds.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
  #define SL_VERIFY_SUCCEEDED(exp) SL_VERIFY(SUCCEEDED(exp))
#else
  #define SL_VERIFY_SUCCEEDED(exp) exp
#endif


#endif // _CADKIT_STANDARD_LIBRARY_ASSERTION_DEFINITIONS_H_
