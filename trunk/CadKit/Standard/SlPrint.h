
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
//  SlPrint: Debug print definitions.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_DEBUG_PRINT_DEFINES_H_
#define _CADKIT_STANDARD_LIBRARY_DEBUG_PRINT_DEFINES_H_


//////////////////////////////////////////////////////////////////////////
//
// Debug print.
//
//////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_SL_PRINT_DEBUG
# include <stdio.h>
# define SL_PRINT ::printf
# define SL_PRINT2 SL_PRINT
# define SL_PRINT3 SL_PRINT
# define SL_PRINT4 SL_PRINT
# define SL_PRINT5 SL_PRINT
# define SL_WPRINT ::wprintf
# define SL_WPRINT2 SL_WPRINT
# define SL_WPRINT3 SL_WPRINT
# define SL_WPRINT4 SL_WPRINT
# define SL_WPRINT5 SL_WPRINT
#else // No _CADKIT_SL_PRINT_DEBUG
# define SL_PRINT(exp) ((void)0)
# define SL_PRINT2(exp1,exp2) ((void)0)
# define SL_PRINT3(exp1,exp2,exp3) ((void)0)
# define SL_PRINT4(exp1,exp2,exp3,exp4) ((void)0)
# define SL_PRINT5(exp1,exp2,exp3,exp4,exp5) ((void)0)
# define SL_WPRINT(exp) ((void)0)
# define SL_WPRINT2(exp1,exp2) ((void)0)
# define SL_WPRINT3(exp1,exp2,exp3) ((void)0)
# define SL_WPRINT4(exp1,exp2,exp3,exp4) ((void)0)
# define SL_WPRINT5(exp1,exp2,exp3,exp4,exp5) ((void)0)
#endif // _CADKIT_SL_PRINT_DEBUG


#endif // _CADKIT_STANDARD_LIBRARY_DEBUG_PRINT_DEFINES_H_
