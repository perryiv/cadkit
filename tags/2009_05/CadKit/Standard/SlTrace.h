
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlTrace: Trace definitions.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TRACE_DEFINES_H_
#define _CADKIT_STANDARD_LIBRARY_TRACE_DEFINES_H_


//////////////////////////////////////////////////////////////////////////
//
// Debug trace.
//
//////////////////////////////////////////////////////////////////////////

namespace CadKit
{
// Using Win32 this will dump to the output debug window. On Unix it will dump to stdout.
void _cadkitStandardLibraryDebugTrace ( const char *formatString, ... );
};

#ifdef _DEBUG
# define SL_TRACE CadKit::_cadkitStandardLibraryDebugTrace
# define SL_TRACE2 SL_TRACE
# define SL_TRACE3 SL_TRACE
# define SL_TRACE4 SL_TRACE
# define SL_TRACE5 SL_TRACE
# define SL_TRACE6 SL_TRACE
# define SL_TRACE7 SL_TRACE
# define SL_TRACE8 SL_TRACE
# define SL_TRACE9 SL_TRACE
# define SL_TRACE10 SL_TRACE
# define SL_TRACE11 SL_TRACE
# define SL_TRACE12 SL_TRACE
# define SL_TRACE13 SL_TRACE
# define SL_TRACE14 SL_TRACE
# define SL_TRACE15 SL_TRACE
# define SL_TRACE16 SL_TRACE
# define SL_TRACE17 SL_TRACE
# define SL_TRACE18 SL_TRACE
# define SL_TRACE19 SL_TRACE
# define SL_TRACE20 SL_TRACE
#else
# define SL_TRACE(e) ((void)0)
# define SL_TRACE2(e1,e2) ((void)0)
# define SL_TRACE3(e1,e2,e3) ((void)0)
# define SL_TRACE4(e1,e2,e3,e4) ((void)0)
# define SL_TRACE5(e1,e2,e3,e4,e5) ((void)0)
# define SL_TRACE6(e1,e2,e3,e4,e5,e6) ((void)0)
# define SL_TRACE7(e1,e2,e3,e4,e5,e6,e7) ((void)0)
# define SL_TRACE8(e1,e2,e3,e4,e5,e6,e7,e8) ((void)0)
# define SL_TRACE9(e1,e2,e3,e4,e5,e6,e7,e8,e9) ((void)0)
# define SL_TRACE10(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10) ((void)0)
# define SL_TRACE11(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11) ((void)0)
# define SL_TRACE12(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12) ((void)0)
# define SL_TRACE13(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13) ((void)0)
# define SL_TRACE14(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14) ((void)0)
# define SL_TRACE15(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14,e15) ((void)0)
# define SL_TRACE16(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14,e15,e16) ((void)0)
# define SL_TRACE17(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14,e15,e16,e17) ((void)0)
# define SL_TRACE18(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14,e15,e16,e17,e18) ((void)0)
# define SL_TRACE19(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14,e15,e16,e17,e18,e19) ((void)0)
# define SL_TRACE20(e1,e2,e3,e4,e5,e6,e7,e8,e9,e10,e11,e12,e13,e14,e15,e16,e17,e18,e19,e20) ((void)0)
#endif


#endif // _CADKIT_STANDARD_LIBRARY_TRACE_DEFINES_H_
