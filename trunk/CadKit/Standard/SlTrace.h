
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
void _modelspaceStandardLibraryDebugTrace ( const char *formatString, ... );
};

#ifdef _DEBUG
#define SL_TRACE CadKit::_modelspaceStandardLibraryDebugTrace
#else
#define SL_TRACE ((void)0)
#endif
#define SL_TRACE2 SL_TRACE
#define SL_TRACE3 SL_TRACE
#define SL_TRACE4 SL_TRACE
#define SL_TRACE5 SL_TRACE


#endif // _CADKIT_STANDARD_LIBRARY_TRACE_DEFINES_H_
