
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlPrint: Debug print definitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_DEBUG_PRINT_DEFINES_H_
#define _CADKIT_STANDARD_LIBRARY_DEBUG_PRINT_DEFINES_H_


///////////////////////////////////////////////////////////////////////////////
//
// Debug print.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _CADKIT_SL_PRINT_DEBUG
# include <stdio.h>
# define SL_PRINT ::printf
# define SL_PRINT2 SL_PRINT
# define SL_PRINT3 SL_PRINT
# define SL_PRINT4 SL_PRINT
# define SL_PRINT5 SL_PRINT
# define SL_PRINT6 SL_PRINT
# define SL_PRINT7 SL_PRINT
# define SL_WPRINT ::wprintf
# define SL_WPRINT2 SL_WPRINT
# define SL_WPRINT3 SL_WPRINT
# define SL_WPRINT4 SL_WPRINT
# define SL_WPRINT5 SL_WPRINT
# define SL_WPRINT6 SL_WPRINT
# define SL_WPRINT7 SL_WPRINT
#else // No _CADKIT_SL_PRINT_DEBUG
# define SL_PRINT(exp) ((void)0)
# define SL_PRINT2(exp1,exp2) ((void)0)
# define SL_PRINT3(exp1,exp2,exp3) ((void)0)
# define SL_PRINT4(exp1,exp2,exp3,exp4) ((void)0)
# define SL_PRINT5(exp1,exp2,exp3,exp4,exp5) ((void)0)
# define SL_PRINT6(exp1,exp2,exp3,exp4,exp5,exp6) ((void)0)
# define SL_PRINT7(exp1,exp2,exp3,exp4,exp5,exp6,exp7) ((void)0)
# define SL_WPRINT(exp) ((void)0)
# define SL_WPRINT2(exp1,exp2) ((void)0)
# define SL_WPRINT3(exp1,exp2,exp3) ((void)0)
# define SL_WPRINT4(exp1,exp2,exp3,exp4) ((void)0)
# define SL_WPRINT5(exp1,exp2,exp3,exp4,exp5) ((void)0)
# define SL_WPRINT6(exp1,exp2,exp3,exp4,exp5,exp6) ((void)0)
# define SL_WPRINT7(exp1,exp2,exp3,exp4,exp5,exp6,exp7) ((void)0)
#endif // _CADKIT_SL_PRINT_DEBUG


#endif // _CADKIT_STANDARD_LIBRARY_DEBUG_PRINT_DEFINES_H_
