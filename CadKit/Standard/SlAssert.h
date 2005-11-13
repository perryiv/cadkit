
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
#   include <crtdbg.h>
#   define SL_ASSERT _ASSERT	  // Default assertion behavior is ok.
# else // Unix.
#   include <stdio.h>
#   define SL_ASSERT(exp) if ( !(exp) ) { printf ( "Assertion failed at line: %s, file: %s\n", __LINE__, __FILE__ ); fflush ( stdio ); }
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
