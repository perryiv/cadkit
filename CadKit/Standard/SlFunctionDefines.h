
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
//  SlFunctionDefine: Pound defines of certain functions.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_FUNCTION_POUND_DEFINES_H_
#define _CADKIT_STANDARD_LIBRARY_FUNCTION_POUND_DEFINES_H_


//////////////////////////////////////////////////////////////////////////
//
// Define the "current working directory" function.
//
//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# define SL_GETCWD ::_getcwd
#elif _LINUX
# define SL_GETCWD ::getcwd
#else // ?
TODO
#endif // Platforms


//////////////////////////////////////////////////////////////////////////
//
// Define the snprintf function.
//
//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# define SL_SNPRINTF ::_snprintf
#elif _LINUX
# define SL_SNPRINTF ::snprintf
#else // ?
TODO
#endif // Platforms


//////////////////////////////////////////////////////////////////////////
//
// Define the vsnprintf function.
//
//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# define SL_VSNPRINTF ::_vsnprintf
#elif _LINUX
# define SL_VSNPRINTF ::vsnprintf
#else // ?
TODO
#endif // Platforms


//////////////////////////////////////////////////////////////////////////
//
// Define the vsnwprintf function.
//
//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# define SL_VSNWPRINTF ::_vsnwprintf
#elif _LINUX
# ifndef _CADKIT_USE_PRECOMPILED_HEADERS
#  define __USE_ISOC99
#  include <wchar.h>
#  define SL_VSNWPRINTF ::vswprintf
# endif
#else // ?
TODO
#endif // Platforms


#endif // _CADKIT_STANDARD_LIBRARY_FUNCTION_POUND_DEFINES_H_
